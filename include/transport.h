#ifndef transport
#define transport

#include <transport.h>
#include <protocol.h>

int send_packet_client(ShashnetClient *sender, char *data);
int recv_packet_server(ShashnetServer *receiver, char *data);

#endif // transport