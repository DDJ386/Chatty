#include "display.h"

char manual[] = "login\nsignin\n ";

extern struct curPosition screenSize;
extern struct curPosition displayZoneStart, displayZoneEnd;

void help() { 
    clear(displayZoneStart, displayZoneEnd);
    positionPrint(displayZoneStart, displayZoneEnd, "%s", manual); 
    }