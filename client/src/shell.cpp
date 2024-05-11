#include <iostream>
#include <map>
#include <string>

#include "display.h"
using namespace std;

extern "C" void shell();

extern "C" {
void help();
void touch();
void sendMsg();
void sendFile();
void logout();
}

typedef void (*pfunc)();

map<string, pfunc> commands = {
    {"help", help},
    {"touch", touch},
    {"sdmsg", sendMsg},
    {"sd", sendMsg},
    {"sdfile", sendFile},
    {"logout", logout}
};

void shell() {
    while (1) {
        string cmd;
        cin >> cmd;
        clearInput();
        pfunc pf = commands[cmd];
        if (pf != NULL)
            pf();
        else
            cout << cmd << " is not a command\n";
    }
}
