#include "display.h"

#include <stdarg.h>
#include <stdio.h>
#include <pthread.h>

static pthread_mutex_t printLock = PTHREAD_MUTEX_INITIALIZER; 

void positionPrint(struct curPosition position, struct curPosition limit,const char *format, ...) {
    
    uint16_t col = position.col;
    uint16_t row = position.row;
    char buffer[4096];

    va_list arglist;
    va_start(arglist, format);
    vsprintf(buffer, format, arglist);
    va_end(arglist);

    pthread_mutex_lock(&printLock);
    saveCur();
    /* now print data into screen */
    setCur(row++, col);

    int lineLen = limit.col - position.col;
    char *p = buffer;
    int len = 1;
    while (*p != '\0') {
        if (*p == '\n' || len > lineLen) {
            setCur(row++, col);
            len = 0;
            if (row > limit.row) break;
        } else {
            putchar(*p);
        }
        p++;
        len++;
    }
    restoreCur();
    pthread_mutex_unlock(&printLock);
    fflush(stdout);
}

void clear(struct curPosition start, struct curPosition limit) {
    int line = limit.row - start.row;
    int colomn = limit.col - start.col;
    positionPrint(start, limit, "%*s", line * colomn, " ");
}

extern struct curPosition inputZoneStart, inputZoneEnd;
void clearInput() {
    clear(inputZoneStart, inputZoneEnd);
    setCur(inputZoneStart.row, inputZoneStart.col);
}