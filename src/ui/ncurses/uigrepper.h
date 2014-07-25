#ifndef __UIGREPPER_H__
#define __UIGREPPER_H__

#include "motorcommon.h"
#include "ncursesui.h"
#include "motorgrepper.h"

__MOTOR_BEGIN_NAMESPACE

class uigrepper {
    private:
	bool initmode, replacemode;
	vector<motorui::editfile>::const_iterator currentgrep;
	string pattern, replacement;
	INT grepoptions;

	bool fregexp, fcase, fwhole;
	motorgrepper::grepmode grepmode;

	bool init();
	bool go();

	bool replacego();
	bool collectgo();
	bool currentgo();

    public:
	uigrepper();
	~uigrepper();

	void grep();
	void replace();
	void again();
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::uigrepper;

#endif

extern uigrepper uigrep;

#endif
