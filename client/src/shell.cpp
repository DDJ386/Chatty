#include <iostream>
#include <map>
#include <string>
using namespace std;

extern "C" void shell();

extern "C"{
    void login();
    void enroll();
    void help();
}

typedef void (*pfunc)();

map<string,pfunc> commands = {
    {"help", help},
    {"login",login},
    {"register", enroll}
};

void shell() {
    while (1) {
        string cmd;
        cin >> cmd;
        (commands[cmd])();
    }
}
