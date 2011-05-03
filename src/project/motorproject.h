#ifndef __MOTORPROJECT_H__
#define __MOTORPROJECT_H__

#include "motorcommon.h"
#include "projectname.h"
#include "projectfiles.h"
#include "projectpaths.h"
#include "projecttempl.h"
#include "projectdesk.h"
#include "projectdebug.h"
#include "motortagbrowser.h"

#define	CR_GNU_DOC		2
#define	CR_GENERATE_SOURCE	4

#define	LP_NOCHECK		2

__MOTOR_BEGIN_NAMESPACE

class motorproject:
    public projectname,
    public projectfiles,
    public projectdebug,
    public projectdesk,
    public projectpaths,
    public projecttempl
{
    public:
	enum makemodekind {
	    automake,
	    manual
	};

    protected:
	string cflags, lflags, destdir, vcsname;
	makemodekind makemode;
	bool gettextized, fdmake, fdcomp, fdvcs;
	int options;

	void writemakefunc();
	void arrangebuildstuff();
	void getdisp();
	void addgnudoc();
	bool onprojectstart();
	void checkautomake();

	static char *svmakefile(const char *value, const char *key);

    public:
	motorproject();
	motorproject(const projectname aname, int aoptions = 0);

	~motorproject();

	void populateparselist(pparamslist *parselist);

	bool load(const projectname aname);
	void save();
	bool close();

	void chdir();

	bool create(int options = 0);
	bool build();
	void clean();
	void dist(const string &targetname, const string &destdir);
	bool runtags();
	void regenerate();
	bool remove();
	void runtarget(const string &targetname);
	bool execvcs(const string &action);

	void getflags(string &acflags, string &alflags) const;
	void setflags(const string &acflags, const string &alflags);

	bool isgettextized() const;
	void setgettextized(bool a);

	void setvcs(const string &avcsname);
	string getvcs() const;

	makemodekind getmakemode() const;
	void setmakemode(makemodekind amakemode);

	void settemplate(const string &atemplate);
	void generate();
	void import(const string &mask, const string &root, int options = 0);

	bool addfile(const motorfile afile, int options = 0);
	void removefile(const motorfile afile, motorfile::filekind akind = motorfile::source);

	string getprojfname() const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::motorproject;

#endif

extern motorproject project;

#endif
