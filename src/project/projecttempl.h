#ifndef __PROJECTTEMPL_H__
#define __PROJECTTEMPL_H__

#include "motorcommon.h"
#include "projectname.h"
#include "sourcetemplate.h"
#include "parserule.h"
#include "motordist.h"

__MOTOR_BEGIN_NAMESPACE

/*
*
* This class is intended for holding project template settings.
*
*/

class projecttempl/*: virtual public projectname*/ {
    private:
	projectname pname;

    protected:
	vector<parserule> parserules;
	vector<sourcetemplate> sourcetemplates;
	vector<motordist> dists;
	vector< pair<char, string> > tagnames;

//	void clear();

    public:
	projecttempl();
	projecttempl(const projectname aname);
	~projecttempl();

	vector<parserule>::const_iterator parsebegin() const;
	vector<parserule>::const_iterator parseend() const;

	vector<sourcetemplate>::const_iterator templbegin() const;
	vector<sourcetemplate>::const_iterator templend() const;

	vector<motordist>::const_iterator distbegin() const;
	vector<motordist>::const_iterator distend() const;

	vector< pair<char, string> >::const_iterator tagnamebegin() const;
	vector< pair<char, string> >::const_iterator tagnameend() const;

	bool load(const projectname aname);

	void populateparselist(pparamslist *parselist);
	string getbrowsertagname(char kind) const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::projecttempl;

#endif

#endif
