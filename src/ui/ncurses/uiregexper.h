#ifndef __UIREGEXPER_H__
#define __UIREGEXPER_H__

#include "motorcommon.h"
#include "ncursesui.h"

__MOTOR_BEGIN_NAMESPACE

class uiregexper {
    protected:
	dialogbox db;
	string regexp, example;
	bool initialized;

    public:
	uiregexper();
	~uiregexper();

	void setregexp(const string &aregexp);
	void setexample(const string &aexample);

	string getregexp() const;

	void exec();
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::uiregexper;

#endif

extern uiregexper regexper;

#endif
