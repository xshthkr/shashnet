#ifndef transport
#define transport

#include <transport.h>
#include <protocol.h>

int send_packet_client(ShashnetClient *sender, char *message);
int recv_packet_server(ShashnetServer *receiver, char *message);

#endif // transport