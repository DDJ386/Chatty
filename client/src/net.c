#include "net.h"
#include "protocol.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8848

static int client_fd;

void connectServer() {
    struct sockaddr_in server_addr;
    // open the socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error\n");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }
    // connect the server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
}

void sendMessage(struct package *message) {
    size_t len = message->length + HEADER_LEN;
    send(client_fd, (void*)message, len, 0)
}

void receveMessage(void *buffer){
    read(client_fd, buffer, PACKAGE_SIZE);
}