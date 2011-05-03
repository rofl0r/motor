#ifndef __PROJECTNAME_H__
#define __PROJECTNAME_H__

#include "motorcommon.h"

__MOTOR_BEGIN_NAMESPACE

/*
*
* This class is intended for holding project names. To be exact,
* these are project name itself and a name of template.
*
*/

class projectname {
    private:
	string name, templname, version, revision;
	bool loaded, fmodified;

    protected:
	void clear();
	bool convert4();

    public:
	projectname();
	projectname(const string &pname/*, const string revision*/);
	~projectname();

	void setmodified(bool exp = true);

	virtual string getname() const;
	virtual string getregfname() const;
	virtual string getversion() const;
	virtual string gettemplatename() const;
//	virtual const string getrevision() const;

	virtual void setname(const string &aname);
	virtual void setversion(const string &aversion);
	virtual void settemplate(const string &atemplate);
//	virtual void setrevision(const string arevision);

	virtual void save();
	virtual bool load(const string &aname/*, const string arevision*/);
	virtual bool empty() const;
	virtual bool modified();

	virtual void populateparselist(pparamslist *parselist) const;
	virtual void absorb(const projectname &aname);

    public:
	virtual bool operator == (const projectname &aname) const;
	virtual bool operator != (const projectname &aname) const;
	virtual bool operator  < (const projectname &aname) const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::projectname;

#endif

#endif
