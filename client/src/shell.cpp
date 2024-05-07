#include <iostream>
#include <map>
#include <string>
using namespace std;

extern "C" void shell();

extern "C" {
void help();
}

typedef void (*pfunc)();

map<string, pfunc> commands = {
    {"help", help},
};

void shell() {
    while (1) {
        string cmd;
        cin >> cmd;
        (commands[cmd])();
    }
}
