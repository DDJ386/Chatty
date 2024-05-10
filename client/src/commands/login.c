#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#include "display.h"
#include "net.h"
#include "protocol.h"

extern char currentUser[32];

void login() {
    clearInput();
    char username[32];
    char password[32];
    printf("please input your username:");
    scanf("%s", username);

    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    printf("enter your password:");
    scanf("%s", password);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    // // send message to server
    // connectServer();
    // struct package message;
    // snprintf(message.data, sizeof(message.data), "%s %s", username, password);
    // message.method = LOGIN;
    // message.length = strlen(message.data);
    // sendMessage(&message);

    // // receive message
    // receveMessage((void *)&message);
    // if (message.method != REPLY || strcmp(message.data, "success")) {
    //     perror("login faild\n");
    //     exit(-1);
    // }

    // login succeed
    // creat data file
    char cmd[256];
    char *homeDir = getenv("HOME");
    snprintf(cmd, 256, "mkdir -p %s/Chatty/client/user/%s/record", homeDir, username);
    system(cmd);
    snprintf(cmd, 256, "mkdir  %s/Chatty/client/user/%s/file", homeDir, username);
    system(cmd);

    memcpy(currentUser, username, 32);
    clearInput();
    printf("login success, press any key to continue");
    getchar();
    getchar();
}