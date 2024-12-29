#ifndef transport
#define transport

#include <transport.h>
#include <protocol.h>

int send_packet_to_receiver(Sender *sender, Packet *packet);
int send_packet_to_sender(Receiver *receiver, Packet *packet);

Packet receive_packet_from_receiver(Sender *sender);
Packet receive_packet_from_sender(Receiver *sender);

int ack_receiver_packet(Sender *sender, Packet *packet);
int ack_sender_packet(Receiver *receiver, Packet *packet);

#endif // transport