#ifndef __BREAKPOINT_H__
#define __BREAKPOINT_H__

#include "motorcommon.h"

__MOTOR_BEGIN_NAMESPACE

class breakpoint {
    protected:
	string fname, id;
	int line;
	bool fpermanent;

    public:
	breakpoint();
	breakpoint(const string &aid, const string &afname, int aline, bool apermanent);
	breakpoint(const string &afname, int aline);
	breakpoint(const string &definition);
	~breakpoint();

	string getfname() const;
	int getline() const;
	bool permanent() const;

	bool operator == (const string &aid) const;
	bool operator != (const string &aid) const;

	bool operator == (const pair<string, int> aloc) const;
	bool operator != (const pair<string, int> aloc) const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::breakpoint;

#endif

#endif
