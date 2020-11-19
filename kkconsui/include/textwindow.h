#ifndef __KONST_UI_WINDOW_H_
#define __KONST_UI_WINDOW_H_

#include "conf.h"
#include "conscommon.h"
#include "abstractui.h"

enum textwindowtype {
    TW_CENTERED = 2,
    TW_NOBORDER = 4,
    TW_SPACEBORDER = 8
};

__KTOOL_BEGIN_NAMESPACE

class textwindow: public abstractuicontrol {
    private:
	string title;
	int wc, tc, options;
	
    public:
	textwindow();
	textwindow(const textwindow &aw);
	textwindow(int xx1, int yy1, int xx2, int yy2, int wcolor,
	    int noptions = 0, int tcolor = 0, const char *tfmt = 0, ...);
	~textwindow();

	void write(int x, int y, int c, const string &text);
	void write(int x, int y, const string &text);

	void writef(int x, int y, int c, const char *fmt, ...);
	void writef(int x, int y, const char *fmt, ...);

	void separatey(int y);
	void separatex(int x);

	void setoptions(int aoptions);
	void setcolor(int awc);
	void setcoords(int x1, int y1, int x2, int y2);
	void set_titlef(int color, const char *fmt, ...);
	void set_title(int color, const string &atitle);

	void gotoxy(int x, int y);
	void open();
	void redraw();
	void close();

	bool isbordered();
};

__KTOOL_END_NAMESPACE

#ifdef __KTOOL_USE_NAMESPACES

using ktool::textwindow;

#endif

#endif
