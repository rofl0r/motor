#include "projectfiles.h"
#include "motorconfiguration.h"
#include "motormisc.h"
#include "projecttake.h"

projectfiles::projectfiles() {
}

projectfiles::projectfiles(const projectname aname) {
    load(aname);
}

projectfiles::~projectfiles() {
}

vector<motorfolder>::iterator projectfiles::foldbegin() {
    return folders.begin();
}

vector<motorfolder>::iterator projectfiles::foldend() {
    return folders.end();
}

void projectfiles::loadfolders() {
    ifstream f;
    string sect, buf, param;
    projecttake take;
    vector<motorfolder>::iterator ifold = folders.end();
    vector<projecttake> takes;

    string prevsect, tagname, name, raccept, rreject;
    motorfile::filekind content;

    f.open((conf.gettemplatedir(pname.gettemplatename()) + "/Folders").c_str());

    if(f.is_open()) {
        while(getconf(sect, buf, f)) {
            if((sect != prevsect) || !prevsect.size()) {
		if(!tagname.empty())
		    folders.push_back(motorfolder(tagname, name,
		    content, raccept, rreject, takes));

		name = raccept = rreject = "";
                tagname = sect;
		takes.clear();
            }

            prevsect = sect;
            param = getword(buf);

            if(param == "element") {

                if(buf == "file") content = motorfile::source; else
                if(buf == "symbol") content = motorfile::symbol; else
                if(buf == "dir") content = motorfile::directory; else
                if(buf == "proj") content = motorfile::project;
                
            } else if(param == "take") {

		try {
		    take = projecttake(buf);
		    takes.push_back(take);
		} catch(...) {
		}

            } else if(param == "mask") {
		raccept = buf;
	    } else if(param == "reject") {
		rreject = buf;
	    } else if(param == "name") {
		name = buf;
	    }
        }

	if(!tagname.empty())
	    folders.push_back(motorfolder(tagname, name,
	    content, raccept, rreject, takes));
	
	f.close();
    }
}

bool projectfiles::load(const projectname aname) {
    bool ret = false;
    ifstream f;
    string sect, buf, prefix;
    vector<motorfolder>::iterator ifold;
    vector<motorfile>::iterator ifile;

    folders.clear();
//    projectname::absorb(aname);
    pname = aname;
    loadfolders();

    f.open(pname.getregfname().c_str());
    if(ret = f.is_open()) {
        while(getconf(sect, buf, f)) {
	    prefix = getword(sect);
    
	    if(prefix == "file") {
	        if((ifold = find(folders.begin(), folders.end(), getword(sect))) != folders.end()) {
		    if((ifile = find(ifold->begin(), ifold->end(), sect)) == ifold->end()) {
			ifold->addfile(motorfile(sect));
			ifile = ifold->end()-1;
		    }

		    prefix = getword(buf);
		    if(!buf.empty()) {
			motorfile::build b = ifile->getbuild();
			if(prefix == "param") b.param = buf; else
			if(prefix == "help") b.help = buf; else
			if(prefix == "default") b.def = buf == "1";
			ifile->setbuild(b);
		    }
		}
	    }
	}

	f.close();
    }

    return ret;
}

void projectfiles::removefile_r(const string &mask, motorfile::filekind kind) {
    regex_t r;
    vector<motorfolder>::iterator ifold;
    vector<motorfile>::iterator ifile;

    if(!regcomp(&r, mask.c_str(), REG_EXTENDED)) {
	for(ifold = foldbegin(); ifold != foldend(); ifold++)
	    if(ifold->getcontentkind() == kind)
                while((ifile = find(ifold->begin(), ifold->end(), &r)) != ifold->end())
                    ifold->removefile(*ifile);

	regfree(&r);
    }
}

vector<motorfile> projectfiles::getfiles_r(const string &mask,
motorfile::filekind kind) {
    regex_t r;
    vector<motorfile> lst;
    vector<motorfolder>::iterator ifold;
    vector<motorfile>::iterator ifile;

    if(!regcomp(&r, mask.c_str(), REG_EXTENDED)) {
	for(ifold = foldbegin(); ifold != foldend(); ifold++)
	if(ifold->getcontentkind() == kind) {
	    ifile = ifold->begin();
	    while((ifile = find(ifile, ifold->end(), &r)) != ifold->end()) {
		lst.push_back(*ifile);
		ifile++;
	    }
	}

	regfree(&r);
    }

    return lst;
}

vector<string> projectfiles::extractdirectories() {
    vector<string> r;
    vector<string>::iterator ir;
    vector<motorfolder>::iterator ifold;
    vector<motorfile>::const_iterator ifile;
    string pname;
    int pos;

    for(ifold = foldbegin(); ifold != foldend(); ifold++) {
	if(ifold->getcontentkind() == motorfile::source) {
	    for(ifile = ifold->begin(); ifile != ifold->end(); ifile++) {
		if(justfname(ifile->getfname()) != "Makefile.am") {
		    pname = justpathname(ifile->getfname());
		    while(!pname.empty()) {
			r.push_back(pname);
			getrword(pname, "/");
		    }
		}
	    }
	}
    }

    sort(r.begin(), r.end());
    ir = unique(r.begin(), r.end());
    for(pos = ir-r.begin(); r.size() != pos; r.erase(r.end()-1));

    return r;
}
