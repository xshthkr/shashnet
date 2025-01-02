#ifndef transport
#define transport

#include <transport.h>
#include <protocol.h>

int send_packet_client(Shashnet *sender, char *data);
int recv_packet_server(Shashnet *receiver, char *data);

int send_packet_server(Shashnet *sender, char *data);
int recv_packet_client(Shashnet *receiver, char *data);

#endif // transport