#ifndef __PROJECTFILES_H__
#define __PROJECTFILES_H__

#include "motorcommon.h"
#include "projectname.h"
#include "motorfolder.h"

__MOTOR_BEGIN_NAMESPACE

/*
*
* This class is intended for holding project folders and files.
*
*/

class projectfiles/*: virtual public projectname*/ {
    private:
	projectname pname;

    protected:
	vector<motorfolder> folders;

	void loadfolders();

    public:
	projectfiles();
	projectfiles(const projectname aname);
	~projectfiles();

	vector<motorfolder>::iterator foldbegin();
	vector<motorfolder>::iterator foldend();

	bool load(const projectname aname);

	void removefile_r(const string &mask, motorfile::filekind kind);
	vector<motorfile> getfiles_r(const string &mask, motorfile::filekind kind);
	vector<string> extractdirectories();
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::projectfiles;

#endif

#endif
