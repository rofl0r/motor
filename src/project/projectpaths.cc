#include "projectpaths.h"
#include "motormisc.h"
#include "motorconfiguration.h"

projectpaths::projectpaths() {
}

projectpaths::projectpaths(const projectname aname) {
    load(aname);
}

projectpaths::~projectpaths() {
}

string projectpaths::getrootdir() const {
    return rootdir;
}

string projectpaths::getvcsmodulename() const {
    return vcsmodule;
}

string projectpaths::getvcsrevision() const {
    return vcsrevision;
}

string projectpaths::getvcsroot() const {
    return vcsroot;
}

void projectpaths::setvcsmodule(const string &avcsmodule, const string &avcsrevision) {
    pname.setmodified(vcsmodule != avcsmodule || vcsrevision != avcsrevision);

    vcsmodule = avcsmodule;
    vcsrevision = avcsrevision;
}

void projectpaths::setvcsroot(const string &avcsroot) {
    pname.setmodified(vcsroot != avcsroot);
    vcsroot = avcsroot;
}

void projectpaths::setrootdir(const string &arootdir) {
    char buf[512];

    pname.setmodified(rootdir != arootdir);
    rootdir = arootdir;

    if(!chdir(rootdir.c_str()))
	realrootdir = getcwd(buf, 512);
}

bool projectpaths::load(const projectname aname) {
    ifstream f;
    string sect, buf;
    char cbuf[512];
    bool ret;

//    projectname::absorb(aname);
    pname = aname;
    f.open(pname.getregfname().c_str());

    if(ret = f.is_open()) {
	while(getconf(sect, buf, f) &&
	(rootdir.empty() || vcsmodule.empty() || vcsroot.empty())) {
    	    if(sect == "vcsmodule") vcsmodule = buf; else
    	    if(sect == "vcsroot") vcsroot = buf;
	}

	f.close();

	if(readlink(pname.getregfname().c_str(), cbuf, 512) > 0)
	    setrootdir(justpathname(cbuf));
    }

    return ret;
}

string projectpaths::transformfname(fnamemode mode, const string &fname) const {
    string r = fname;

    switch(mode) {
	case relative:
	    if(fname.substr(0, rootdir.size()) == rootdir) {
		r = fname.substr(rootdir.size()+1);
	    } else if(fname.substr(0, realrootdir.size()) == realrootdir) {
		r = fname.substr(realrootdir.size()+1);
	    }
	    break;
	case absolute:
	    if(fname.substr(0, 1) != "/") {
		r = rootdir;
		if(r.substr(r.size()-1, 1) != "/") r += '/';
		r += fname;
	    }
	    break;
    }

    return r;
}

void projectpaths::populateparselist(pparamslist *parselist) {
    pparamslist_add("rootdir", getrootdir().c_str(), parselist);
    pparamslist_add("vcsroot", vcsroot.c_str(), parselist);
    pparamslist_add("vcsmodule", vcsmodule.c_str(), parselist);
    pparamslist_add("vcsrevision", vcsrevision.c_str(), parselist);
}
