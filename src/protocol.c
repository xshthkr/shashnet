#include <protocol.h>
#include <utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define assert(cond, msg, cleanup) if (!(cond)) { perror(msg); cleanup; exit(EXIT_FAILURE); }

Packet create_packet(uint32_t seq_num, uint32_t ack_num, const char *message) {
    Packet packet;
    packet.seq_num = seq_num;
    packet.ack_num = ack_num;
    packet.checksum = checksum(message, strlen(message));
    packet.data_length = strlen(message);
    strcpy(packet.payload, message);
    return packet;
}

int validate_packet(Packet *packet) {
    return packet->checksum == checksum(packet->payload, packet->data_length);
}

int create_sender_socket(Sender *sender, int port, const char *server_addr) {
    sender->server_addr_len = sizeof(sender->server_addr);

    assert((sender->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0, "Socket creation failed", NULL);
    
    sender->server_addr.sin_family = AF_INET;
    sender->server_addr.sin_port = htons(port);
    assert(inet_pton(AF_INET, server_addr, &sender->server_addr.sin_addr) > 0, "Invalid address", NULL);
    
    return 0;
}

int create_receiver_socket(Receiver *receiver, int port) {
    receiver->client_addr_len = sizeof(receiver->client_addr);
    
    assert((receiver->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0, "Socket creation failed", NULL);
    
    receiver->server_addr.sin_family = AF_INET;
    receiver->server_addr.sin_addr.s_addr = INADDR_ANY;
    receiver->server_addr.sin_port = htons(port);
    
    assert(bind(receiver->sockfd, (struct sockaddr *)&receiver->server_addr, sizeof(receiver->server_addr)) >= 0, "Bind failed", close(receiver->sockfd));
    
    return 0;
}

int start_handshake(Sender *sender) {

    // send connection request to server
    Packet packet = create_packet(sender->seq_num++, 0, "SYN");
    sendto(sender->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *)&sender->server_addr, sender->server_addr_len);

    printf("Sent SYN to %s:%d\n", inet_ntoa(sender->server_addr.sin_addr), sender->server_addr.sin_port);

    // wait for server response
    recvfrom(sender->sockfd, &packet, sizeof(Packet), 0, NULL, NULL);
    assert(validate_packet(&packet), "Invalid checksum", NULL);


    // send connection response to server
    if (strcmp(packet.payload, "SYN, ACK") == 0) {
        printf("Received SYN, ACK\n");
        
        char *message = "ACK";
        packet = create_packet(sender->seq_num++, packet.seq_num + 1, message);
        sendto(sender->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *)&sender->server_addr, sender->server_addr_len);
        
        printf("Sent ACK\n");
    }

    return 0;

}

int accept_handshake(Receiver *receiver) {

    // wait for client connection request
    Packet packet;
    recvfrom(receiver->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &(receiver->client_addr), &(receiver->client_addr_len));
    assert(validate_packet(&packet), "Invalid checksum", NULL);


    // send connection response to client
    if (strcmp(packet.payload, "SYN") == 0) {
        printf("Received SYN from %s:%d\n", inet_ntoa(receiver->client_addr.sin_addr), receiver->client_addr.sin_port);
        
        char *message = "SYN, ACK";
        packet = create_packet(receiver->seq_num++, packet.seq_num + 1, message);
        sendto(receiver->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &(receiver->client_addr), receiver->client_addr_len);
        
        printf("Sent SYN, ACK\n");
    }


    // wait for client response
    recvfrom(receiver->sockfd, &packet, sizeof(Packet), 0, NULL, NULL);
    assert(validate_packet(&packet), "Invalid checksum", NULL);
    
    if (strcmp(packet.payload, "ACK") == 0) {
        printf("Received ACK\n");
    }

    return 0;
}