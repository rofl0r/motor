#ifndef __UITAGBROWSER_H__
#define __UITAGBROWSER_H__

#include "motorcommon.h"
#include "ncursesui.h"
#include "motortagbrowser.h"

__MOTOR_BEGIN_NAMESPACE

class uitagbrowser {
    private:
	string search, filter;
	textwindow w;
	verticalmenu m;
	bool modified;
	int mpos;
	vector<motortagbrowser::record> tags;

	static int mkeys(verticalmenu &m, int k);
	static int multiplekeys(verticalmenu &m, int k);

	void makemenu(verticalmenu &m);
	void input();
	bool open(string &fname, int &lineno);

	void changemode();
	void changefilter();

	void title();
	void reload();

    public:
	uitagbrowser();
	~uitagbrowser();

	string getfilter() const { return filter; }
	void setfilter(const string &afilter) { filter = afilter; }

	void exec();
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::uitagbrowser;

#endif

extern uitagbrowser uitb;

#endif
