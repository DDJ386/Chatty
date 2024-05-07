#include "display.h"

char manual[] = "login\nsignin\n ";

extern  struct curPosition screenSize;
extern uint16_t vPartLine, hPartLine;

void help() { 
  saveCur();
  formatPrint((struct curPosition){vPartLine + 1, 1}, screenSize, "%s", manual); 
  restorCur();
}