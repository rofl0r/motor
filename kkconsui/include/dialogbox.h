#ifndef __KONST_UI_DLG_H__
#define __KONST_UI_DLG_H__

#include <list>
#include <algorithm>
#include <unistd.h>

#include "conf.h"
#include "textwindow.h"
#include "cmenus.h"
#include "treeview.h"
#include "horizontalbar.h"
#include "textbrowser.h"

__KTOOL_BEGIN_NAMESPACE

class dialogbox {
    protected:
	static dialogbox *it;

        class keybarassociation {
            public:
                INT key, baritem;
                keybarassociation(INT nkey, INT nbarit);

                bool operator == (const INT akey);
                bool operator != (const INT akey);
        };

        bool freemenu, freetree, freewindow, freebar, freebrowser, first;

        verticalmenu *menu;
        treeview *tree;
        textwindow *window;
        horizontalbar *bar;
        textbrowser *browser;

        list<keybarassociation> kba;

        static void menuidle(verticalmenu &caller);
        static void browseridle(textbrowser &caller);
        static INT menukeys(verticalmenu &caller, INT k);
        static INT browserkeys(textbrowser &caller, INT k);

    public:
        INT (*otherkeys)(dialogbox &caller, INT k);
            // function called on pressing of non-standard menu keys
            // should return a number of selected menu item or -1 to
            // continue browsing

        void (*idle)(dialogbox &caller);

        dialogbox();
        ~dialogbox();

        void setwindow(textwindow *neww, bool fw = true);
        void setbar(horizontalbar *newb, bool fb = true);
        void setmenu(verticalmenu *newm, bool fm = true);
        void settree(treeview *newt, bool ft = true);
        void setbrowser(textbrowser *newbr, bool fbr = true);

        verticalmenu *getmenu();
        treeview *gettree();
        textwindow *getwindow();
        horizontalbar *getbar();
        textbrowser *getbrowser();

        void redraw();
        bool open(INT &menuitem, INT &baritem, void **ref = 0);
        bool open(INT &menuitem);
        bool open();
        void close();

        void clearkeys();
        void addkey(INT key, INT baritem);
	void addautokeys();
};

__KTOOL_END_NAMESPACE

#ifdef __KTOOL_USE_NAMESPACES

using ktool::dialogbox;

#endif

#endif
