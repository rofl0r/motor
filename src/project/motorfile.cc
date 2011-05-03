#include "motorfile.h"

motorfile::motorfile(const string &afname) {
    fname = afname;
}

motorfile::~motorfile() {
}

string motorfile::getfname() const {
    return fname;
}

void motorfile::remove() {
}

bool motorfile::operator == (const string &afname) const {
    return fname == afname;
}

bool motorfile::operator != (const string &afname) const {
    return !(*this == afname);
}

bool motorfile::operator == (regex_t *r) const {
    return !regexec(r, fname.c_str(), 0, 0, 0);
}

bool motorfile::operator != (regex_t *r) const {
    return !(*this == r);
}

bool motorfile::operator == (const motorfile &afile) const {
    return afile.getfname() == getfname();
}

bool motorfile::operator != (const motorfile &afile) const {
    return !(*this == afile);
}
