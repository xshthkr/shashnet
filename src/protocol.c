#include <protocol.h>
#include <transport.h>
#include <utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define assert(cond, msg, cleanup) if (!(cond)) { perror(msg); cleanup; exit(EXIT_FAILURE); }

int init_packet(Packet *packet, uint32_t seq_num, uint32_t ack_num, const char *message) {
    memset(packet, 0, sizeof(*packet));
    packet->seq_num = seq_num;
    packet->ack_num = ack_num;
    packet->checksum = checksum(message, strlen(message));
    packet->data_length = strlen(message);
    strcpy(packet->payload, message);
    return 0;
}

int validate_packet_checksum(Packet *packet) {
    return packet->checksum == checksum(packet->payload, packet->data_length);
}

void print_packet(Packet *packet) {
    printf("[SEQ:%d] [ACK:%d] [PAYLOAD:%s]\n\n", packet->seq_num, packet->ack_num, packet->payload);
    return;
}

int create_client_socket(ShashnetClient *sender, int port, const char *server_addr, int seq_num) {
    sender->server_addr_len = sizeof(sender->server_addr);

    assert((sender->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0, "Socket creation failed", NULL);
    
    sender->server_addr.sin_family = AF_INET;
    sender->server_addr.sin_port = htons(port);
    assert(inet_pton(AF_INET, server_addr, &sender->server_addr.sin_addr) > 0, "Invalid address", NULL);

    sender->seq_num = seq_num;
    sender->ack_num = 0;
    
    return 0;
}

int create_server_socket(ShashnetServer *receiver, int port, int seq_num) {
    receiver->client_addr_len = sizeof(receiver->client_addr);
    
    assert((receiver->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0, "Socket creation failed", NULL);
    
    receiver->server_addr.sin_family = AF_INET;
    receiver->server_addr.sin_addr.s_addr = INADDR_ANY;
    receiver->server_addr.sin_port = htons(port);

    receiver->seq_num = seq_num;
    receiver->ack_num = 0;
    
    assert(bind(receiver->sockfd, (struct sockaddr *)&receiver->server_addr, sizeof(receiver->server_addr)) >= 0, "Bind failed", close(receiver->sockfd));
    
    return 0;
}

int start_handshake(ShashnetClient *sender) {

    // send connection request to server
    Packet packet;
    init_packet(&packet, sender->seq_num++, 0, "SYN");
    sendto(sender->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *)&sender->server_addr, sender->server_addr_len);

    // debug
    printf("Sent SYN to %s:%d\n", inet_ntoa(sender->server_addr.sin_addr), sender->server_addr.sin_port);
    print_packet(&packet);

    // wait for server response
    recvfrom(sender->sockfd, &packet, sizeof(Packet), 0, NULL, NULL);
    assert(validate_packet_checksum(&packet), "Invalid checksum", NULL);

    // send connection response to server
    if (strcmp(packet.payload, "SYN, ACK") == 0) {
        
        // debug
        printf("Received SYN, ACK\n");
        print_packet(&packet);
        
        char *message = "ACK";
        sender->ack_num = packet.seq_num + 1;
        init_packet(&packet, sender->seq_num, sender->ack_num, message);
        sendto(sender->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *)&sender->server_addr, sender->server_addr_len);
        
        // debug
        printf("Sent ACK\n");
        print_packet(&packet);
    }

    printf("Handshake complete!\n\n");

    return 0;

}

int accept_handshake(ShashnetServer *receiver) {

    // wait for client connection request
    Packet packet;
    recvfrom(receiver->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &(receiver->client_addr), &(receiver->client_addr_len));
    assert(validate_packet_checksum(&packet), "Invalid checksum", NULL);


    // send connection response to client
    if (strcmp(packet.payload, "SYN") == 0) {
        
        // debug
        printf("Received SYN from %s:%d\n", inet_ntoa(receiver->client_addr.sin_addr), receiver->client_addr.sin_port);
        print_packet(&packet);

        char *message = "SYN, ACK";
        init_packet(&packet, receiver->seq_num++, packet.seq_num + 1, message);
        sendto(receiver->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &(receiver->client_addr), receiver->client_addr_len);
        
        // debug
        printf("Sent SYN, ACK\n");
        print_packet(&packet);
    }

    // wait for client response
    recvfrom(receiver->sockfd, &packet, sizeof(Packet), 0, NULL, NULL);
    assert(validate_packet_checksum(&packet), "Invalid checksum", NULL);
    
    if (strcmp(packet.payload, "ACK") == 0) {
        // debug
        printf("Received ACK\n");
        print_packet(&packet);
    }

    printf("Handshake complete!\n\n");

    return 0;
}

int close_client_connection(ShashnetClient *sender) {
    Packet packet;
    init_packet(&packet, sender->seq_num++, 0, "FIN");
    sendto(sender->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *)&sender->server_addr, sender->server_addr_len);

    // debug
    printf("Sent FIN\n");
    print_packet(&packet);

    recvfrom(sender->sockfd, &packet, sizeof(Packet), 0, NULL, NULL);
    assert(validate_packet_checksum(&packet), "Invalid checksum", NULL);

    if (strcmp(packet.payload, "FIN, ACK") == 0) {

        // debug
        printf("Received FIN, ACK\n");
        print_packet(&packet);
        
        char *message = "ACK";
        init_packet(&packet, sender->seq_num++, packet.seq_num + 1, message);
        sendto(sender->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *)&sender->server_addr, sender->server_addr_len);

        // debug
        printf("Sent ACK\n");
        print_packet(&packet);
    }

    printf("Connection closed!\n\n");

    close(sender->sockfd);
    return 0;
}

int close_server_connection(ShashnetServer *receiver) {
    Packet packet;
    recvfrom(receiver->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &(receiver->client_addr), &(receiver->client_addr_len));
    assert(validate_packet_checksum(&packet), "Invalid checksum", NULL);

    if (strcmp(packet.payload, "FIN") == 0) {

        // debug
        printf("Received FIN\n");
        print_packet(&packet);

        char *message = "FIN, ACK";
        init_packet(&packet, receiver->seq_num++, packet.seq_num + 1, message);
        sendto(receiver->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &(receiver->client_addr), receiver->client_addr_len);

        // debug
        printf("Sent FIN, ACK\n");
        print_packet(&packet);
    }

    recvfrom(receiver->sockfd, &packet, sizeof(Packet), 0, NULL, NULL);
    assert(validate_packet_checksum(&packet), "Invalid checksum", NULL);

    if (strcmp(packet.payload, "ACK") == 0) {
        // debug
        printf("Received ACK\n");
        print_packet(&packet);
    }

    printf("Connection closed!\n\n");

    close(receiver->sockfd);
    return 0;
}