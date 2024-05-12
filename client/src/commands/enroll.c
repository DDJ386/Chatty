#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "display.h"
#include "net.h"
#include "protocol.h"

extern void login();

void enroll() {
    clearInput();
    char username[32];
    char password1[32];
    char password2[32];
    printf("enter your username:");
    scanf("%s", username);
    saveCur();
    int flag = 0;

    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

again:
    restoreCur();
    printf("\033[0J");
    if (flag) printf("\033[0;31mThe password is not the same, please try again\033[0m\n");
    printf("enter your password:");
    scanf("%s", password1);
    printf("\nretype your password:");
    scanf("%s", password2);
    if (strcmp(password1, password2) != 0) {
        flag = 1;
        goto again;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    // send message to server
    connectServer();
    struct package message;
    snprintf(message.data, sizeof(message.data), "%s %s", username, password1);
    message.method = REGIS;
    message.length = strlen(message.data);
    sendMessage(&message);

    // receive message
    if (receveMessage((void *)&message) < 0) {
        printf("connect timeout\n");
    }
    if (message.method != REPLY || strcmp(message.data, "success")) {
        printf("\nregister faild, press any key to continue\n");
        return;
    }

    closeConnect();

    // register success
    clearInput();
    printf("register success, press any key to login");
    getchar();
    getchar();
    login();
}