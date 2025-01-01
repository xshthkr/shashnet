#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345
#define SERVER_ADDR "127.0.0.1"
#define MAX_BUF_LEN 1024

#define assert(cond, msg, cleanup) if (!(cond)) { perror(msg); cleanup; exit(EXIT_FAILURE); }

void test_udp_client();
void test_tcp_client();

int main() {
    test_udp_client();
    return 0;
}

void test_udp_client() {
    int sockfd;
    char buffer[MAX_BUF_LEN];
    struct sockaddr_in server_addr;

    // Create a UDP socket
    assert((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0, "Socket creation failed", NULL);

    // Fill server information
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    assert(inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr) > 0, "Invalid address", NULL);

    // Send a message to the server
    const char *message = "Hello from client!";
    sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Receive server response
    int n = recvfrom(sockfd, (char *)buffer, MAX_BUF_LEN, 0, NULL, NULL);
    buffer[n] = '\0'; // Null-terminate the received message
    printf("[UDP] Server: %s\n", buffer);

    close(sockfd);
    return;
}

void test_tcp_client() {
    int sockfd;
    struct sockaddr_in server_addr;

    // Create a TCP socket
    assert((sockfd = socket(AF_INET, SOCK_STREAM, 0)) >= 0, "Socket creation failed", NULL);

    // Fill server information
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    assert(inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr) > 0, "Invalid address", close(sockfd));

    // Connect to the server
    assert(connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) >= 0, "Connection failed", close(sockfd));

    // Send a message to the server
    const char *message = "Hello from client!";
    send(sockfd, message, strlen(message), 0);

    // Receive server response
    char buffer[MAX_BUF_LEN];
    int n = recv(sockfd, buffer, MAX_BUF_LEN, 0);
    buffer[n] = '\0'; // Null-terminate the received message
    printf("[TCP] Server: %s\n", buffer);

    close(sockfd);
    return;
}