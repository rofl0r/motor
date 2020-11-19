#ifndef __MOTORFOLDER_H__
#define __MOTORFOLDER_H__

#include "motorcommon.h"
#include "motorfile.h"
#include "projecttake.h"

#define	AF_VCS		2
    // add a file to VCS
#define	AF_CHECKDIR	4
    // check path to a file
#define AF_TAKE		8
    // perform a "take" action when
    // adding a connected project

__MOTOR_BEGIN_NAMESPACE

class motorfolder {
    protected:
	vector<motorfile> files;
	vector<projecttake> takes;
	motorfile::filekind content;
	string tagname, name, raccept, rreject;

	void take(const string &pname);
	void untake(const string &pname);

    public:
	motorfolder(const string &atagname, const string &aname,
	    motorfile::filekind acontent, const string &araccept,
	    const string &arreject, const vector<projecttake> atakes);

	~motorfolder();

	string getname() const;
	string gettagname() const;
	motorfile::filekind getcontentkind() const;
	    // general folder settings

	vector<motorfile>::iterator begin();
	vector<motorfile>::iterator end();
	    // begin and end interators of the file list

	bool addfile(const motorfile afile, int options = 0);
	    // returns true if the file was accepted

	bool removefile(const motorfile afile);
	    // returns true if the file existed

	vector<string> gettempltotakefrom();

	bool operator == (const string &atagname) const;
	bool operator != (const string &atagname) const;

	bool operator == (const motorfile::filekind akind) const;
	bool operator != (const motorfile::filekind akind) const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::motorfolder;

#endif

#endif
