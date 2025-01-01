#ifndef utils
#define utils

#include <protocol.h>

typedef struct PacketNode {
    Packet packet;
    struct PacketNode *next;
} PacketNode;

typedef struct {
    PacketNode *head;
    PacketNode *tail;
} PacketQueue;

uint16_t checksum(const char *data, int len);

// packet queue
int packet_queue_init(PacketQueue *queue);
int packet_enqueue(PacketQueue *queue, Packet *packet);
Packet *packet_dequeue(PacketQueue *queue, Packet *packet);
int packet_queue_is_empty(PacketQueue *queue);

#endif // utils