#ifndef __UICONFIGURATION_H__
#define __UICONFIGURATION_H__

#include "motorcommon.h"
#include "uidefs.h"

__MOTOR_BEGIN_NAMESPACE

class uiconfiguration {
    protected:
	vector<int> boldcolors;
	string schemename, lastproject;
	bool mcedclip, smarttab;

	void readthemes();

    public:
	class syntaxcolorpair {
	    protected:
		int id, pair;
		bool bold;

	    public:
		syntaxcolorpair(int aid, int apair, bool bold);
		~syntaxcolorpair();

		bool isbold();
		int getpairnumber();

		bool operator == (const int &aid);
		bool operator != (const int &aid);
	};

	class syntaxitem: public pair<string, int> {
	// first is filename mask, second is syntaxhl id
	    public:
		syntaxitem(const string &afname, int id);

		bool operator == (const string &afname);
		bool operator != (const string &afname);
	};

	uiconfiguration();
	~uiconfiguration();

	int getcolor(int paircode) const;
	string getschemename() const;

	string getclipboardfname() const;
	string getlastproject() const;

	bool getmcedclip() const;
	void setmcedclip(bool af);

	bool getsmarttab() const;
	void setsmarttab(bool asmarttab);

	void load();
	void save();

	int getwatchlines() const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::uiconfiguration;

#endif

extern uiconfiguration uiconf;

#endif
