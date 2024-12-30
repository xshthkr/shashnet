#include <protocol.h>
#include <transport.h>

#include <stdio.h>

#define PORT 12345
#define SERVER_ADDR "127.0.0.1"
#define MAX_BUF_LEN 1024

int main() {

    ShashnetClient client;

    create_client_socket(&client, PORT, SERVER_ADDR, 0);

    send_packet_client(&client, "Hello, World!");
    printf("Sent: Hello, World!\n");

    return 0;
}

