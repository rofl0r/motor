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
	    int color, kind;
	    void *ref;
	};

	vector<verticalmenuitem> items;
	textwindow window;

	int ncolor, scolor;
	int curelem, firstdisp, lastkey;

	void initmembers();
	
	bool shownelem(int n, int selected);
	void showall();

	void checkclear();
	void intredraw();

    public:
	bool clearonfocuslost;
	bool cycled;
	bool exitonedges;

	int (*otherkeys)(verticalmenu &caller, int k);
	    // function called on pressing of non-standard menu keys
	    // should return a number of selected menu item or -1 to
	    // continue browsing

	void (*idle)(verticalmenu &caller);

	verticalmenu(int px1, int py1, int px2, int py2, int pncolor, int pscolor);
	verticalmenu(int pncolor = 0, int pscolor = 0);
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

	void additem(int color, void *ref, const string &text);
	void additemf(int color, void *ref, const char *fmt, ...);

	void additem(int color, int ref, const string &text);
	void additemf(int color, int ref, const char *fmt, ...);

	void addline();
	void addline(int color, const char *fmt = 0, ...);

	void clear();
	void redraw();

	bool empty();

	int getcount();
	int getlastkey();
	void *getref(int n);

	int getpos();
	void getpos(int &cur, int &first);
	void setpos(int cur, int first = -1);
	void setitemcolor(int pos, int color);
	void setwindow(textwindow nwindow);
	void remove(int pos);

	int open();
	void scale();
	void close();
	void setcoords(int nx1, int ny1, int nx2, int ny2);
	void setcolor(int pncolor, int pscolor);

    friend class horizontalmenu;
};

struct horizontalmenuitem {
    string text;
    int color;
    verticalmenu menu;

    horizontalmenuitem();
    horizontalmenuitem(const horizontalmenuitem &a);
    ~horizontalmenuitem();
};

class horizontalmenu: public abstractuicontrol {
    protected:
	vector<horizontalmenuitem> menus;
	int ncolor, scolor, fcolor, coordy, coordx, selected;

	static int menu_otherkeys(verticalmenu &ref, int k);

	void draw();
	void moveelem(int old);
	void saveline();
	void restoreline();

	int getx(int n);
	int menulen(int n);

    public:
	bool (*otherkeys)(horizontalmenu &caller, int k);
	void (*idle)(horizontalmenu &caller);

	horizontalmenu(int x, int y, int normcolor, int selcolor, int framecolor);
	horizontalmenu();
	~horizontalmenu();

	void additemf(int color, const char *fmt, ...);
	void additemf(const char *fmt, ...);

	void additem(int color, const string &text);
	void additem(const string &text);

	verticalmenu *pulldown(int n);

	virtual void redraw();
	bool open(int *hor, int *pulld);
	void close();
};

__KTOOL_END_NAMESPACE

#ifdef __KTOOL_USE_NAMESPACES

using ktool::verticalmenu;
using ktool::horizontalmenu;

#endif

#endif
