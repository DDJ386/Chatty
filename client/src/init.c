#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "display.h"

extern struct curPosition screenSize;
extern uint16_t vPartLine, hPartLine;
extern void netInit();

char logo[] =
    "       _           _   _\n"
    "      | |         | | | |\n "
    "  ___| |__   __ _| |_| |_ _   _\n "
    " / __| '_ \\ / _` | __| __| | | |\n"
    " | (__| | | | (_| | |_| |_| |_| |\n"
    "  \\___|_| |_|\\__,_|\\__|\\__|\\__, |\n"
    "                            __/ |\n"
    "                           |___/\n";

static void printLogo() {
    int padding = (vPartLine - 34) / 2;
    positionPrint((struct curPosition){1, padding},
                  (struct curPosition){hPartLine - 1, vPartLine - 1},
                  "**********************************\n");
    positionPrint((struct curPosition){2, padding},
                  (struct curPosition){hPartLine - 1, vPartLine - 1},
                  "\033[5m        WELCOME TO CHATTY\033[0m");
    positionPrint((struct curPosition){3, padding},
                  (struct curPosition){hPartLine - 1, vPartLine - 1}, "\n%s\n", logo);
}

static void getScreenSzie(struct curPosition *screenSize) {
    struct winsize size;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
    screenSize->col = size.ws_col;
    screenSize->row = size.ws_row;
}

static void printPartLine() {
    vPartLine = screenSize.col / 8 * 6;
    hPartLine = screenSize.row / 8 * 7;
    for (int i = 1; i < hPartLine; i++) {
        setCur(i, vPartLine);
        printf("|");
    }
    for (int i = 1; i <= screenSize.col; i++) {
        setCur(hPartLine, i);
        printf("—");
    }
}

void enroll() {
    
}

void login() {}

static void authenticateUser() {
    char cmd[8];
    printf("\n\033[5m*\033[0mType \"enroll\" to register, \"login\" to login\n");
    saveCur();
    setCur(hPartLine + 1, 1);
    scanf("%8s", cmd);
    clearInput();
    if (strcmp(cmd, "enroll"))
        enroll();
    else if (strcmp(cmd, "login"))
        login();
}

void init() {
    clearScreen();
    getScreenSzie(&screenSize);
    printPartLine();
    setCur(0, 0);
    printLogo();
    authenticateUser();
}
