#include <protocol.h>
#include <transport.h>

#include <stdio.h>

#define PORT 12345
#define MAX_BUF_LEN 1024

int main() {
    
    ShashnetServer server;

    create_server_socket(&server, PORT, 20);

    char message[1024];
    recv_packet_server(&server, message);
    printf("Received: %s\n", message);

    return 0;
}
