#include <protocol.h>
#include <transport.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/epoll.h>

#define PORT1 12345
#define PORT2 12346
#define MAX_BUF_LEN 1024

#define assert(cond, msg, cleanup) if (!(cond)) { perror(msg); cleanup; exit(EXIT_FAILURE); }

Shashnet serverSender, serverReceiver;
char clientNickname[10], serverNickname[10];

int test1();
void *test_send(void *arg);
void *test_recv(void *arg);

int test2();

int test3();

int main() {
    test1();
    return 0;
}

// P2P connection
int test1() {

    
    create_server_socket(&serverReceiver, PORT1, 20);
    accept_handshake(&serverReceiver);

    char client_addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(serverReceiver.client_addr.sin_addr), client_addr, INET_ADDRSTRLEN);
    create_client_socket(&serverSender, PORT2, client_addr, 10);
    start_handshake(&serverSender);

    printf("Enter nickname: ");
    fgets(serverNickname, sizeof(serverNickname), stdin);
    serverNickname[strlen(serverNickname) - 1] = '\0';

    pthread_t send_thread, recv_thread;

    recv_packet_server(&serverReceiver, clientNickname);
    send_packet_client(&serverSender, serverNickname);

    assert(pthread_create(&send_thread, NULL, test_send, &serverSender) == 0, "Send thread creation failed", close(serverSender.sockfd));
    assert(pthread_create(&recv_thread, NULL, test_recv, &serverReceiver) == 0, "Recv thread creation failed", close(serverReceiver.sockfd));

    printf("Connection established\n");

    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);

    close_server_connection(&serverSender);
    close_client_connection(&serverReceiver);

    return 0;
}

void *test_send(void *arg) {

    while(1) {

        char message[MAX_BUF_LEN];
        fgets(message, sizeof(message), stdin);
        message[strlen(message) - 1] = '\0';

        send_packet_client(&serverSender, message);

    }

    return NULL;
}

void *test_recv(void *arg) {
    while (1) {
        char message[MAX_BUF_LEN] = {0};

        recv_packet_server(&serverReceiver, message);

        printf("<%s> %s\n", clientNickname, message);
        fflush(stdout);

        usleep(100000);
    }
    return NULL;
}

// standard test
// int test2() {

//     create_server_socket(&server, PORT, 20);
//     accept_handshake(&server);
//     char message[MAX_BUF_LEN];

//     recv_packet_server(&server, message);
//     printf("Received: %s\n", message);
//     fflush(stdout);

//     send_packet_server(&server, "Hello, world! from server 1");

//     recv_packet_server(&server, message);
//     printf("Received: %s\n", message);
//     fflush(stdout);

//     recv_packet_server(&server, message);
//     printf("Received: %s\n", message);
//     fflush(stdout);

//     send_packet_server(&server, "Hello, world! from server 2");

//     close_server_connection(&server);

//     return 0;
// }


// test separate ports
// int test3() {
//     return 0;
// }