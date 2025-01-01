#include <protocol.h>
#include <transport.h>

#include <stdio.h>

#define PORT 12345
#define MAX_BUF_LEN 1024

int main() {
    
    ShashnetServer server;
    create_server_socket(&server, PORT, 20);

    // thread 1: receive packet
    // if payload is "SYN", client to hash table
    //      set hash table state to 1
    //      send SYN, ACK
    //      set hash table state to 2
    //      if payload is "ACK", set hash table state to 3
    //      create connection socket
    //      enqueue "joined" message to message queue
    // if client is in hash table
    //      if payload is "FIN", set hash table state to 4
    //          send FIN, ACK
    //          set hash table state to 5
    //          if payload is "ACK", set hash table state to 6
    //          close connection
    //          remove client from hash table
    //          enqueue "left" message to message queue
    //      else enqueue payload to message queue
    // else ignore packet

    // thread 2: broadcast packet
    // if message queue is not empty
    //      dequeue message
    //      loop through hash table connection sockets
    //      send message to client

    // thread 3: print hash table every 5 seconds
    // src ip, src port, state, socket address

    accept_handshake(&server);

    char message[1024];
    recv_packet_server(&server, message);
    recv_packet_server(&server, message);
    recv_packet_server(&server, message);

    close_server_connection(&server);

    return 0;
}
