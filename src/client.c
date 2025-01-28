#include <protocol.h>
#include <transport.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/epoll.h>

#define SERVER_ADDR "127.0.0.1"
#define KNOWN_SERVER_PORT 12345
#define CLIENT_RECEIVER_PORT 12346
#define MAX_BUF_LEN 1024

#define assert(cond, msg, cleanup) if (!(cond)) { perror(msg); cleanup; exit(EXIT_FAILURE); }

Shashnet clientSender, clientReceiver;
char clientNickname[10], serverNickname[10];

int p2p();
void *test_send(void *arg);
void *test_recv(void *arg);

int main() {
    p2p();
    return 0;
}

// P2P connection
int p2p() {

    // create client sender socket with server address and port
    create_client_socket(&clientSender, KNOWN_SERVER_PORT, SERVER_ADDR, 10);
    start_handshake(&clientSender);

    // send client receiver port
    char clientReceiverPort[10];
    sprintf(clientReceiverPort, "%d", CLIENT_RECEIVER_PORT);
    send_packet_client(&clientSender, clientReceiverPort);
    
    // create client receiver socket with clients own port
    create_server_socket(&clientReceiver, CLIENT_RECEIVER_PORT, 20);
    accept_handshake(&clientReceiver);

    printf("Enter nickname: ");
    fgets(clientNickname, sizeof(clientNickname), stdin);
    clientNickname[strlen(clientNickname) - 1] = '\0';

    pthread_t send_thread, recv_thread;

    send_packet_client(&clientSender, clientNickname);
    recv_packet_server(&clientReceiver, serverNickname);

    assert(pthread_create(&send_thread, NULL, test_send, &clientSender) == 0, "Send thread creation failed", close(clientSender.sockfd));
    assert(pthread_create(&recv_thread, NULL, test_recv, &clientReceiver) == 0, "Recv thread creation failed", close(clientReceiver.sockfd));

    printf("Connection established\n");

    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);

    close_client_connection(&clientSender);
    close_server_connection(&clientReceiver);

    return 0;
}

void *test_send(void *arg) {

    while(1) {

        char message[MAX_BUF_LEN];
        fgets(message, sizeof(message), stdin);
        message[strlen(message) - 1] = '\0';

        send_packet_client(&clientSender, message);

    }

    return NULL;
}

void *test_recv(void *arg) {
    while (1) {
        char message[MAX_BUF_LEN] = {0};

        recv_packet_server(&clientReceiver, message);

        printf("<%s> %s\n", serverNickname, message);
        fflush(stdout);

        usleep(100000);
    }
    return NULL;
}

// standard test
// int test2() {
    
//     create_client_socket(&client, PORT, SERVER_ADDR, 10);
//     start_handshake(&client);
//     char message[MAX_BUF_LEN];

//     send_packet_client(&client, "Hello, world! from client 1");

//     recv_packet_client(&client, message);
//     printf("Received: %s\n", message);
//     fflush(stdout);

//     send_packet_client(&client, "Hello, world! from client 2");
//     send_packet_client(&client, "Hello, world! from client 3");

//     recv_packet_client(&client, message);
//     printf("Received: %s\n", message);
//     fflush(stdout);

//     close_client_connection(&client);
    
//     return 0;
// }

// test separate ports
// int test3() {
//     return 0;
// }