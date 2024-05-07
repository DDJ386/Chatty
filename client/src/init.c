#include<stdio.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<termios.h>
#include<stdint.h>
#include"display.h"

extern struct curPosition screenSize;
extern uint16_t partLine;

char logo[] = 
"       _           _   _\n"         
"      | |         | | | |\n "       
"  ___| |__   __ _| |_| |_ _   _\n "
" / __| '_ \\ / _` | __| __| | | |\n"
" | (__| | | | (_| | |_| |_| |_| |\n"
"  \\___|_| |_|\\__,_|\\__|\\__|\\__, |\n"
"                            __/ |\n"
"                           |___/\n";

static void printLogo(){
    int padding = (partLine - 34)/2;
    formatPrint((struct curPosition){padding,0}, "**********************************\n");
    formatPrint((struct curPosition){padding,2},"\033[5m        WELCOME TO CHATTY\033[0m\n");
    formatPrint((struct curPosition){padding,3},"\n%s\n",logo);
}

static void getScreenSzie(struct curPosition *screenSize) {
    struct winsize size;
    ioctl(STDIN_FILENO,TIOCGWINSZ,&size);
    screenSize->col = size.ws_col;
    screenSize->row = size.ws_row;
}

static void printPartLine() {
    partLine = screenSize.col / 8 * 5;
    for(int i = 0; i <= screenSize.row; i++) {
        setCur(i, partLine);
        printf("|");
    }
}

void init(){
    clearScreen();
    getScreenSzie(&screenSize);
    printPartLine();
    setCur(0,0);
    printLogo();
    printf("\nthe client is initialized\n");
    printf("\nconnecting to the service\n");
}
