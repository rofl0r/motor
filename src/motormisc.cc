#include "motormisc.h"
#include "kkfsys.h"

string findprogram(const string &progname) {
    static string spath;

    if(spath.empty()) {
	spath = getenv("PATH") ? getenv("PATH") : "";
	if(!spath.empty()) spath += ":";
	spath += (string) getenv("HOME") + "/.motor";
    }

    return pathfind(progname, spath);
}

string getparsed(motorproject &pf, const string &placefrom) {
    pparamslist *p;
    string r;
    char *pbuf;

    p = pparamslist_create();
    pf.populateparselist(p);
    r = pbuf = sstrparse(placefrom.c_str(), p);
    delete pbuf;
    pparamslist_free(p);

    return r;
}
