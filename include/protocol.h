#ifndef protocol
#define protocol

#include <stdint.h>
#include <arpa/inet.h>

#define MAX_PACKET_SIZE 1024

typedef struct {
    uint32_t seq_num;
    uint32_t ack_num;
    uint16_t checksum;
    uint16_t data_length;
    char payload[MAX_PACKET_SIZE];
} Packet;

typedef struct {
    int sockfd;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    socklen_t server_addr_len;
    socklen_t client_addr_len;
    int seq_num;
    int ack_num;
} Shashnet;

int init_packet(Packet* packet, uint32_t seq_num, uint32_t ack_num, const char *message);
int validate_packet_checksum(Packet *packet);
void print_packet(Packet* packet);

int create_client_socket(Shashnet*sender, int port, const char *server_addr, int seq_num);
int create_server_socket(Shashnet*receiver, int port, int seq_num);

int start_handshake(Shashnet *sender);
int accept_handshake(Shashnet *receiver);

int close_client_connection(Shashnet *sender);
int close_server_connection(Shashnet *receiver);

// int connect_to_server_sp(ShashnetClient *sender);
// int listen_for_clients_sp(ShashnetServer *listener, ShashnetServer *connections);    

// int connect_to_server_vs(ShashnetClient *sender);
// int listen_for_clients_vs(ShashnetServer *listener);

#endif // protocol