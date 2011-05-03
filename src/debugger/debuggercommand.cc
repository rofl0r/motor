#include "debuggercommand.h"

debuggercommand::debuggercommand(const string &atext, commandkind akind) {
    text = atext;
    kind = akind;
}

debuggercommand::~debuggercommand() {
}

string debuggercommand::get(pparamslist *plist) const {
    string r;
    char *pbuf;

    if(plist) {
	r = pbuf = sstrparse(text.c_str(), plist);
	delete pbuf;
    } else {
	r = text;
    }

    return r;
}

bool debuggercommand::operator == (const commandkind akind) const {
    return kind == akind;
}

bool debuggercommand::operator != (const commandkind akind) const {
    return !(*this == akind);
}
