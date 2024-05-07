#include "display.h"

char manual[] = "login\nsignin\n ";

extern struct curPosition screenSize;
extern uint16_t vPartLine, hPartLine;

void help() {
    saveCur();
    formatPrint((struct curPosition){1, vPartLine + 1},
                (struct curPosition){hPartLine - 1, screenSize.col}, "%s", manual);
    restoreCur();
}