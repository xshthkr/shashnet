#include <utils.h>

#include <stdint.h>
#include <stdlib.h>

uint16_t checksum(const char *data, int len) {
    uint32_t sum = 0;
    for (int i = 0; i < len; i++) {
        sum += data[i];
    }
    return (uint16_t)sum;
}

int packet_queue_init(PacketQueue *queue) {
    queue->head = NULL;
    queue->tail = NULL;
    return 0;
}

int packet_enqueue(PacketQueue *queue, Packet *packet) {
    PacketNode *node = (PacketNode *)malloc(sizeof(PacketNode));
    node->packet = *packet;
    node->next = NULL;

    if (queue->head == NULL) {
        queue->head = node;
        queue->tail = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }

    return 0;
}

Packet *packet_dequeue(PacketQueue *queue, Packet *packet) {
    if (queue->head == NULL) {
        return NULL;
    }

    PacketNode *node = queue->head;
    *packet = node->packet;
    queue->head = node->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }
    
    free(node);
    return packet;
}

int packet_queue_is_empty(PacketQueue *queue) {
    return queue->head == NULL;
}