#include "motorconfiguration.h"
#include "motorui.h"
#include "motorproject.h"
#include "conscommon.h"

motorconfiguration::extkey::extkey() {
}

motorconfiguration::extkey::extkey(const string &akey, const string &acmd) {
    if((keycode = string2key(keyname = akey)) == -1)
	throw (0);
    command = acmd;
}

motorconfiguration::extkey::~extkey() {
}

void motorconfiguration::extkey::exec() const {
    char *pbuf;
    string r;
    pparamslist *plist;

    plist = pparamslist_create();
    project.populateparselist(plist);
    r = pbuf = sstrparse(command.c_str(), plist);
    pparamslist_free(plist);
    delete pbuf;

    ui.logf("Executing external ~%s~", r.c_str());
    ui.externalexec(0, r);
}

string motorconfiguration::extkey::description() const {
    return keyname + " " + command;
}

bool motorconfiguration::extkey::operator == (int akeycode) const {
    return keycode == akeycode;
}

bool motorconfiguration::extkey::operator != (int akeycode) const {
    return !(*this == akeycode);
}
