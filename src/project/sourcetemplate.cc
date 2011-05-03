#include "sourcetemplate.h"
#include "motorui.h"
#include "motorproject.h"
#include "motorconfiguration.h"

#include "parser.h"

sourcetemplate::sourcetemplate(const string &afname) {
    fname = afname;
}

sourcetemplate::~sourcetemplate() {
}

string sourcetemplate::getfname() const {
    pparamslist *p = pparamslist_create();
    string r;
    char *pbuf;

    project.populateparselist(p);
    r = pbuf = sstrparse(fname.c_str(), p);
    pparamslist_free(p);

    delete pbuf;
    return r;
}

string sourcetemplate::getrawfname() const {
    return fname;
}

bool sourcetemplate::operator == (const string &afname) const {
    return getfname() == afname;
}

bool sourcetemplate::operator != (const string &afname) const {
    return !(*this == afname);
}

bool sourcetemplate::generate() const {
    bool ret = true;
    motorui::askresult ar;
    pparamslist *p;
    FILE *f;
    string rfname, tname;
    vector<motorfolder>::iterator ifold;

    rfname = project.getrootdir() + "/" + getfname();
    tname = conf.gettemplatedir(project.gettemplatename()) + "/" + getrawfname() + ".tmpl";

    if(ret && !access(rfname.c_str(), F_OK))
    switch(ui.askf("YNC", _("File %s already exits. Overwrite it?"), getfname().c_str())) {
	case motorui::no:
	    return true;
	case motorui::cancel:
	    ret = false;
    }

    if(ret)
    if(ret = (f = fopen(rfname.c_str(), "w"))) {
	p = pparamslist_create();
	project.populateparselist(p);
	strparse(tname.c_str(), tname.size(), p, f, PARSER_SRC_FILE, PARSER_FLOAD_MMAP);
	pparamslist_free(p);
	fclose(f);

	rfname = project.transformfname(projectpaths::relative, rfname);
	for(ifold = project.foldbegin(); ifold != project.foldend(); ifold++)
            if(ifold->addfile(motorfile(rfname), AF_VCS)) break;
    }

    return ret;
}
