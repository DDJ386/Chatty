#include <stdint.h>
#include <stdio.h>

#include "display.h"

void init();
void shell();

struct curPosition screenSize;
uint16_t vPartLine, hPartLine;
struct curPosition inputZoneStart, inputZoneEnd, displayZoneStart, displayZoneEnd, informZoneStart,
    informZoneEnd;

char 

int main() {
    init();
    shell();
}
