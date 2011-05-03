#ifndef __DEBUGGERMESSAGE_H__
#define __DEBUGGERMESSAGE_H__

#include "motorcommon.h"

__MOTOR_BEGIN_NAMESPACE

class debuggermessage {
    public:
	enum messagekind {
	    terminated, cantstart, interrupted, breakpoint, setbreakpoint,
	    variablevalue, prompt, stdheader, location, stack, unknown
	};

    protected:
	messagekind kind;
	vector< pair<string, string> > lines;
	    // first is regexp
	    // second is positional order

	mutable int matchend;
	mutable string porder;
	mutable vector<string> values;

    public:
	debuggermessage(messagekind akind);
	~debuggermessage();

	void definitionadd(const string &text);

	messagekind getkind() const;

	string getvalue(const string &paramname = "") const;
	string getvalue(int n) const;
	    // the whole expression match (0) is omitted
	    // so 0 is the first sub-match found

	int getmatchendpos() const;

	bool operator == (const messagekind akind) const;
	bool operator != (const messagekind akind) const;

	bool operator == (const string &amsg) const;
	bool operator != (const string &amsg) const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::debuggermessage;

#endif

#endif
