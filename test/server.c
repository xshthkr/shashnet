#include <protocol.h>
#include <transport.h>

#include <stdio.h>

#define PORT 12345
#define MAX_BUF_LEN 1024

int main() {
    
    ShashnetServer server;

    create_server_socket(&server, PORT, 20);

    accept_handshake(&server);

    char message[1024];
    recv_packet_server(&server, message);
    recv_packet_server(&server, message);
    recv_packet_server(&server, message);

    close_server_connection(&server);

    return 0;
}
