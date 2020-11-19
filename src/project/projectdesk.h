#ifndef __PROJECTDESK_H__
#define __PROJECTDESK_H__

#include "motorcommon.h"
#include "projectname.h"
#include "motorui.h"

__MOTOR_BEGIN_NAMESPACE

/*
*
* This class is intended for holding project specific desktop settings.
*
*/

class projectdesk/*: virtual public projectname*/ {
    private:
	projectname pname;

    protected:
	string arguments;
	vector< pair<string, string> > settings;
	vector<motorui::editfile> fedit;

	void loadeditfile(const string &fname, int x, int y);

    public:
	projectdesk();
	projectdesk(const projectname aname);
	~projectdesk();

	string getarguments() const;
	void setarguments(const string &aarguments);

	bool load(const projectname aname);
	void editload();

	void populateparselist(pparamslist *parselist);
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::projectdesk;

#endif

#endif
