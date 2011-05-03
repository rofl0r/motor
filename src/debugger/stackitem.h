#ifndef __STACKITEM_H__
#define __STACKITEM_H__

#include "motorcommon.h"
#include "motorui.h"

__MOTOR_BEGIN_NAMESPACE

class stackitem {
    private:
	string name, args;
	motorui::editfile location;

    public:
	stackitem();
	stackitem(const string &aname, const string &aargs, motorui::editfile alocation);
	~stackitem();

	string getname() const;
	string getarguments() const;
	const motorui::editfile getlocation() const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::stackitem;

#endif

#endif
