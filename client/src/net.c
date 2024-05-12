#include "net.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"

#define SERVER_IP "118.178.253.77"
// #define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8848

static int client_fd;

static pthread_mutex_t netlock = PTHREAD_MUTEX_INITIALIZER;

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

    struct timeval timeout;
    timeout.tv_sec = 10;  // 设置超时时间为 10 秒
    timeout.tv_usec = 0;
    if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
}

void netLock() { pthread_mutex_lock(&netlock); }
void netUnlock() { pthread_mutex_unlock(&netlock); }

int sendMessage(struct package *message) {
    // size_t len = message->length + HEADER_LEN;
    return send(client_fd, (void *)message, 4096, 0);
}

int receveMessage(void *buffer) {
    memset(buffer, 0, DATA_SEZE);
    return recv(client_fd, buffer, PACKAGE_SIZE, 0);
}

void closeConnect() { close(client_fd); }