#include "projecttake.h"
#include "motormisc.h"
#include "motorproject.h"

projecttake::projecttake() {
}

projecttake::projecttake(const string &definition) {
    regex_t r;
    string buf, token;
    regmatch_t rm[4];
    bool rdone;

    rdone = false;
    token = getword(buf = definition);
    if(token != "take") buf = definition;

    if(!regcomp(&r, "^\"(.+)\" to (.+) from (.+)$", REG_EXTENDED)) {
	if(rdone = !regexec(&r, buf.c_str(), 4, rm, 0)) kind = text;
	regfree(&r);
    }

    if(!rdone && !regcomp(&r, "^(.+) to (.+) from (.+)$", REG_EXTENDED)) {
	if(rdone = !regexec(&r, buf.c_str(), 4, rm, 0)) kind = folder;
	regfree(&r);
    }

    if(!rdone || (rm[3].rm_so == -1)) throw (0);

    placefrom = buf.substr(rm[1].rm_so, rm[1].rm_eo-rm[1].rm_so);
    placeto = buf.substr(rm[2].rm_so, rm[2].rm_eo-rm[2].rm_so);
    sourcetemplate = buf.substr(rm[3].rm_so, rm[3].rm_eo-rm[3].rm_so);
}

projecttake::~projecttake() {
}

void projecttake::exec(const string &pname) {
    vector<motorfolder>::iterator ifold, ffold;
    vector<motorfile>::iterator ifile;
    motorproject mp;
    string fname;

    if(!(mp = motorproject(pname)).empty()) {
	if(kind == text) {

	    if((ifold = find(project.foldbegin(), project.foldend(), placeto))
	    != project.foldend())
		ifold->addfile(motorfile(getparsed(mp, placefrom)));

	} else if(kind == folder) {

	    if((ifold = find(project.foldbegin(), project.foldend(), placeto)) != project.foldend())
	    if((ffold = find(mp.foldbegin(), mp.foldend(), placefrom)) != mp.foldend()) {
		for(ifile = ffold->begin(); ifile != ffold->end(); ifile++) {
		    fname = mp.transformfname(projectpaths::absolute, ifile->getfname());
		    ifold->addfile(fname);
		}
	    }

	}
    }

    project.chdir();
}

void projecttake::undo(const string &pname) {
    vector<motorfolder>::iterator ifold, ffold;
    vector<motorfile>::iterator ifile, ef;
    motorproject mp;
    string fname;

    if(!(mp = motorproject(pname)).empty()) {
	if(kind == text) {

	    if((ifold = find(project.foldbegin(), project.foldend(), placeto)) != project.foldend())
            if((ifile = find(ifold->begin(), ifold->end(), getparsed(mp, placefrom))) != ifold->end())
                ifold->removefile(*ifile);

	} else if(kind == folder) {

	    if((ifold = find(project.foldbegin(), project.foldend(), placeto)) != project.foldend())
	    if((ffold = find(mp.foldbegin(), mp.foldend(), placefrom)) != mp.foldend()) {
		for(ifile = ffold->begin(); ifile != ffold->end(); ifile++)
                    if((ef = find(ifold->begin(), ifold->end(), ifile->getfname())) != ifold->end()) {
			fname = mp.transformfname(projectpaths::absolute, ef->getfname());
                        ifold->removefile(fname);
		    }
	    }

	}
    }

    project.chdir();
}

string projecttake::getsourcetemplate() {
    return sourcetemplate;
}
