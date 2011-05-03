#ifndef __UIVCS_H__
#define __UIVCS_H__

#include "motorcommon.h"
#include "ncursesui.h"
#include "motorvcs.h"

__MOTOR_BEGIN_NAMESPACE

class uivcs {
    private:
	bool enabled();

    public:
	uivcs();
	~uivcs();

	void check();
	void forkbranch();
	void tag();
	void import();
	void explore();
	void merge();
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::uivcs;

#endif

extern uivcs uv;

#endif
