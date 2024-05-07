#include "display.h"
#include <stdarg.h>

void formatPrint(struct curPosition position, char *format, ...) {
    uint16_t col = position.col;
    uint16_t row = position.row;
    char buffer[4096];

    va_list arglist;
    va_start(arglist, format);
    vsprintf(buffer, format, arglist);
    va_end(arglist);

    setCur(row++, col);

    char *p = buffer;
    while(*p != '\0') {
        if(*p == '\n'){
            setCur(row++, col);
        }
        else{
            putchar(*p);
        }
        p++;
    }
}
