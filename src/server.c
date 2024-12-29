#include <protocol.h>

#include <stdio.h>

#define PORT 12345
#define MAX_BUF_LEN 1024

int main() {
    
    Receiver receiver;
    
    create_receiver_socket(&receiver, PORT);

    accept_handshake(&receiver);

    return 0;
}
