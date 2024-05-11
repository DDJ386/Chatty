#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "display.h"
#include "net.h"

extern char currentUser[32];
extern char currentChat[32];
extern struct curPosition inputZoneStart, inputZoneEnd, displayZoneStart, displayZoneEnd;

void printHeadline();
void printLog();

void sendMsg() {
    char filename[256];
    char message[128];
    FILE *fd;
    if (strcmp(currentChat, "") == 0) {
        clearInput();
        positionPrint(inputZoneStart, inputZoneEnd, "please touch who you wangt to talk first\n");
        return;
    }
    scanf(" %[^\n]%*c", message);

    // send message to server
    struct package package;
    package.method = SDMSG;
    sprintf(package.data,"%s %s", currentChat, message);
    netLock();
    sendMessage(&package);
    netUnlock();

    // print to history
    snprintf(filename, 256, "%s/Chatty/client/user/%s/record/%s", getenv("HOME"), currentUser,
             currentChat);
    fd = fopen(filename, "a");
    fprintf(fd, "%s %s\n", currentUser, message);
    fclose(fd);
    clear(displayZoneStart, displayZoneEnd);
    printHeadline();
    printLog();
}
