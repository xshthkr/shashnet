#include <protocol.h>
#include <transport.h>

#include <stdio.h>

#define PORT 12345
#define SERVER_ADDR "127.0.0.1"
#define MAX_BUF_LEN 1024

int main() {

    ShashnetClient client;

    create_client_socket(&client, PORT, SERVER_ADDR, 10);

    start_handshake(&client);

    Packet packet, ackpacket;
    init_packet(&packet, client.seq_num, client.ack_num, "Hello!");
    send_pkt_client(&client, &packet, &ackpacket);
    
    //debug
    printf("Sent DATA packet\n");
    print_packet(&packet);
    printf("Received DATA-ACK packet\n");
    print_packet(&ackpacket);

    close_client_connection(&client);

    return 0;
}

