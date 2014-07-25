#ifndef __KONST_UI_MENU_H_
#define __KONST_UI_MENU_H_

#include "conf.h"
#include "conscommon.h"
#include "textwindow.h"

#include <string>
#include <vector>

__KTOOL_BEGIN_NAMESPACE

#define ITEM_NORM       0
#define ITEM_SEP        1
#define ITEM_LINE       2

class verticalmenu: public abstractuicontrol {
    private:
	struct verticalmenuitem {
	    string text;
	    INT color, kind;
	    void *ref;
	};

	vector<verticalmenuitem> items;
	textwindow window;

	INT ncolor, scolor;
	INT curelem, firstdisp, lastkey;

	void initmembers();
	
	bool shownelem(INT n, INT selected);
	void showall();

	void checkclear();
	void intredraw();

    public:
	bool clearonfocuslost;
	bool cycled;
	bool exitonedges;

	INT (*otherkeys)(verticalmenu &caller, INT k);
	    // function called on pressing of non-standard menu keys
	    // should return a number of selected menu item or -1 to
	    // continue browsing

	void (*idle)(verticalmenu &caller);

	verticalmenu(INT px1, INT py1, INT px2, INT py2, INT pncolor, INT pscolor);
	verticalmenu(INT pncolor = 0, INT pscolor = 0);
	~verticalmenu();

	// following special characters are valid in the items text:

	//   \001 == switch the color attribute
	//   \002 == HLINE    [ -  ]
	//   \003 == VLINE    [ |  ]
	//   \004 == RTEE     [ -| ]
	//   \005 == LTEE     [ |- ]
	//   \006 == ULCORNER [ |^ ]
	//   \007 == LLCORNER [ |_ ]
	//   \008 == LRCORNER [ _| ]
	//   \009 == URCORNER [ ^| ]

	void additemf(const char *fmt, ...);
	void additem(const string &text);

	void additem(INT color, void *ref, const string &text);
	void additemf(INT color, void *ref, const char *fmt, ...);

	void additem(INT color, INT ref, const string &text);
	void additemf(INT color, INT ref, const char *fmt, ...);

	void addline();
	void addline(INT color, const char *fmt = 0, ...);

	void clear();
	void redraw();

	bool empty();

	INT getcount();
	INT getlastkey();
	void *getref(INT n);

	INT getpos();
	void getpos(INT &cur, INT &first);
	void setpos(INT cur, INT first = -1);
	void setitemcolor(INT pos, INT color);
	void setwindow(textwindow nwindow);
	void remove(INT pos);

	INT open();
	void scale();
	void close();
	void setcoords(INT nx1, INT ny1, INT nx2, INT ny2);
	void setcolor(INT pncolor, INT pscolor);

    friend class horizontalmenu;
};

struct horizontalmenuitem {
    string text;
    INT color;
    verticalmenu menu;

    horizontalmenuitem();
    horizontalmenuitem(const horizontalmenuitem &a);
    ~horizontalmenuitem();
};

class horizontalmenu: public abstractuicontrol {
    protected:
	vector<horizontalmenuitem> menus;
	INT ncolor, scolor, fcolor, coordy, coordx, selected;

	static INT menu_otherkeys(verticalmenu &ref, INT k);

	void draw();
	void moveelem(INT old);
	void saveline();
	void restoreline();

	INT getx(INT n);
	INT menulen(INT n);

    public:
	bool (*otherkeys)(horizontalmenu &caller, INT k);
	void (*idle)(horizontalmenu &caller);

	horizontalmenu(INT x, INT y, INT normcolor, INT selcolor, INT framecolor);
	horizontalmenu();
	~horizontalmenu();

	void additemf(INT color, const char *fmt, ...);
	void additemf(const char *fmt, ...);

	void additem(INT color, const string &text);
	void additem(const string &text);

	verticalmenu *pulldown(INT n);

	virtual void redraw();
	bool open(INT *hor, INT *pulld);
	void close();
};

__KTOOL_END_NAMESPACE

#ifdef __KTOOL_USE_NAMESPACES

using ktool::verticalmenu;
using ktool::horizontalmenu;

#endif

#endif
