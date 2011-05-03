#ifndef __MOTORDIST_H__
#define __MOTORDIST_H__

#include "motorcommon.h"

__MOTOR_BEGIN_NAMESPACE

class motordist {
    public:
	struct distparam {
	    string name, title, defval;
	    distparam(const string &aname, const string &atitle, const string &adefval = ""):
		name(aname), title(atitle), defval(adefval) {}

	    bool operator == (const string &aname) { return aname == name; }
	    bool operator != (const string &aname) { return aname != name; }
	};

    private:
	string name, target;
	vector<distparam> parameters;

    public:
	motordist(const string &aname, const string &atarget);
	~motordist();

	string getname() const;
	void exec(const string &destdir) const;

	void addparameter(const string &name, const string &title)
	    { parameters.push_back(distparam(name, title)); }

	vector<distparam>::iterator parambegin() { return parameters.begin(); }
	vector<distparam>::iterator paramend() { return parameters.end(); }

	vector<distparam>::const_iterator parambegin() const { return parameters.begin(); }
	vector<distparam>::const_iterator paramend() const { return parameters.end(); }

	bool operator == (const string &d) const { return name == d; }
	bool operator != (const string &d) const { return name != d; }
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::motordist;

#endif

#endif
