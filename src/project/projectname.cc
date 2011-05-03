#include "projectname.h"
#include "motormisc.h"
#include "motorconfiguration.h"
#include "kkfsys.h"

#include <sys/stat.h>

projectname::projectname() {
    loaded = fmodified = false;
}

projectname::projectname(const string &aname/*, const string arevision*/) {
    loaded = fmodified = false;
    load(aname);
}

projectname::~projectname() {
}

string projectname::getregfname() const {
    string r = conf.getprojectdir() + name;
//    if(!revision.empty()) r += (string) "-" + revision;
    return r;
}

string projectname::getname() const {
    return name;
}

string projectname::gettemplatename() const {
    return templname;
}

string projectname::getversion() const {
    return version;
}
/*
const string projectname::getrevision() const {
    return revision;
}
*/
void projectname::setversion(const string &aversion) {
    setmodified(version != aversion);
    version = aversion;
}

void projectname::setname(const string &aname) {
    name = aname;
}

void projectname::settemplate(const string &atemplate) {
    templname = atemplate;
}
/*
void projectname::setrevision(const string arevision) {
    revision = arevision;
}
*/

bool projectname::convert4() {
    bool r = false;
    ifstream f(getregfname().c_str());
    string rootdir, sect, buf;
    struct stat st;

    if(r = f.is_open()) {
	while(getconf(sect, buf, f) && rootdir.empty())
    	    if(sect == "rootdir") rootdir = buf;

	f.close();

	if(r = (!rootdir.empty() && !stat(rootdir.c_str(), &st))) {
	    string projfname = rootdir + "/" + name + ".motor";
	    fmove(getregfname().c_str(), projfname.c_str());
	    symlink(projfname.c_str(), getregfname().c_str());
	}
    }

    return r;
}

bool projectname::load(const string &aname/*, const string arevision*/) {
    ifstream f;
    string sect, buf;
    struct stat st;
    bool ret = true;

    name = aname;
//    revision = arevision;

    if(lstat(getregfname().c_str(), &st))
        return false;

    if(S_ISDIR(st.st_mode))
	return false;

    if(!S_ISLNK(st.st_mode)) {
	if(!convert4())
	    return false;
    }

    if(!loaded || (aname != name)) {
	f.open(getregfname().c_str());

	if(loaded = ret = f.is_open()) {
	    fmodified = false;

	    while(getconf(sect, buf, f) &&
	    (version.empty() || templname.empty())) {
    		if(sect == "version") version = buf; else
    		if(sect == "template") templname = buf;
	    }

	    f.close();
	    loaded = !version.empty() && !templname.empty();
	}
    }

    return ret;
}

bool projectname::empty() const {
    return !loaded;
}

bool projectname::modified() {
    return fmodified;
}

void projectname::setmodified(bool exp) {
    fmodified = fmodified || exp;
}

void projectname::save() {
    fmodified = false;
    loaded = true;
}

void projectname::populateparselist(pparamslist *parselist) const {
    pparamslist_add("name", getname().c_str(), parselist);
    pparamslist_add("version", getversion().c_str(), parselist);
}

void projectname::clear() {
    name = templname = "";
    loaded = false;
}

bool projectname::operator == (const projectname &aname) const {
    return aname.getname() == getname();
}

bool projectname::operator != (const projectname &aname) const {
    return !(*this == aname);
}

bool projectname::operator < (const projectname &aname) const {
    if(templname != aname.templname) {
	return templname < aname.templname;
    } else {
	return name < aname.name;
    }
}

void projectname::absorb(const projectname &aname) {
    name = aname.name;
    version = aname.version;
    templname = aname.templname;
    loaded = aname.loaded;
    fmodified = aname.fmodified;
}
