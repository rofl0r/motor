#ifndef __MOTORGREPPER_H__
#define __MOTORGREPPER_H__

#include "motorcommon.h"
#include "motorui.h"

#define MG_REGEXP	2
#define MG_CASESENS	4
#define MG_WHOLEWORD	8

__MOTOR_BEGIN_NAMESPACE

class motorgrepper {
    public:
	enum grepmode { currentfile, folders, allfiles, connected };

    protected:
	grepmode mode;
	string pattern;
	int options;
	regex_t re;

	vector<motorui::editfile> openfiles;
	vector<motorui::editfile> results;
	vector<string> fadd, flst;

	void makefilelist();
	void go();
	void grepfile(const string &fname);
	int grepline(const string &s, int &subsize, int frompos);

    public:
	motorgrepper();
	~motorgrepper();

	void exec(grepmode amode, const string &apattern, int aoptions);
	void setadditional(const vector<string> a);
	    // for the "folders" and "connected" modes

	void clear();

	vector<motorui::editfile>::const_iterator begin() const;
	vector<motorui::editfile>::const_iterator end() const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::motorgrepper;

#endif

extern motorgrepper grepper;

#endif
