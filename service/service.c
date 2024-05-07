#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"

#define PORT 8888
#define IP "127.0.0.1"
#define BACKLOG 1024

int main() {
    int ServerSocket, ClientSocket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char *buffer;
    int rcv = -1;

    ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ServerSocket == -1) {
        perror("Failed to create socket");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(ServerSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Failed to bind on port");
        return -1;
    }

    if (listen(ServerSocket, BACKLOG) == -1) {
        perror("Failed to listen");
        return -1;
    }
    ClientSocket = accept(ServerSocket, (struct sockaddr *)&client_addr, &client_addr_len);
    if (ClientSocket == -1) {
        perror("fail to connection");
        return -1;
    }
    // connected
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        if (read(ClientSocket, buffer, sizeof(buffer)) <= 0) {
            close(ClientSocket);
            continue;
        }
        if (write(ClientSocket, buffer, sizeof(buffer)) < 0) {
            close(ClientSocket);
            continue;
        }
    }
}
