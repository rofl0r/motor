#ifndef __KONST_UI_TEXT_H_
#define __KONST_UI_TEXT_H_

#include <vector>

#include "conf.h"
#include "conscommon.h"
#include "abstractui.h"

__KTOOL_BEGIN_NAMESPACE

class textbrowser: public abstractuicontrol {
    protected:
	INT ncolor, line, endline;
	vector<string> lines;
	string buf;

	void draw(INT line);
	
    public:
	INT (*otherkeys)(textbrowser &caller, INT k);
	void (*idle)(textbrowser &caller);
	
	textbrowser(INT nx1, INT ny1, INT nx2, INT ny2, INT clr);
	textbrowser(INT clr = 0);
	~textbrowser();

	void setcolor(INT clr);
	void setcoords(INT nx1, INT ny1, INT nx2, INT ny2);
	void redraw();
	void move(INT k);

	INT open();

	bool empty();
	void setbuf(const string &p);
};

__KTOOL_END_NAMESPACE

#ifdef __KTOOL_USE_NAMESPACES

using ktool::textbrowser;

#endif

#endif
