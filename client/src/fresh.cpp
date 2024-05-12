#include <pthread.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <string>

#include "display.h"
using namespace std;

extern "C" {
void* fresh(void* pvoid);
void printHeadline();
void printLog();
}
extern char currentUser[32];
extern char currentChat[32];
extern map<string, int> undealData;
extern pthread_mutex_t mapLock;
extern pthread_mutex_t fileLock;
extern struct curPosition inputZoneStart, inputZoneEnd, displayZoneStart, displayZoneEnd,
    informZoneStart, informZoneEnd;

void printHeadline() {
    uint16_t padding =
        (displayZoneEnd.col - displayZoneStart.col - (uint16_t)strlen(currentChat)) / 2;
    struct curPosition headLine = {1, padding};
    positionPrint(headLine, displayZoneEnd, "\033[4m%s\033[0m", currentChat);
}

void printLog() {
    uint16_t linePerMsg = (uint16_t)128 / (displayZoneEnd.col - displayZoneStart.col) + 2;
    uint16_t maxNum = (displayZoneEnd.row - displayZoneStart.row + 1) / linePerMsg;
    string filename = getenv("HOME");
    filename += "/Chatty/client/user/";
    filename += (const char*)currentUser;
    filename += "/record/";
    filename += currentChat;
    ifstream file(filename.c_str());
    queue<string> toDisplay;
    string line;
    while (getline(file, line)) {
        toDisplay.push(line);
        if (toDisplay.size() > maxNum) {
            toDisplay.pop();
        }
    }

    int size = toDisplay.size();
    struct curPosition start = {2, 1};
    for (int i = 0; i < size; i++) {
        line = toDisplay.front();
        toDisplay.pop();
        int flag = 0;
        for (auto it = line.begin(); it != line.end(); it++) {
            if (flag == 0 && *it == ' ') {
                *it = '\n';
                flag = 1;
                continue;
            }
            if(flag == 1 && *it == '\\'){
                *it = '\n';
                continue;
            }
        }
        positionPrint(start, displayZoneEnd, "%s\n", line.c_str());
        start.row += linePerMsg;
    }
}

static void printInform() {
    clear(informZoneStart, informZoneEnd);
    struct curPosition start = informZoneStart;

    pthread_mutex_lock(&mapLock);
    for (auto it = undealData.begin(); it != undealData.end(); it++) {
        positionPrint(start, informZoneEnd, "\033[0;36m%s\t%d\033[0m\n", it->first.c_str(),
                      it->second);
        start.row++;
    }
    pthread_mutex_unlock(&mapLock);
}

void* fresh(void* pvoid) {
    (void)pvoid;

    while (1) {
        printInform();
        if (strcmp(currentChat, "") == 0) {
            sleep(1);
            continue;
        }

        pthread_mutex_lock(&mapLock);
        auto it = undealData.find(currentChat);
        if (it == undealData.end()) {
            sleep(1);
            pthread_mutex_unlock(&mapLock);
            continue;
        }
        undealData.erase(it);
        pthread_mutex_unlock(&mapLock);

        clear(displayZoneStart, displayZoneEnd);
        printHeadline();
        printLog();
        usleep(10000);
    }
}