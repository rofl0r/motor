#ifndef __BREAKPOINT_H__
#define __BREAKPOINT_H__

#include "motorcommon.h"

__MOTOR_BEGIN_NAMESPACE

class breakpoint {
    protected:
	string fname, id;
	INT line;
	bool fpermanent;

    public:
	breakpoint();
	breakpoint(const string &aid, const string &afname, INT aline, bool apermanent);
	breakpoint(const string &afname, INT aline);
	breakpoint(const string &definition);
	~breakpoint();

	string getfname() const;
	INT getline() const;
	bool permanent() const;

	bool operator == (const string &aid) const;
	bool operator != (const string &aid) const;

	bool operator == (const pair<string, INT> aloc) const;
	bool operator != (const pair<string, INT> aloc) const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::breakpoint;

#endif

#endif
