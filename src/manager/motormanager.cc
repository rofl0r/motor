#include "motormanager.h"
#include "motorconfiguration.h"

#include "kkfsys.h"

#include <dirent.h>

motormanager::motormanager() {
}

motormanager::~motormanager() {
}

vector<projectname> motormanager::getprojectlist() {
    struct dirent *ent;
    DIR *d;
    vector<projectname> result;
    projectname pname;
    
    if(d = opendir(conf.getprojectdir().c_str())) {
	while(ent = readdir(d)) {
	    pname = projectname();
	    if(pname.load(ent->d_name)) {
		result.push_back(pname);
	    }
	}

	closedir(d);
    }

    sort(result.begin(), result.end());
    return result;
}

int motormanager::addtemplate(const char *file, const struct stat *sb, int flag) {
    int pos;
    string tname = file, buf;

    if(flag == FTW_F)
    if(pos = tname.find("/templates/")) {
        tname.erase(0, pos+11);
	getrword(tname, "/");
	tname = trailcut(leadcut(tname, "/"), "/");
        buf = getrword(buf = tname, "/");

        if(buf != "CVS") {
            buf = justfname(file);
            if(buf.substr(0, 8) != "Makefile")
	    if(find(manager.container.begin(), manager.container.end(), tname)
	    == manager.container.end())
		manager.container.push_back(tname);
        }
    }

    return 0;
}

vector<string> motormanager::gettemplatelist() {
    container.clear();
    ftw(conf.gettemplatedir().c_str(), &addtemplate, 5);
    sort(container.begin(), container.end());
    return container;
}

vector<string> motormanager::getvcslist() {
    DIR *d;
    struct dirent *e;
    vector<string> r;
    string dname, fname;
    int i, dp;

    for(i = 0; i < 2; i++) {
	if(i) {
	    dname = SHARE_DIR;
	} else {
	    dname = conf.getmotordir();
	}

	if(d = opendir(dname.c_str())) {
	    while(e = readdir(d)) {
		fname = e->d_name;

		if((dp = fname.find(".")) != -1)
		    if(fname.substr(dp+1) == "vcsrc") {
			fname.erase(dp);
			if(find(r.begin(), r.end(), fname) == r.end())
			    r.push_back(fname);
		    }
	    }

	    closedir(d);
	}
    }

    return r;
}
