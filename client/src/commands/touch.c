#include "display.h"
#include <string.h>
extern char currentChat[32];
void printHeadline();
void printLog();
void clear();
extern struct curPosition displayZoneStart, displayZoneEnd;
void touch() {
  scanf("%32s", currentChat);
  clear(displayZoneStart, displayZoneEnd);
  printHeadline();
  printLog();
}