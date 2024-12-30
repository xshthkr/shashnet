#ifndef utils
#define utils

#include <protocol.h>

#include <stdint.h>

typedef struct {
    Packet *packets;
    uint8_t front;
    uint8_t rear;
} Queue;

uint16_t checksum(const char *data, int len);

int init_queue(Queue *queue);
int enqueue(Queue *queue, Packet *packet);
int dequeue(Queue *queue, Packet *packet);
int is_empty(Queue *queue);
int is_full(Queue *queue);
Packet *front(Queue *queue);
Packet *rear(Queue *queue);
void print_queue(Queue *queue);

#endif // utils