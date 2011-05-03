#include "breakpoint.h"

breakpoint::breakpoint() {
    fpermanent = false;
    line = 0;
}

breakpoint::breakpoint(const string &aid, const string &afname, int aline, bool apermanent) {
    fname = afname;
    line = aline;
    id = aid;
    fpermanent = apermanent;
}

breakpoint::breakpoint(const string &afname, int aline) {
    fname = afname;
    line = aline;
    fpermanent = true;
}

breakpoint::breakpoint(const string &definition) {
    string buf, sl;
    sl = getrword(buf = definition);
    fname = buf;
    line = atol(sl.c_str());
    fpermanent = true;

    if(fname.empty() || !line) throw (0);
}

breakpoint::~breakpoint() {
}

string breakpoint::getfname() const {
    return fname;
}

int breakpoint::getline() const {
    return line;
}

bool breakpoint::permanent() const {
    return fpermanent;
}

bool breakpoint::operator == (const string &aid) const {
    return id == aid;
}

bool breakpoint::operator != (const string &aid) const {
    return !(*this == aid);
}

bool breakpoint::operator == (const pair<string, int> aloc) const {
    return (justfname(aloc.first) == justfname(fname)) && (aloc.second == line);
}

bool breakpoint::operator != (const pair<string, int> aloc) const {
    return !(*this == aloc);
}
