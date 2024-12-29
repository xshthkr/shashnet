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


// client sends packet to server
// client waits for ACK
int send_pkt_client(ShashnetClient *sender, Packet *packet, Packet *ackpacket) {
    sendto(sender->sockfd, packet, sizeof(Packet), 0, (struct sockaddr *) &sender->server_addr, sender->server_addr_len);
    sender->seq_num++;
    
    // Packet ackpacket;
    recvfrom(sender->sockfd, ackpacket, sizeof(Packet), 0, (struct sockaddr *) &sender->server_addr, &sender->server_addr_len);
    
    // validate_packet_ack(packet, &ackpacket);

    return 0;
}

// server sends packet to client
// server waits for ACK
int send_pkt_server(ShashnetServer *receiver, Packet *packet) {
    sendto(receiver->sockfd, packet, sizeof(Packet), 0, (struct sockaddr *) &receiver->client_addr, receiver->client_addr_len);
    receiver->seq_num++;

    Packet ackpacket;
    recvfrom(receiver->sockfd, &ackpacket, sizeof(Packet), 0, (struct sockaddr *) &receiver->client_addr, &receiver->client_addr_len);
    
    // validate_packet_ack(packet, &ackpacket);

    return 0;
}

// client receives packet
// client ACKs server
int recv_pkt_client(ShashnetClient *sender, Packet *packet) {
    recvfrom(sender->sockfd, packet, sizeof(Packet), 0, (struct sockaddr *) &sender->server_addr, &sender->server_addr_len);

    Packet ackpacket;
    sender->ack_num = packet->seq_num + 1;
    init_packet(&ackpacket, sender->seq_num++, packet->seq_num+1, "ACK");
    sendto(sender->sockfd, &ackpacket, sizeof(Packet), 0, (struct sockaddr *) &sender->server_addr, sender->server_addr_len);

    return 0;
}

// server receives packet
// server ACKs client
int recv_pkt_server(ShashnetServer *receiver, Packet *packet, Packet *ackpacket) {
    recvfrom(receiver->sockfd, packet, sizeof(Packet), 0, (struct sockaddr *) &receiver->client_addr, &receiver->client_addr_len);

    // Packet ackpacket;
    receiver->ack_num = packet->seq_num + 1;
    init_packet(ackpacket, receiver->seq_num++, receiver->ack_num, "ACK");
    sendto(receiver->sockfd, ackpacket, sizeof(Packet), 0, (struct sockaddr *) &receiver->client_addr, receiver->client_addr_len);

    return 0;
}