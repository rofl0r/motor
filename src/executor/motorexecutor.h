#ifndef __MOTOREXECUTOR_H__
#define __MOTOREXECUTOR_H__

#include "motorcommon.h"
#include "parserule.h"

#include <stdio.h>

__MOTOR_BEGIN_NAMESPACE

class motorexecutor {
    public:
	struct message {
	    parserule::messagekind kind;
	    string fname, description;
	    INT line;
	};

    private:
	string target, lastbuf;
	vector<message> lastmessages;
	vector< pair<string, string> > variables;

	FILE *fpipe;
	INT nerr, nwarn, ninfo;

	void pop();
	void piperead();
	void setdefaultvars();

    public:
	motorexecutor();
	~motorexecutor();

	void setvar(const string &aname, const string &avalue);
	void clearvars();

	bool runmake(const string &target);

	vector<message>::const_iterator begin() const;
	vector<message>::const_iterator end() const;

	string getlastbuf() const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::motorexecutor;

#endif

extern motorexecutor executor;

#endif
