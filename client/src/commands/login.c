#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "display.h"
#include "net.h"
#include "protocol.h"

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

    csetattr(STDIN_FILENO, TCSANOW, &oldt);

    // send message to server
    struct package message;
    snprintf(message.data, sizeof(message.data), "%s%s", username, password);
    message.method = LOGIN;
    message.length = strlen(message.data);
    sendMessage((void*)&message);

    // receive message
    receveMessage((void *)&message);
    if(strcmp(message.data, "success")) {
      perror("login faild\n");
      exit(-1);
    }
}