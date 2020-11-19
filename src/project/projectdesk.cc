#include "projectdesk.h"
#include "motormisc.h"
#include "motorconfiguration.h"

projectdesk::projectdesk() {
}

projectdesk::projectdesk(const projectname aname) {
    load(aname);
}

projectdesk::~projectdesk() {
}

string projectdesk::getarguments() const {
    return arguments;
}

void projectdesk::setarguments(const string &aarguments) {
    arguments = aarguments;
}

void projectdesk::loadeditfile(const string &afname, int x, int y) {
    fedit.push_back(motorui::editfile(afname, x, y));
}

bool projectdesk::load(const projectname aname) {
    int ex, ey;
    string sect, buf, tmbuf;
    ifstream f;

    fedit.clear();
    settings.clear();

//    projectname::absorb(aname);
    pname = aname;
    f.open(pname.getregfname().c_str());

    if(f.is_open()) {
	while(getconf(sect, buf, f)) {
	    if(sect == "arguments") {
		arguments = buf;

	    } else if(sect == "editor") {
		tmbuf = getrword(buf); ey = atol(tmbuf.c_str());
		tmbuf = getrword(buf); ex = atol(tmbuf.c_str());
		loadeditfile(buf, ex, ey);

	    } else if(sect == "desktop") {
		tmbuf = getword(buf);
		settings.push_back(pair<string, string>(tmbuf, buf));

	    }
	}

	f.close();
    }
}

void projectdesk::editload() {
    vector<motorui::editfile>::iterator ie;

    for(ie = fedit.begin(); ie != fedit.end(); ie++) {
	ui.loadeditfile(*ie);
    }

    ui.setdesktop(settings);
}

void projectdesk::populateparselist(pparamslist *parselist) {
    vector<motorui::editfile> edfiles;

    edfiles = ui.geteditfiles();
    pparamslist_add("args", arguments.c_str(), parselist);

    if(!edfiles.empty()) {
	pparamslist_add("cureditfile", (edfiles.end()-1)->fname.c_str(), parselist);
    }
}
