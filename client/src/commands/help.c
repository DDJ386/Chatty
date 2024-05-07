#include "display.h"
extern int vPartLine;

char manual[] = {
  "login\n"
  "signin\n"
};

void help() {
  formatPrint((struct curPosition){vPartLine, 0}, "%s", manual);
}