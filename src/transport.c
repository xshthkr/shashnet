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


// client sends packet to server
// client waits for ACK
int send_packet_client(ShashnetClient *sender, char *message) {

    // make packet
    Packet packet;
    init_packet(&packet, sender->seq_num, sender->ack_num, message);
    
    // send packet
    sendto(sender->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &sender->server_addr, sender->server_addr_len);
    sender->seq_num++;

    //debug
    printf("Sent DATA packet\n");
    print_packet(&packet);

    return 0;
}

// server receives packet
// server ACKs client
int recv_packet_server(ShashnetServer *receiver, char *message) {
    
    // receive packet
    Packet packet;
    recvfrom(receiver->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &receiver->client_addr, &receiver->client_addr_len);
    // validate_packet_checksum(packet)

    // extract message
    strcpy(message, packet.payload);

    return 0;
}