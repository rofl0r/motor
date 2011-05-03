#include "motordist.h"
#include "motorproject.h"

motordist::motordist(const string &aname, const string &atarget) {
    name = aname;
    target = atarget;
}

motordist::~motordist() {
}

string motordist::getname() const {
    return name;
}

void motordist::exec(const string &destdir) const {
    project.dist(target, destdir);
}
