#ifndef __WATCH_H__
#define __WATCH_H__

#include "motorcommon.h"

__MOTOR_BEGIN_NAMESPACE

class watch {
    protected:
	string expression;

    public:
	watch(const string &aexpression);
	~watch();
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::watch;

#endif

#endif
