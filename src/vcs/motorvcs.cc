#include "motorvcs.h"
#include "motorexecutor.h"
#include "motorconfiguration.h"
#include "motormisc.h"
#include "motorui.h"

#include <sstream>

motorvcs::motorvcs() {
    fgood = true;
}

motorvcs::motorvcs(const string &avcsname) {
    fgood = true;
    load(avcsname);
}

motorvcs::~motorvcs() {
}

void motorvcs::load(const string &avcsname) {
    string sect, buf;
    ifstream f(conf.getconfigfname(avcsname + ".vcsrc").c_str());

    make = "";

    if(f.is_open()) {
	while(getconf(sect, buf, f)) {
	    if(sect == "make") {
		make += buf + "\n";
	    } else if(sect == "error") {
		parserrors.push_back(parserule("", buf, parserule::error));
	    }
	}

	f.close();
    }
}

void motorvcs::setcommitcomment(const string &amsg) {
    executor.setvar("MOTOR_VCSCOMMENT", amsg);
}

bool motorvcs::good() {
    return fgood;
}

void motorvcs::makeflist(const vector<string> &fnames) {
    string flist;
    vector<string>::const_iterator i;

    if(!fnames.empty()) {
        for(i = fnames.begin(); i != fnames.end(); i++) {
            if(!flist.empty()) flist += " ";
            flist += *i;
        }
        executor.setvar("MOTOR_VCSFILES", flist);
    }
}

void motorvcs::commit(const vector<string> &fnames) {
    makeflist(fnames);
    ui.log(_("Checking in.."));
    fgood = project.execvcs("commit");
}

void motorvcs::update(const vector<string> &fnames) {
    makeflist(fnames);
    ui.log(_("Checking out.."));
    fgood = project.execvcs("update");
}

void motorvcs::localunmodify(const vector<string> &fnames = vector<string>()) {
    makeflist(fnames);
    fgood = project.execvcs("unmodify");
}

void motorvcs::addfile(const string &afname) {
    ui.log(_("Adding a file to VCS.."));
    executor.setvar("MOTOR_FNAME", afname);
    fgood = project.execvcs("addfile");
}

void motorvcs::removefile(const string &afname) {
    ui.log(_("Removing a file from VCS.."));
    executor.setvar("MOTOR_FNAME", afname);
    fgood = project.execvcs("removefile");
}

void motorvcs::tag(const string &arelname) {
    ui.log(_("Tagging a release.."));
    executor.setvar("MOTOR_TAG", arelname);
    fgood = project.execvcs("tag");
}

void motorvcs::import() {
    ui.log(_("Importing into VCS.."));
    fgood = project.execvcs("import");
}

void motorvcs::checkout() {
    fgood = project.execvcs("checkout");
}

void motorvcs::putmake(FILE *f, pparamslist *plist) {
    fstrparse(make.c_str(), plist, f);
}

bool motorvcs::enabled() {
    return
	!project.getvcs().empty() &&
	!project.getvcsmodulename().empty() &&
	!project.getvcsroot().empty();
}

vector< pair<string, motorvcs::changekind> > motorvcs::getchanges() {
    vector< pair<string, changekind> > r;
    stringstream st;
    string buf, fname;
    changekind ck;

    if(ui.autosave()) {
	ui.log(_("Retrieving recent modifications.."));

	if(fgood = project.execvcs("getchanges")) {
	    st << executor.getlastbuf();

	    while(!st.eof()) {
    		getstring(st, buf);

	        if(buf.substr(0, 4) == "LA: ") ck = locally_added; else
		if(buf.substr(0, 4) == "LR: ") ck = locally_removed; else
	        if(buf.substr(0, 4) == "LM: ") ck = locally_modified; else
		if(buf.substr(0, 4) == "RM: ") ck = remotely_modified; else
		    continue;

		buf.erase(0, 4);

		while(!buf.empty()) {
		    fname = getword(buf);
	    	    r.push_back(pair<string, changekind>(fname, ck));
		}
	    }

	    if(r.empty()) {
		ui.log(_("VCS check complete. No changes found"));
	    }
	}
    }

    return r;
}

bool motorvcs::iserror(parserule *apr, const string &msg) {
    bool r;
    vector<parserule>::iterator i;

    if(r = (i = find(parserrors.begin(), parserrors.end(),
    pair<string, string>(msg, ""))) != parserrors.end()) {
        apr = &*i;
    }

    return r;
}

string motorvcs::getdiff(const string &afname) {
    ui.log(_("Retrieving the diff record.."));
    executor.setvar("MOTOR_FNAME", afname);
    fgood = project.execvcs("getdiff");
    return executor.getlastbuf();
}

void motorvcs::forkbranch(const string &tag, const string &destdir) {
    ui.logf(_("Creating branch %s.."), tag.c_str());
    executor.setvar("MOTOR_TAG", tag);
    executor.setvar("MOTOR_DIR", destdir);
    fgood = project.execvcs("branch");
}
