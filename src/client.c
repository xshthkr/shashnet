#include <protocol.h>
#include <transport.h>

#include <stdio.h>

#define PORT 12345
#define SERVER_ADDR "127.0.0.1"
#define MAX_BUF_LEN 1024

int main() {

    ShashnetClient client;
    create_client_socket(&client, PORT, SERVER_ADDR, 10);

    // start handshake
    // thread 1: send packet
    // thread 2: receive packet

    start_handshake(&client);

    send_packet_client(&client, "Hello, World 1!");
    send_packet_client(&client, "Hello, World 2!");
    send_packet_client(&client, "Hello, World 3!");

    close_client_connection(&client);

    return 0;
}

