#include <protocol.h>

#include <stdio.h>

#define PORT 12345
#define SERVER_ADDR "127.0.0.1"
#define MAX_BUF_LEN 1024

int main() {

    Sender sender;

    create_client_socket(&sender, PORT, SERVER_ADDR);

    start_handshake(&sender);

    return 0;
}

