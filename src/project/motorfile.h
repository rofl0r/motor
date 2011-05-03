#ifndef __MOTORFILE_H__
#define __MOTORFILE_H__

#include "motorcommon.h"

__MOTOR_BEGIN_NAMESPACE

class motorfile {
    public:
	enum filekind {
	    source,
	    directory,
	    symbol,
	    project
	};

	struct build {
	    build(): def(false) {}
	    string param, help;
	    bool def;
	};

    protected:
	string fname;
	build fbuild;

    public:
	motorfile(const string &afname);
	~motorfile();

	string getfname() const;

	build getbuild() const { return fbuild; }
	void setbuild(const build &b) { fbuild = b; }

	void remove();
	    // removes the file from disk

	bool operator == (const string &afname) const;
	bool operator != (const string &afname) const;

	bool operator == (const motorfile &afile) const;
	bool operator != (const motorfile &afile) const;

	bool operator == (regex_t *r) const;
	bool operator != (regex_t *r) const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::motorfile;

#endif

#endif
