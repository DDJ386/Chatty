// #include<stdio.h>
// #include<sys/types.h>
// #include<sys/ioctl.h>
// #include<unistd.h>
// #include<termios.h>
// #include<stdarg.h>
// #include<signal.h>
// #include<sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8848

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[1024] = {0};
    const char *message = "Hello, server!";
    
    // 创建套接字
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址信息
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    
    // 转换IPv4地址从文本到二进制形式
    if (inet_pton(AF_INET, SERVER_ADDRESS, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }
    
    // 连接到服务器
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Connected to server\n");
    
    // 发送数据
    send(client_socket, message, strlen(message), 0);
    printf("Message sent to server: %s\n", message);
    
    // 接收服务器响应
    read(client_socket, buffer, 1024);
    printf("Server response: %s\n", buffer);
    
    // 关闭套接字
    close(client_socket);
    
    return 0;
}

