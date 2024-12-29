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
} Sender;

typedef struct {
    int sockfd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
    int seq_num;
    
} Receiver;

Packet create_packet(uint32_t seq_num, uint32_t ack_num, const char *message);
int validate_packet(Packet *packet);

int create_sender_socket(Sender *sender, int port, const char *server_addr);
int create_receiver_socket(Receiver *receiver, int port);

int start_handshake(Sender *sender);
int accept_handshake(Receiver *receiver);

#endif // protocol