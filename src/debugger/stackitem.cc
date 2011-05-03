#include "motordebugger.h"

stackitem::stackitem() {
}

stackitem::stackitem(const string &aname, const string &aargs,
motorui::editfile alocation) {
    name = aname;
    args = aargs;
    location = alocation;
}

stackitem::~stackitem() {
}

string stackitem::getname() const {
    return name;
}

string stackitem::getarguments() const {
    return args;
}

const motorui::editfile stackitem::getlocation() const {
    return location;
}
