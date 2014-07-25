#ifndef __UICONFIGURATION_H__
#define __UICONFIGURATION_H__

#include "motorcommon.h"
#include "uidefs.h"

__MOTOR_BEGIN_NAMESPACE

class uiconfiguration {
    protected:
	vector<INT> boldcolors;
	string schemename, lastproject;
	bool mcedclip, smarttab;

	void readthemes();

    public:
	class syntaxcolorpair {
	    protected:
		INT id, pair;
		bool bold;

	    public:
		syntaxcolorpair(INT aid, INT apair, bool bold);
		~syntaxcolorpair();

		bool isbold();
		INT getpairnumber();

		bool operator == (const INT &aid);
		bool operator != (const INT &aid);
	};

	class syntaxitem: public pair<string, INT> {
	// first is filename mask, second is syntaxhl id
	    public:
		syntaxitem(const string &afname, INT id);

		bool operator == (const string &afname);
		bool operator != (const string &afname);
	};

	uiconfiguration();
	~uiconfiguration();

	INT getcolor(INT paircode) const;
	string getschemename() const;

	string getclipboardfname() const;
	string getlastproject() const;

	bool getmcedclip() const;
	void setmcedclip(bool af);

	bool getsmarttab() const;
	void setsmarttab(bool asmarttab);

	void load();
	void save();

	INT getwatchlines() const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::uiconfiguration;

#endif

extern uiconfiguration uiconf;

#endif
