#ifndef __PROJECTPATHS_H__
#define __PROJECTPATHS_H__

#include "motorcommon.h"
#include "projectname.h"

__MOTOR_BEGIN_NAMESPACE

class projectpaths {
    private:
	projectname pname;

    public:
	enum fnamemode {
	    relative, absolute
	};

    protected:
	string rootdir, vcsmodule, vcsroot, vcsrevision;
	string realrootdir;

    public:
	projectpaths();
	projectpaths(const projectname aname);
	~projectpaths();

	string getrootdir() const;
	string getvcsmodulename() const;
	string getvcsrevision() const;
	string getvcsroot() const;

	void setvcsmodule(const string &avcsmodule, const string &avcsrevision = "");
	void setvcsroot(const string &avcsroot);
	void setrootdir(const string &arootdir);
	string getprojfname() const;

	string transformfname(fnamemode mode, const string &fname) const;

	bool load(const projectname aname);

	void populateparselist(pparamslist *parselist);
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::projectpaths;

#endif

#endif
