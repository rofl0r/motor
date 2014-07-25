#ifndef __MOTORDEBUGGER_H__
#define __MOTORDEBUGGER_H__

#include "motorcommon.h"
#include "breakpoint.h"
#include "debuggercommand.h"
#include "debuggermessage.h"
#include "stackitem.h"

__MOTOR_BEGIN_NAMESPACE

class motordebugger {
    protected:
	FILE *fpread, *fpwrite;
	string outbuf, lfname, termname, result, forcedtty;
	INT lline, cpid, termnumber;
	bool factive, frunning, fcommand, rebuildasked, alwaysjustrun;
	pparamslist *plist;
	const debuggermessage *lastdmsg;

#ifdef DEBUG
	static ofstream dlog;
#endif

	vector<breakpoint> breakpoints;
	vector<string> watches;

	enum termswitchdirection { here, there };

	void init();
	void react();

	bool getfreeterm();
	void switchterm(termswitchdirection d);

        void gonext(debuggercommand::commandkind akind);

	void exec(debuggercommand::commandkind akind);
	void exec(const vector<string> cmds);
	void exec(const string &cmd);

	void readoutput();
	void getlocation();

	bool nodebug(const string &fname);
	void setparser(const string &name, const string &value);
	void setbreakpoints();
	void checkrebuildneed();

	void launch(debuggercommand::commandkind akind);

    public:
	motordebugger();
	~motordebugger();

	void run();
	void kill();
	void cont();
	void next();
	void step();
	vector<stackitem> getstack();
	void tocursor(const string &fname, INT line);

	string loadcore(const string &corefname);

	void setvar(const string &vname, const string &vvalue);
	string getvar(const string &vname);

	void removebreakpoint(const breakpoint &bp);
	void addbreakpoint(const breakpoint &bp);
	bool isbreakpoint(const breakpoint &bp) const;
	vector<breakpoint> getbreakpoints() const;

	void removewatch(const string &wexpr);
	void addwatch(const string &wexpr);
	bool iswatch(const string &wexpr) const;
	vector<string> getwatches() const;

	bool running() const;
	string getfname() const;
	INT getline() const;

	void forceusetty(const string &tname);
	void done();
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::motordebugger;

#endif

extern motordebugger debugger;

#endif
