// TODO:
// Implement sliding window and buffering

#include <transport.h>
#include <protocol.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>

#define MAX_EVENTS 1
#define TIMEOUT 1000

#define assert(cond, msg, cleanup) if (!(cond)) { perror(msg); cleanup; exit(EXIT_FAILURE); }

// client sends packet to server
int send_packet_client(Shashnet *sender, char *data) {

    /* state 1: waiting for call to send data */

    // make packet
    Packet packet;
    init_packet(&packet, sender->seq_num, sender->ack_num, data);    // checksum computed internally
    
    int epollfd, nfds;
    struct epoll_event event, events[MAX_EVENTS];

    // create epoll instance
    epollfd = epoll_create1(0);
    assert(epollfd >= 0, "epoll_create1 failed", NULL);

    // add socket to epoll instance
    event.events = EPOLLIN;
    event.data.fd = sender->sockfd;
    assert(epoll_ctl(epollfd, EPOLL_CTL_ADD, sender->sockfd, &event) >= 0, "epoll_ctl failed", close(sender->sockfd));

    int retries = 10;

    while (retries--) {
        
        // send packet
        ssize_t bytes = sendto(sender->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &sender->server_addr, sender->server_addr_len);
        assert(bytes >= 0, "sendto failed", close(sender->sockfd));

        //debug
        // printf("Sent DATA packet\n");
        // print_packet(&packet);
        // fflush(stdout);

        /* state 2: waiting for ACK or timeout */
        
        // wait for ACK or timeout
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, TIMEOUT);
        assert(nfds >= 0, "epoll_wait failed", close(sender->sockfd));
        
        // if timeout, resend packet
        if (nfds == 0) {
            continue;
        }
        
        if (events[0].data.fd == sender->sockfd) {

            // receive packet
            Packet ackpacket;
            ssize_t bytes = recvfrom(sender->sockfd, &ackpacket, sizeof(Packet), 0, NULL, NULL);
            assert(bytes >= 0, "recvfrom failed", close(sender->sockfd));

            // debug
            // printf("Received ACK/NAK packet\n");
            // print_packet(&ackpacket);
            // fflush(stdout);

            // if received packet is corrupted or duplicate ACK, resend packet
            if (!validate_packet_checksum(&ackpacket) || ackpacket.ack_num != sender->seq_num + 1) {
                continue;
            } else {    // if received packet is valid and has correct ACK number, break
                sender->ack_num = ackpacket.seq_num + 1;
                break;
            }
        }
    }

    sender->seq_num = sender->seq_num + 1;

    close(epollfd);
    return 0;
}

// server receives packet
int recv_packet_server(Shashnet *receiver, char *data) {
    
    /* state 1: wait for call to receive packet from client */

    while (1) {

        /* state 2: receive packet from client */

        // receive packet
        Packet packet;
        ssize_t bytes = recvfrom(receiver->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &receiver->client_addr, &receiver->client_addr_len);
        assert(bytes >= 0, "recvfrom failed", close(receiver->sockfd));

        // debug
        // printf("Received DATA packet\n");
        // print_packet(&packet);
        // fflush(stdout);

        // if packet is valid and has correct sequence number, send ACK
        if (validate_packet_checksum(&packet) && packet.seq_num == receiver->ack_num) {
            // extract data from packet
            strcpy(data, packet.payload);
            receiver->ack_num = packet.seq_num + 1;
            init_packet(&packet, receiver->seq_num, receiver->ack_num, "ACK");
            ssize_t bytes = sendto(receiver->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &receiver->client_addr, receiver->client_addr_len);
            assert(bytes >= 0, "sendto failed", close(receiver->sockfd));

            // debug
            // printf("Sent ACK packet\n");
            // print_packet(&packet);
            // fflush(stdout);

            break;
        
        } else {    // if packet is duplicate or invalid, resend ACK for previous packet
            init_packet(&packet, receiver->seq_num, receiver->ack_num, "ACK");
            ssize_t bytes = sendto(receiver->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &receiver->client_addr, receiver->client_addr_len);
            assert(bytes >= 0, "sendto failed", close(receiver->sockfd));

            // debug
            // printf("Sent ACK packet\n");
            // print_packet(&packet);
            // fflush(stdout);
        }
    }

    receiver->seq_num = receiver->seq_num + 1;

    return 0;
}

// server sends packet to client
int send_packet_server(Shashnet *sender, char *data) {

    /* state 1: waiting for call to send data */

    // make packet
    Packet packet;
    init_packet(&packet, sender->seq_num, sender->ack_num, data);    // checksum computed internally
    
    int epollfd, nfds;
    struct epoll_event event, events[MAX_EVENTS];

    // create epoll instance
    epollfd = epoll_create1(0);
    assert(epollfd >= 0, "epoll_create1 failed", NULL);

    // add socket to epoll instance
    event.events = EPOLLIN;
    event.data.fd = sender->sockfd;
    assert(epoll_ctl(epollfd, EPOLL_CTL_ADD, sender->sockfd, &event) >= 0, "epoll_ctl failed", close(sender->sockfd));

    int retries = 10;

    while (retries--) {
        
        // send packet
        ssize_t bytes = sendto(sender->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &sender->client_addr, sender->client_addr_len);
        assert(bytes >= 0, "sendto failed", close(sender->sockfd));

        //debug
        printf("Sent DATA packet\n");
        print_packet(&packet);
        fflush(stdout);

        /* state 2: waiting for ACK or timeout */
        
        // wait for ACK or timeout
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, TIMEOUT);
        assert(nfds >= 0, "epoll_wait failed", close(sender->sockfd));
        
        // if timeout, resend packet
        if (nfds == 0) {
            continue;
        }
        
        if (events[0].data.fd == sender->sockfd) {

            // receive packet
            Packet ackpacket;
            ssize_t bytes = recvfrom(sender->sockfd, &ackpacket, sizeof(Packet), 0, NULL, NULL);
            assert(bytes >= 0, "recvfrom failed", close(sender->sockfd));

            // debug
            printf("Received ACK/NAK packet\n");
            print_packet(&ackpacket);
            fflush(stdout);

            // if received packet is corrupted or duplicate ACK, resend packet
            if (!validate_packet_checksum(&ackpacket) || ackpacket.ack_num != sender->seq_num + 1) {
                continue;
            } else {    // if received packet is valid and has correct ACK number, break
                sender->ack_num = ackpacket.seq_num + 1;
                break;
            }
        }
    }

    sender->seq_num = sender->seq_num + 1;

    close(epollfd);
    return 0;
}

// client receives packet
int recv_packet_client(Shashnet *receiver, char *data) {
    
    /* state 1: wait for call to receive packet from server */

    while (1) {

        /* state 2: receive packet from server */

        // receive packet
        Packet packet;
        ssize_t bytes = recvfrom(receiver->sockfd, &packet, sizeof(Packet), 0, NULL, NULL);
        assert(bytes >= 0, "recvfrom failed", close(receiver->sockfd));

        // debug
        printf("Received DATA packet\n");
        print_packet(&packet);
        fflush(stdout);

        // if packet is valid and has correct sequence number, send ACK
        if (validate_packet_checksum(&packet) && packet.seq_num == receiver->ack_num) {
            // extract data from packet
            strcpy(data, packet.payload);
            receiver->ack_num = packet.seq_num + 1;
            init_packet(&packet, receiver->seq_num, receiver->ack_num, "ACK");
            ssize_t bytes = sendto(receiver->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &receiver->server_addr, receiver->server_addr_len);
            assert(bytes >= 0, "sendto failed", close(receiver->sockfd));

            // debug
            printf("Sent ACK packet\n");
            print_packet(&packet);
            fflush(stdout);

            break;
        
        } else {    // if packet is duplicate or invalid, resend ACK for previous packet
            init_packet(&packet, receiver->seq_num, receiver->ack_num, "ACK");
            ssize_t bytes = sendto(receiver->sockfd, &packet, sizeof(Packet), 0, (struct sockaddr *) &receiver->server_addr, receiver->server_addr_len);
            assert(bytes >= 0, "sendto failed", close(receiver->sockfd));

            // debug
            printf("Sent ACK packet\n");
            print_packet(&packet);
            fflush(stdout);
        }
    }

    receiver->seq_num = receiver->seq_num + 1;

    return 0;
}