#include "display.h"

#include <stdarg.h>

void positionPrint(struct curPosition position, struct curPosition limit, char *format, ...) {
    uint16_t col = position.col;
    uint16_t row = position.row;
    char buffer[4096];

    va_list arglist;
    va_start(arglist, format);
    vsprintf(buffer, format, arglist);
    va_end(arglist);

    setCur(row++, col);

    int lineLen = limit.col - position.col;
    char *p = buffer;
    int len = 1;
    while (*p != '\0') {
        if (*p == '\n' || len > lineLen) {
            setCur(row++, col);
            len = 0;
            if (row >= limit.row) break;
        } else {
            putchar(*p);
        }
        p++;
        len++;
    }
}

void clear(struct curPosition start, struct curPosition limit) {
    saveCur();
    int line = limit.row - start.row;
    int colomn = limit.col - start.col;
    positionPrint(start, limit, "%*s", line * colomn, " ");
    restoreCur();
}

extern uint16_t hPartLine;
extern struct curPosition screenSize;
void clearInput() {
    clear((struct curPosition){hPartLine + 1, 1}, screenSize);
    setCur(hPartLine + 1, 1);
}