#include <protocol.h>
#include <transport.h>

#include <stdio.h>

#define PORT 12345
#define MAX_BUF_LEN 1024

int main() {
    
    ShashnetServer server;

    create_server_socket(&server, PORT, 20);

    accept_handshake(&server);

    Packet packet, ackpacket;
    recv_pkt_server(&server, &packet, &ackpacket);

    // debug
    printf("Received DATA packet\n");
    print_packet(&packet);
    printf("Sent DATA-ACK packet\n");
    print_packet(&ackpacket);

    close_server_connection(&server);

    return 0;
}
