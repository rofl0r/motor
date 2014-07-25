#ifndef __MOTORTAGBROWSER_H__
#define __MOTORTAGBROWSER_H__

#include "motorcommon.h"
#include "motorui.h"

__MOTOR_BEGIN_NAMESPACE

class motortagbrowser {
    public:
	enum viewscope {
	    File = 0,
	    Project,
	    Everything,
	    viewscope_size
	};

	class record {
	    private:
		string name, fname, info;
    		char kind;
    		INT lineno;

	    public:
		record();
		record(const string &adef);
		~record();

		motorui::editfile geteditfile() const;
		string getname() const;
		string getinfo() const;
		string getbrowsertagname() const;
		char getkind() const;
	};

    private:
	vector<record> tags;
	viewscope scope;

	bool readok;

	void readtags();

    public:
	motortagbrowser();
	~motortagbrowser();

	vector<record> fetch(const string &prefix = "", const string &filter = "");

	void setscope(viewscope ascope);
	viewscope getscope() const;

	void clear();

	bool ok();
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::motortagbrowser;

#endif

extern motortagbrowser tagbrowser;

#endif
