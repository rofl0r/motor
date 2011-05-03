#include "projecttempl.h"
#include "motorconfiguration.h"
#include "motormisc.h"

#include <dirent.h>

projecttempl::projecttempl() {
}

projecttempl::projecttempl(const projectname aname) {
    load(aname);
}

projecttempl::~projecttempl() {
}

vector<parserule>::const_iterator projecttempl::parsebegin() const {
    return parserules.begin();
}

vector<parserule>::const_iterator projecttempl::parseend() const {
    return parserules.end();
}

vector<sourcetemplate>::const_iterator projecttempl::templbegin() const {
    return sourcetemplates.begin();
}

vector<sourcetemplate>::const_iterator projecttempl::templend() const {
    return sourcetemplates.end();
}

vector<motordist>::const_iterator projecttempl::distbegin() const {
    return dists.begin();
}

vector<motordist>::const_iterator projecttempl::distend() const {
    return dists.end();
}

vector< pair<char, string> >::const_iterator projecttempl::tagnamebegin() const {
    return tagnames.begin();
}

vector< pair<char, string> >::const_iterator projecttempl::tagnameend() const {
    return tagnames.end();
}
/*
void projecttempl::clear() {
    parserules.clear();
    sourcetemplates.clear();
    dists.clear();
    tagnames.clear();
}*/

bool projecttempl::load(const projectname aname) {
    ifstream f;
    string sect, buf, let, spref, param;
    DIR *d;
    struct dirent *de;
    bool ret;

    vector<motordist>::iterator id;
    vector<motordist::distparam>::iterator idp;

//    projectname::absorb(aname);
    pname = aname;

    f.open((conf.gettemplatedir(pname.gettemplatename()) + "/Parserules").c_str());
    if(ret = f.is_open()) {
	while(getconf(sect, buf, f)) {
	    parserules.push_back(parserule(sect, buf));
	}

	f.close();
	f.clear();
    }

    f.open((conf.gettemplatedir(pname.gettemplatename()) + "/Settings").c_str());

    if(ret = f.is_open()) {
	while(getconf(sect, buf, f)) {
	    if(sect.substr(0, 4) == "dist") {
                getword(sect);
		param = getword(buf);

                if(param == "target") {
		    dists.push_back(motordist(sect, buf));

		} else if(param == "parameter") {
		    if((id = find(dists.begin(), dists.end(), sect)) != dists.end()) {
			param = getword(buf);
			id->addparameter(param, buf);
		    }

		} else if(param == "default") {
		    if((id = find(dists.begin(), dists.end(), sect)) != dists.end()) {
			param = getword(buf);
			if((idp = find(id->parambegin(), id->paramend(), param)) != id->paramend())
			    idp->defval = buf;
		    }

		}

	    } else if(sect == "tagnames") {
		if((let = getword(buf)).size() == 1) {
		    tagnames.push_back(pair<char, string>(let[0], buf));
		}
	    }
	}

	f.close();
	f.clear();
    }

    if(d = opendir(conf.gettemplatedir(pname.gettemplatename()).c_str())) {
	while(de = readdir(d)) {
	    buf = de->d_name;
	    if(getrword(buf, ".") == "tmpl") {
		sourcetemplates.push_back(sourcetemplate(buf));
	    }
	}
	closedir(d);
    }

    return ret;
}

void projecttempl::populateparselist(pparamslist *parselist) {
}

string projecttempl::getbrowsertagname(char kind) const {
    string r;
    vector< pair<char, string> >::const_iterator it;

    for(it = tagnames.begin(); it != tagnames.end() && r.empty(); it++)
	if(it->first == kind)
	    r = it->second;

    if(r.empty()) r += kind;
    return r;
}
