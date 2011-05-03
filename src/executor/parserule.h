#ifndef __PARSERULE_H__
#define __PARSERULE_H__

#include "motorcommon.h"

#include <regex.h>

__MOTOR_BEGIN_NAMESPACE

class parserule {
    public:
	enum messagekind {
	    warning,
	    error,
	    info
	};

    protected:
	string mask, porder;
	    // regexp to match

	string target;
	    // associated make target

	mutable regex_t rex;
	mutable bool rexcompiled;

	messagekind kind;
	mutable vector<string> values;

    public:
	parserule(const string &atarget, const string &amask, messagekind kind = info);
	parserule(const parserule &p);
	~parserule();

	bool exec(const string &buf) const;
	string getvalue(const string &n = "") const;
	messagekind getkind() const;

	bool operator == (const pair<string, string> &ast) const;
	bool operator != (const pair<string, string> &ast) const;
	    // first is a string and second is a target
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::parserule;

#endif

#endif
