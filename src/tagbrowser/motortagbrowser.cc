#include "motortagbrowser.h"
#include "motorproject.h"
#include "motorexecutor.h"

#include <sstream>

motortagbrowser::motortagbrowser()
    : scope(Everything), readok(false)
{ }

motortagbrowser::~motortagbrowser() {
}

vector<motortagbrowser::record> motortagbrowser::fetch(const string &prefix, const string &filter) {
    vector<record> r;
    vector<record>::iterator ir;

    readtags();

    for(ir = tags.begin(); ir != tags.end(); ir++)
	if(prefix == ir->getname().substr(0, prefix.size()))
	    if(filter.find(ir->getkind()) != -1)
		r.push_back(*ir);

    return r;
}

void motortagbrowser::readtags() {
    stringstream st;
    string buf;
    record tr;

    readok = true;

    if(tags.empty()) {
	if(ui.autosave()) {
    	    ui.log(_("Executing Exuberant Ctags.."));

	    if(readok = project.runtags()) {
		st << executor.getlastbuf();

		while(!st.eof()) {
		    getstring(st, buf);

		    try {
			tr = record(buf);
			tags.push_back(tr);
		    } catch(...) {}
		}
	    } else {
		ui.log(_("Cannot read the tags table"));
	    }
	}
    }
}

void motortagbrowser::clear() {
    tags.clear();
}

bool motortagbrowser::ok() {
    return readok;
}

void motortagbrowser::setscope(viewscope ascope) {
    scope = ascope;
}

motortagbrowser::viewscope motortagbrowser::getscope() const {
    return scope;
}

// ----------------------------------------------------------------------------

motortagbrowser::record::record() {
}

motortagbrowser::record::record(const string &adef) {
    static vector<parserule> p;
    vector<parserule>::iterator i;

    if(p.empty()) {
	p.push_back(parserule("", "^([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([[:digit:]]+);\"[[:space:]]+([[:alpha:]]?)$	NFLK"));
	p.push_back(parserule("", "^([^[:space:]]+)[[:space:]]+([^[:space:]]+)[[:space:]]+([[:digit:]]+);\"[[:space:]]+([[:alpha:]]?)[[:space:]]+(.+)$	NFLKI"));
    }

    if((i = find(p.begin(), p.end(), pair<string, string>(adef, ""))) != p.end()) {
	name = i->getvalue("N");
	fname = i->getvalue("F");
	lineno = atol(i->getvalue("L").c_str());
	info = i->getvalue("I");
        kind = *i->getvalue("K").begin();
    } else {
	throw (0);
    }
}

motortagbrowser::record::~record() {
}

motorui::editfile motortagbrowser::record::geteditfile() const {
    return motorui::editfile(fname, 0, lineno);
}

string motortagbrowser::record::getname() const {
    return name;
}

string motortagbrowser::record::getinfo() const {
    return info;
}

char motortagbrowser::record::getkind() const {
    return kind;
}

string motortagbrowser::record::getbrowsertagname() const {
    return project.getbrowsertagname(kind);
}
