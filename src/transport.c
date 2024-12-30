// Sending Packets:
//  - Send packet
//  - Start timer
//  - If timer expires with no ACK, send packet again
//  - If ACK received before timer expires, stop.

// Receiving Packets:
//  - If packet checksum corrupted, send prev ACK packet
//  - If duplicate packet, send ACK again
//  - If packet not received send prev ACK packet

// Implement sliding window and buffering
// poll or epoll, socket timeout, select (limited, legacy)

#include <transport.h>
#include <protocol.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


// client sends packet to server
int send_packet_client(ShashnetClient *sender, char *message) {

    /* state 1: waiting for call to send data */

    // make packet
    Packet packet;
    init_packet(&packet, sender->seq_num, sender->ack_num, message);    // checksum computed internally
    
    // send packet
    sendto(sender->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &sender->server_addr, sender->server_addr_len);
    sender->seq_num++;

    //debug
    printf("Sent DATA packet\n");
    print_packet(&packet);

    /* state 2: waiting for ACK or NAK packet */

    while (1) {
        
        // receive packet
        Packet ackpacket;
        recvfrom(sender->sockfd, &ackpacket, sizeof(Packet), 0, NULL, NULL);

        // debug
        printf("Received ACK/NAK packet\n");
        print_packet(&ackpacket);

        // if ACK received then break out of loop, else resend packet
        if (ackpacket.ack_num == sender->seq_num) {
            break;
        } else {
            
            // debug
            printf("Resending DATA packet\n");

            sendto(sender->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &sender->server_addr, sender->server_addr_len);
        }
    }

    return 0;
}

// server receives packet
int recv_packet_server(ShashnetServer *receiver, char *message) {
    
    /* state 1: receive packet from client */

    // receive packet
    Packet packet;
    recvfrom(receiver->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &receiver->client_addr, &receiver->client_addr_len);

    // debug
    printf("Received DATA packet\n");
    print_packet(&packet);

    // if packet is valid, send ACK, else send NAK
    if (validate_packet_checksum(&packet)) {
        // extract message from packet
        strcpy(message, packet.payload);
        receiver->ack_num = packet.seq_num + 1;
        init_packet(&packet, receiver->seq_num, receiver->ack_num, "ACK");
        sendto(receiver->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &receiver->client_addr, receiver->client_addr_len);
    
        // debug
        printf("Sent ACK packet\n");
        print_packet(&packet);
    
    } else {
        init_packet(&packet, receiver->seq_num, receiver->ack_num, "NAK");
        sendto(receiver->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &receiver->client_addr, receiver->client_addr_len);
    
        // debug
        printf("Sent NAK packet\n");
        print_packet(&packet);
    
    }

    return 0;
}