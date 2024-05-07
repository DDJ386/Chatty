#include<iostream>
#include<string>
#include<map>
using namespace std;

extern "C" void bfunc();
extern "C" {
    void func1();
    void func2();
    void func3();
}

typedef void (*pfunc)();

map<string, pfunc> simap{
    {"func1", func1},{"func2", func2}, {"func3", func3}
};

void bfunc() {
    string cmd;
    cin >> cmd;
    (simap[cmd])();
}
