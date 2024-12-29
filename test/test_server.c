#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345
#define MAX_BUF_LEN 1024

#define assert(cond, msg, cleanup) if (!(cond)) { perror(msg); cleanup; exit(EXIT_FAILURE); }

void test_udp_server();
void test_tcp_server();

int main() {
    test_udp_server();
    return 0;
}

void test_udp_server() {
    int sockfd;
    char buffer[MAX_BUF_LEN];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Create a UDP socket
    assert((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0, "Socket creation failed", NULL);

    // Fill server information
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the address and port
    assert(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) >= 0, "Bind failed", close(sockfd));

    printf("[UDP] Server listening on port %d...\n", PORT);

    while (1) {
        // Receive message from client
        int n = recvfrom(sockfd, (char *)buffer, MAX_BUF_LEN, 0, (struct sockaddr *)&client_addr, &addr_len);
        buffer[n] = '\0'; // Null-terminate the received message
        printf("[UDP] Client: %s\n", buffer);

        // Send response back to client
        const char *response = "Hello from server!";
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&client_addr, addr_len);
    }

    close(sockfd);
    return;
}

void test_tcp_server() {
    int sockfd, connfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Create a TCP socket
    assert((sockfd = socket(AF_INET, SOCK_STREAM, 0)) >= 0, "Socket creation failed", NULL);

    // Fill server information
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the address and port
    assert(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) >= 0, "Bind failed", close(sockfd));

    // Listen for incoming connections
    assert(listen(sockfd, 5) >= 0, "Listen failed", close(sockfd));

    printf("[TCP] Server listening on port %d...\n", PORT);

    // Accept incoming connection
    assert((connfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len)) >= 0, "Accept failed", close(sockfd));

    // Receive message from client
    char buffer[MAX_BUF_LEN];
    int n = read(connfd, buffer, MAX_BUF_LEN);
    buffer[n] = '\0'; // Null-terminate the received message
    printf("[TCP] Client: %s\n", buffer);

    // Send response back to client
    const char *response = "Hello from server!";
    write(connfd, response, strlen(response));

    close(connfd);
    close(sockfd);
    return;
}