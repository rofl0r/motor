#include "motorgrepper.h"
#include "motorfolder.h"
#include "motorproject.h"
#include "motorui.h"

#include "kkfsys.h"

motorgrepper::motorgrepper() {
}

motorgrepper::~motorgrepper() {
}

void motorgrepper::setadditional(const vector<string> a) {
    fadd = a;
}

void motorgrepper::makefilelist() {
    vector<motorui::editfile> efs;
    vector<string>::iterator i;
    vector<motorfolder>::iterator ifold;
    vector<motorfile>::iterator ifile;
    projectfiles pf;
    projectpaths pp;

    flst.clear();

    if(mode == currentfile) {

	efs = ui.geteditfiles();
	if(!efs.empty())
	    flst.push_back((efs.end()-1)->fname);

    } else if(mode == folders) {

	for(i = fadd.begin(); i != fadd.end(); i++)
	    if((ifold = find(project.foldbegin(), project.foldend(), *i)) != project.foldend())
		if(ifold->getcontentkind() == motorfile::source)
		    for(ifile = ifold->begin(); ifile != ifold->end(); ifile++)
			flst.push_back(ifile->getfname());

    } else if(mode == allfiles) {

	for(ifold = project.foldbegin(); ifold != project.foldend(); ifold++)
	    if(ifold->getcontentkind() == motorfile::source)
		for(ifile = ifold->begin(); ifile != ifold->end(); ifile++)
		    flst.push_back(ifile->getfname());

    } else if(mode == connected) {

	for(i = fadd.begin(); i != fadd.end(); i++)
	    if(pp.load(*i) && pf.load(*i)) {
		for(ifold = pf.foldbegin(); ifold != pf.foldend(); ifold++) {
		    if(ifold->getcontentkind() == motorfile::source) {
			for(ifile = ifold->begin(); ifile != ifold->end(); ifile++) {
			    flst.push_back(pp.transformfname(projectpaths::absolute,
				ifile->getfname()));
			}
		    }
		}
	    }

    }

}

void motorgrepper::exec(grepmode amode, const string &apattern, int aoptions) {
    mode = amode;
    pattern = apattern;
    options = aoptions;

    makefilelist();
    go();
}

void motorgrepper::clear() {
    results.clear();
}

void motorgrepper::go() {
    vector<string>::const_iterator i;

    openfiles = ui.geteditfiles();

    if(options & MG_REGEXP)
	if(regcomp(&re, pattern.c_str(), REG_EXTENDED
	|| ((options & MG_CASESENS) ? 0 : REG_ICASE))) {
	    ui.log(_("The regexp specified cannot be compiled"));
	    return;
	}

    for(i = flst.begin(); i != flst.end(); i++) grepfile(*i);
    ui.logf(_("%d files grepped, %d matches found"), flst.size(), results.size());

    if(options & MG_REGEXP)
	regfree(&re);
}

void motorgrepper::grepfile(const string &fname) {
    vector<string> content;
    vector<string>::iterator ic;
    vector<motorui::editfile> fedit;
    vector<motorui::editfile>::iterator ie;
    string buf;
    int col, frompos, subsize;

    project.chdir();
    ui.logf(_("Grepping %s.."), fname.c_str());

    for(ie = openfiles.begin(); ie != openfiles.end() && content.empty(); ie++) {
        if(samefile(ie->fname, fname)) content = ui.geteditfile(ie->fname);
    }

    if(content.empty()) {
	int fsize;
	char *pbuf;
	ifstream f(fname.c_str());

	if(f.is_open()) {
	    f.seekg(0, ios::end);
	    fsize = f.tellg();
	    f.seekg(0, ios::beg);
	    pbuf = new char[fsize+1];
	    f.read(pbuf, fsize);
	    pbuf[fsize] = 0;
	    breakintolines(pbuf, content);
	    f.close();
            delete pbuf;
	}
    }

    switch(mode) {
	case currentfile:
	    fedit = ui.geteditfiles();
	    if(!fedit.empty()) {
		ic = content.begin() + (fedit.end()-1)->y;
	    }
	    break;
	default:
	    ic = content.begin();
	    break;
    }

    for(; ic != content.end(); ic++) {
        frompos = col = 0;

	while((col = grepline(*ic, subsize, frompos)) != -1) {
	    results.push_back(motorui::editfile(fname, col, (ic-content.begin())+1));
	    frompos = col+subsize;
	}
    }
}

int motorgrepper::grepline(const string &s, int &subsize, int frompos) {
    string cpat, cs, origs, buf;
    string::iterator c;
    regmatch_t rm[1];
    int r;

    r = -1;
    subsize = 0;
    origs = s;
    buf = s.substr(frompos);

    if(options & MG_REGEXP) {
	if(!regexec(&re, buf.c_str(), 1, rm, 0)) {
            r = rm[0].rm_so;
	    subsize = rm[0].rm_eo-rm[0].rm_so;
        }
    } else {
	cs = buf, cpat = pattern;

	if(!(options & MG_CASESENS)) {
	    for(cs = buf, c = cs.begin();
                c != cs.end();
                *c = toupper(*c), c++);

	    for(cpat = pattern, c = cpat.begin();
                c != cpat.end();
                *c = toupper(*c), c++);
	}

	if((r = rm[0].rm_so = cs.find(cpat)) != -1) {
	    rm[0].rm_eo = rm[0].rm_so + (subsize = cpat.size());
	}
    }

    if(r != -1) {
	if(options & MG_WHOLEWORD)
            if(!iswholeword(origs, rm[0].rm_so+frompos, rm[0].rm_eo+frompos))
                r = -1;
	r += frompos;
    }

    return r;
}

vector<motorui::editfile>::const_iterator motorgrepper::begin() const {
    return results.begin();
}

vector<motorui::editfile>::const_iterator motorgrepper::end() const {
    return results.end();
}
