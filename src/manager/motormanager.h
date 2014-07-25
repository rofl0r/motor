#ifndef __MOTORMANAGER_H__
#define __MOTORMANAGER_H__

#include "motorcommon.h"
#include "projectname.h"

__MOTOR_BEGIN_NAMESPACE

class motormanager {
    private:
	vector<string> container;

	static INT addtemplate(const char *file, const struct stat *sb, INT flag);

    public:
	motormanager();
	~motormanager();

	vector<projectname> getprojectlist();
	vector<string> gettemplatelist();
	vector<string> getvcslist();
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::motormanager;

#endif

extern motormanager manager;

#endif
