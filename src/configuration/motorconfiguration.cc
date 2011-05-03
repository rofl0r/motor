#include "motorconfiguration.h"
#include "motormisc.h"

#include "kkfsys.h"

#include <pwd.h>
#include <sys/types.h>

motorconfiguration::motorconfiguration():
dispvcs(false), dispcomp(true), dispmake(true), debugstd(false), checktty(true) {
    motordir = (string) getenv("HOME") + "/.motor/";
    defprojectsdir = (string) getenv("HOME") + "/projects";
}

motorconfiguration::~motorconfiguration() {
}

string motorconfiguration::getmotordir() {
    return motordir;
}

string motorconfiguration::getprojectdir() {
    return motordir + "projects/";
}

string motorconfiguration::gettemplatedir(const string &aname) {
    string dname;

    if(access((dname = getmotordir() + "templates/" + aname).c_str(), X_OK))
        dname = (string) SHARE_DIR + "/templates/" + aname;

    return dname;
}

string motorconfiguration::getconfigfname(const string &afname) {
    string fname;

    if(access((fname = getmotordir() + afname).c_str(), R_OK))
	fname = (string) SHARE_DIR + "/" + afname;

    return fname;
}

void motorconfiguration::getdisplay(bool &amake, bool &acomp, bool &avcs) const {
    amake = dispmake;
    acomp = dispcomp;
    avcs = dispvcs;
}

void motorconfiguration::setdisplay(bool amake, bool acomp, bool avcs) {
    dispmake = amake;
    dispcomp = acomp;
    dispvcs = avcs;
}

bool motorconfiguration::getdebugstd() const {
    return debugstd;
}

void motorconfiguration::setdebugstd(bool adebugstd) {
    debugstd = adebugstd;
}

bool motorconfiguration::getchecktty() const {
    return checktty;
}

void motorconfiguration::setchecktty(bool achecktty) {
    checktty = achecktty;
}

bool motorconfiguration::load() {
    extkey ek;
    string sect, buf;
    ifstream f;
    bool ret = true;

    f.open((getmotordir() + "conf").c_str());
    if(f.is_open()) {
	while(getconf(sect, buf, f)) {
	    if(sect == "showvcs") dispvcs = buf == "1"; else
	    if(sect == "showmake") dispmake = buf == "1"; else
	    if(sect == "showcomp") dispcomp = buf == "1"; else
	    if(sect == "debugstd") debugstd = buf == "1"; else
	    if(sect == "defprojectsdir") defprojectsdir = buf; else
	    if(sect == "checktty") checktty = buf == "1";
	}

	f.close();
    }

    f.clear();
    f.open(getconfigfname("external").c_str());
    if(f.is_open()) {
	while(getconf(sect, buf, f)) {
	    if(sect == "keys") {
		try {
		    ek = extkey(getword(buf), buf);
		    extkeys.push_back(ek);
		} catch(...) {
		}
	    }
	}

	f.close();
    }

    readuserdetails();
    return ret;
}

void motorconfiguration::save() {
    vector<string>::const_iterator i;

    mksubdirs(conf.getmotordir());
    ofstream f((getmotordir() + "conf").c_str());

    if(f.is_open()) {
	f <<
	    "%showvcs" << endl << (dispvcs ? "1" : "0") << endl <<
	    "%showmake" << endl << (dispmake ? "1" : "0") << endl <<
	    "%showcomp" << endl << (dispcomp ? "1" : "0") << endl <<
	    "%debugstd" << endl << (debugstd ? "1" : "0") << endl <<
	    "%defprojectsdir" << endl << defprojectsdir << endl <<
	    "%checktty" << endl << (checktty ? "1" : "0") << endl;

	f.close();
    }
}

string motorconfiguration::getuserfullname() const {
    return userfullname;
}

void motorconfiguration::readuserdetails() {
    int npos;
    struct passwd *p;

    if(p = getpwuid(getuid())) {
	if(p->pw_gecos) {
	    userfullname = p->pw_gecos;
	    if((npos = userfullname.find(",")) != -1)
		userfullname.erase(npos);
	}

	if(p->pw_name && userfullname.empty()) {
	    userfullname = p->pw_name;
	}
    }
}

string motorconfiguration::getdefaultprojectsdir() const {
    return defprojectsdir;
}

void motorconfiguration::setdefaultprojectsdir(const string &ad) {
    defprojectsdir = ad;
}

vector<motorconfiguration::extkey>::const_iterator motorconfiguration::extkeybegin() const {
    return extkeys.begin();
}

vector<motorconfiguration::extkey>::const_iterator motorconfiguration::extkeyend() const {
    return extkeys.end();
}
