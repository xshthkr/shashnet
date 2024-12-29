#ifndef transport
#define transport

#include <transport.h>
#include <protocol.h>

int send_pkt_client(ShashnetClient *sender, Packet *packet, Packet *ackpacket);
int send_pkt_server(ShashnetServer *receiver, Packet *packet);

int recv_pkt_client(ShashnetClient *sender, Packet *packet);
int recv_pkt_server(ShashnetServer *receiver, Packet *packet, Packet *ackpacket);

#endif // transport