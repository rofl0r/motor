#ifndef __DEBUGGERCOMMAND_H__
#define __DEBUGGERCOMMAND_H__

#include "motorcommon.h"

__MOTOR_BEGIN_NAMESPACE

class debuggercommand {
    public:
	enum commandkind {
	    breakmain, breakpoint, kill, next, step, run, cont, loadcore,
	    stack, breakpointdel, setvar, getvar
	};

    protected:
	string text;
	commandkind kind;

    public:
	debuggercommand(const string &atext, commandkind akind);
	~debuggercommand();

	string get(pparamslist *plist) const;

	bool operator == (const commandkind akind) const;
	bool operator != (const commandkind akind) const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::debuggercommand;

#endif

#endif
