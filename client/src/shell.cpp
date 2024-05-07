#include <iostream>
#include <map>
#include <string>
using namespace std;

extern "C" void shell();

extern "C"{
    void login();
    void signin();
    void help();
}

typedef void (*pfunc)();

map<string,pfunc> commands = {
    {"help", help},
    {"login",login},
    {"signin", signin}
};

void shell() {
    while (1) {
        string cmd;
        cin >> cmd;
        (commands[cmd])();
    }
}
