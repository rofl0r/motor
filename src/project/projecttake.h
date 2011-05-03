#ifndef __PROJECTTAKE_H__
#define __PROJECTTAKE_H__

#include "motorcommon.h"

__MOTOR_BEGIN_NAMESPACE

class projecttake {
    private:
	enum takefromkind { folder, text } kind;
	string placefrom, placeto, sourcetemplate;

    public:
	projecttake();
	projecttake(const string &definition);
	~projecttake();

	void exec(const string &pfrom);
	void undo(const string &pfrom);

	string getsourcetemplate();
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::projecttake;

#endif

#endif
