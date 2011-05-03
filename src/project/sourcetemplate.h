#ifndef __SOURCETEMPLATE_H__
#define __SOURCETEMPLATE_H__

#include "motorcommon.h"

__MOTOR_BEGIN_NAMESPACE

class sourcetemplate {
    protected:
	string fname;

    public:
	sourcetemplate(const string &afname);
	~sourcetemplate();

	string getfname() const;
	string getrawfname() const;

	bool generate() const;

	bool operator == (const string &afname) const;
	bool operator != (const string &afname) const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::sourcetemplate;

#endif

#endif
