#include <protocol.h>

#include <stdio.h>

#define PORT 12345
#define MAX_BUF_LEN 1024

int main() {
    
    ShashnetServer server;

    create_server_socket(&server, PORT);

    accept_handshake(&server);

    return 0;
}
