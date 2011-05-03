#ifndef __PROJECTDEBUG_H__
#define __PROJECTDEBUG_H__

#include "motorcommon.h"
#include "projectname.h"
#include "debuggercommand.h"
#include "debuggermessage.h"
#include "breakpoint.h"
#include "watch.h"

__MOTOR_BEGIN_NAMESPACE

/*
*
* This class is intended for holding project
* template debugger related settings.
*
*/

class projectdebug/*: virtual public projectname*/ {
    private:
	projectname pname;

    public:
	enum debuggermode {
	    dm_normal,
	    dm_nodebug,
	    dm_noexec
	};

    protected:
	debuggermode fdm;
	vector<debuggermessage>::iterator lastdmsg;

	vector<debuggercommand> dcommands;
	vector<debuggermessage> dmessages;

	vector<breakpoint> bpoints;
	vector<string> watches;

	vector<string> cmdinit, cmdonterminate;

    public:
	projectdebug();
	projectdebug(const projectname aname);
	~projectdebug();

	vector<string> getdebuginit() const;
	vector<string> getdebugonterminate() const;

	vector<debuggercommand>::const_iterator dcmdbegin() const;
	vector<debuggercommand>::const_iterator dcmdend() const;

	vector<debuggermessage>::const_iterator dmsgbegin() const;
	vector<debuggermessage>::const_iterator dmsgend() const;

	bool load(const projectname aname);

	void populateparselist(pparamslist *parselist);
	void debugload();

	debuggermode getdebuggermode() const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::projectdebug;

#endif

#endif
