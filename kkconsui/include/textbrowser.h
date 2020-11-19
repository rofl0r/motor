#ifndef __KONST_UI_TEXT_H_
#define __KONST_UI_TEXT_H_

#include <vector>

#include "conf.h"
#include "conscommon.h"
#include "abstractui.h"

__KTOOL_BEGIN_NAMESPACE

class textbrowser: public abstractuicontrol {
    protected:
	int ncolor, line, endline;
	vector<string> lines;
	string buf;

	void draw(int line);
	
    public:
	int (*otherkeys)(textbrowser &caller, int k);
	void (*idle)(textbrowser &caller);
	
	textbrowser(int nx1, int ny1, int nx2, int ny2, int clr);
	textbrowser(int clr = 0);
	~textbrowser();

	void setcolor(int clr);
	void setcoords(int nx1, int ny1, int nx2, int ny2);
	void redraw();
	void move(int k);

	int open();

	bool empty();
	void setbuf(const string &p);
};

__KTOOL_END_NAMESPACE

#ifdef __KTOOL_USE_NAMESPACES

using ktool::textbrowser;

#endif

#endif
