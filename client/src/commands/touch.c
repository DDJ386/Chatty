#include "display.h"
#include <string.h>
extern char currentChat[32];
void printHeadline();
void printLog();
void touch() {
  scanf("%32s", currentChat);
  printHeadline();
  printLog();
}