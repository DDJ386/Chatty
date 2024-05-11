#include <pthread.h>
#include <unistd.h>

#include <fstream>
#include <map>
#include <sstream>
#include <string>

#include "display.h"
#include "net.h"

using namespace std;

extern "C" {
void *inquire(void *pvoid);
void *receiveFile(char *data);
}
extern struct curPosition informZoneStart, informZoneEnd;
extern char currentUser[32];

map<string, int> undealData;

pthread_mutex_t mapLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t fileLock = PTHREAD_MUTEX_INITIALIZER;

static void dealWith(char *buffer) {
    stringstream ss(buffer);
    while (ss.rdbuf()->in_avail() != 0) {
        string name;
        int cnt;
        ss >> name >> cnt;
        string *messages = new string[cnt];
        for (int i = 0; i < cnt; i++) {
            ss >> messages[i];
        }
        pthread_mutex_lock(&mapLock);
        undealData[name] += cnt;
        pthread_mutex_unlock(&mapLock);
        //  write to file
        string filename = getenv("HOME");
        filename += "/Chatty/client/user/";
        filename += (const char *)currentUser;
        filename += "/record/";
        filename += name;
        ofstream file(filename.c_str(), ios::app);

        pthread_mutex_lock(&fileLock);
        for (int i = 0; i < cnt; i++) {
            file << name << " " << messages[i] << '\n';
        }
        pthread_mutex_unlock(&fileLock);
        file.close();
    }
}

void *inquire(void *pvoid) {
    (void)pvoid;
    struct package message;
    while (1) {
        message.method = INQRY;
        message.length = 0;
        netLock();
        sendMessage(&message);
        receveMessage(&message);
        netUnlock();

        /* test data */
        // message.method = REPLY;
        // message.length = 1;
        // snprintf(message.data, 4096, "%s %d %s %s %s %d %s", "alex", 2, "msg5", "msg6", "bob", 1,
        //          "msg3");
        /* test data end*/

        if (message.method == REPLY && message.length != 0) {
            dealWith(message.data);
        } else if (message.method == SDFLE) {
            receiveFile(message.data);
        }
        sleep(5);
    }
}