#include <stdio.h>
#include <stdint.h>

#ifndef _DISPLAY_H
#define _DISPLAY_H 1

#define clearScreen() {printf("\033[2J");}
#define setCur(x,y) {printf("\033[%d;%dH",x,y);}

struct curPosition{
    uint16_t col, row;
};

void formatPrint(struct curPosition, char *, ...);

#endif
