#include "motorfolder.h"
#include "motorvcs.h"
#include "motorproject.h"

#include "kkfsys.h"

motorfolder::motorfolder(const string &atagname, const string &aname,
motorfile::filekind acontent, const string &araccept, const string &arreject,
const vector<projecttake> atakes) {
    tagname = atagname;
    name = aname;
    content = acontent;
    raccept = araccept;
    rreject = arreject;
    takes = atakes;
}

motorfolder::~motorfolder() {
}

string motorfolder::getname() const {
    return name;
}

string motorfolder::gettagname() const {
    return tagname;
}

motorfile::filekind motorfolder::getcontentkind() const {
    return content;
}

vector<motorfile>::iterator motorfolder::begin() {
    return files.begin();
}

vector<motorfile>::iterator motorfolder::end() {
    return files.end();
}

bool motorfolder::addfile(const motorfile afile, INT options) {
    regex_t ra, rr;
    struct stat st;
    bool ret = true;
    string dname;

    if(find(begin(), end(), afile.getfname()) != end())
	return true;

    if(content != motorfile::project) {
	if(!stat(afile.getfname().c_str(), &st))
	    if(((content == motorfile::source) && !S_ISREG(st.st_mode))
	    || ((content == motorfile::directory) && !S_ISDIR(st.st_mode)))
		return false;
    }

    if(!rreject.empty())
    if(!regcomp(&rr, rreject.c_str(), REG_EXTENDED)) {
	ret = regexec(&rr, afile.getfname().c_str(), 0, 0, 0);
	regfree(&rr);
    }

    if(!raccept.empty())
    if(!regcomp(&ra, raccept.c_str(), REG_EXTENDED)) {
	ret = ret && !regexec(&ra, afile.getfname().c_str(), 0, 0, 0);
	regfree(&ra);
    }

    if(ret && (options & AF_CHECKDIR)) {
	dname = "";

	switch(content) {
	    case motorfile::source:
		dname = justpathname(afile.getfname());
		break;
	    case motorfile::directory:
		dname = afile.getfname();
		break;
	}

	if(!dname.empty()) {
	    project.chdir();

    	    if(stat(dname.c_str(), &st)) {
		switch(ui.askf("YNC", _("Directory %s does not exist. Create it?"), dname.c_str())) {
		    case motorui::yes:
			if(!(ret = mksubdirs(dname)))
			    ui.log(_("Unable to create the directory"));
                	break;
            	    case motorui::cancel:
			ret = false;
			break;
		}
            } else if(!S_ISDIR(st.st_mode)) {
		ui.logf(_("%s is ~not~ a directory"), dname.c_str());
		ret = false;
	    }
	}
    }

    if(ret) {
	switch(getcontentkind()) {
	    case motorfile::source:
		if(options & AF_VCS)
		if(vcs.enabled()) {
		    vcs.addfile(afile.getfname());
		    ret = vcs.good();
		}
		break;

	    case motorfile::project:
		if(options & AF_TAKE)
		    take(afile.getfname());
		break;
	}

	if(ret) {
	    files.push_back(afile);
	}
    }

    return ret;
}

bool motorfolder::removefile(const motorfile afile) {
    vector<motorfile>::iterator i;

    i = find(files.begin(), files.end(), afile.getfname());

    if(i != files.end()) {
        switch(getcontentkind()) {
	    case motorfile::project:
		untake(i->getfname());
		break;
	    case motorfile::source:
		if(vcs.enabled()) vcs.removefile(afile.getfname());
		break;
	}

        files.erase(i);
    }
}

vector<string> motorfolder::gettempltotakefrom() {
    vector<projecttake>::iterator i;
    vector<string> r;

    for(i = takes.begin(); i != takes.end(); i++)
	r.push_back(i->getsourcetemplate());

    return r;
}

void motorfolder::take(const string &pname) {
    vector<projecttake>::iterator i;

    for(i = takes.begin(); i != takes.end(); i++)
	i->exec(pname);
}

void motorfolder::untake(const string &pname) {
    vector<projecttake>::iterator i;

    for(i = takes.begin(); i != takes.end(); i++)
	i->undo(pname);
}

bool motorfolder::operator == (const string &atagname) const {
    return tagname == atagname;
}

bool motorfolder::operator != (const string &atagname) const {
    return !(*this == atagname);
}

bool motorfolder::operator == (const motorfile::filekind akind) const {
    return getcontentkind() == akind;
}

bool motorfolder::operator != (const motorfile::filekind akind) const {
    return !(*this == akind);
}
