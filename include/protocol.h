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
    int seq_num;
    int ack_num;
} ShashnetClient;

typedef struct {
    int sockfd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
    int seq_num;
    int ack_num;
} ShashnetServer;

int init_packet(Packet* packet, uint32_t seq_num, uint32_t ack_num, const char *message);
int validate_packet_checksum(Packet *packet);
void print_packet(Packet* packet);

int create_client_socket(ShashnetClient *sender, int port, const char *server_addr, int seq_num);
int create_server_socket(ShashnetServer *receiver, int port, int seq_num);

int start_handshake(ShashnetClient *sender);
int accept_handshake(ShashnetServer *receiver);

int close_client_connection(ShashnetClient *sender);
int close_server_connection(ShashnetServer *receiver);

#endif // protocol