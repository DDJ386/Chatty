#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

#include "display.h"

extern void init();
extern void shell();
extern void* inquire(void* pvoid);
extern void* fresh(void* pvoid);
extern void *receive(void *pvoid);

struct curPosition screenSize;
uint16_t vPartLine, hPartLine;
struct curPosition inputZoneStart, inputZoneEnd, displayZoneStart, displayZoneEnd, informZoneStart,
    informZoneEnd;

char currentUser[32];
char currentChat[32];

int main() {
    init();

    pthread_t tid;
    pthread_create(&tid, NULL, inquire, NULL);
    pthread_create(&tid, NULL, receive, NULL);
    pthread_create(&tid, NULL, fresh, NULL);
    clear(displayZoneStart, displayZoneEnd);
    shell();
}
