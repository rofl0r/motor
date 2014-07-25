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
	INT wc, tc, options;
	
    public:
	textwindow();
	textwindow(const textwindow &aw);
	textwindow(INT xx1, INT yy1, INT xx2, INT yy2, INT wcolor,
	    INT noptions = 0, INT tcolor = 0, const char *tfmt = 0, ...);
	~textwindow();

	void write(INT x, INT y, INT c, const string &text);
	void write(INT x, INT y, const string &text);

	void writef(INT x, INT y, INT c, const char *fmt, ...);
	void writef(INT x, INT y, const char *fmt, ...);

	void separatey(INT y);
	void separatex(INT x);

	void setoptions(INT aoptions);
	void setcolor(INT awc);
	void setcoords(INT x1, INT y1, INT x2, INT y2);
	void set_titlef(INT color, const char *fmt, ...);
	void set_title(INT color, const string &atitle);

	void gotoxy(INT x, INT y);
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
