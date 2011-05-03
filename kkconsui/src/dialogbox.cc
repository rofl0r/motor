/*
*
* kkconsui dialogbox class
* $Id: dialogbox.cc,v 1.14 2003/10/19 23:25:05 konst Exp $
*
* Copyright (C) 1999-2001 by Konstantin Klyagin <konst@konst.org.ua>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at
* your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
* USA
*
*/

#include "dialogbox.h"

dialogbox *dialogbox::it;

dialogbox::dialogbox(): menu(0), tree(0), window(0), bar(0), browser(0),
idle(0), otherkeys(0), first(true) {
    freemenu = freetree = freewindow = freebar = freebrowser = false;
}

dialogbox::~dialogbox() {
    if(menu && freemenu) delete menu;
    if(tree && freetree) delete tree;
    if(bar && freebar) delete bar;
    if(window && freewindow) delete window;
    if(browser && freebrowser) delete browser;
}

void dialogbox::setwindow(textwindow *neww, bool fw) {
    window = neww;
    freewindow = fw;
}

void dialogbox::setbar(horizontalbar *newb, bool fb) {
    bar = newb;
    freebar = fb;
}

void dialogbox::setmenu(verticalmenu *newm, bool fm) {
    menu = newm;
    freemenu = fm;
}

void dialogbox::settree(treeview *newt, bool ft) {
    tree = newt;
    freetree = ft;
}

void dialogbox::setbrowser(textbrowser *newbr, bool fbr) {
    browser = newbr;
    freebrowser = fbr;
}

verticalmenu *dialogbox::getmenu() {
    return menu;
}

treeview *dialogbox::gettree() {
    return tree;
}

textwindow *dialogbox::getwindow() {
    return window;
}

horizontalbar *dialogbox::getbar() {
    return bar;
}

textbrowser *dialogbox::getbrowser() {
    return browser;
}

bool dialogbox::open(int &menuitem, int &baritem, void **ref) {
    bool ret = false;
    list<keybarassociation>::iterator i;

    it = this;
    if(first) redraw();

    if(menu) {
	menuitem = menu->open();
	if(ref) *ref = menu->getref(menuitem-1);
	ret = menuitem || (menu->getlastkey() != KEY_ESC);
    } else if(tree) {
	void *r;
	r = tree->open(&menuitem);
	if(ref) *ref = r;
	ret = menuitem || (tree->menu.getlastkey() != KEY_ESC) || r;
    } else if(browser) {
	menuitem = browser->open();
	ret = menuitem;
    } else if(bar) {
	bool fin, proceed;
	int k;
	menuitem = 0;

	for(bool fin = false; !fin; ) {
	    proceed = idle ? keypressed() : true;
	    it = this;

	    if(proceed)
	    switch(k = getkey()) {
		case KEY_LEFT:
		case KEY_RIGHT:
		    bar->movebar(k);
		    bar->redraw();
		    break;
		case '\r':
		    fin = ret = true;
		    break;
		case KEY_ESC:
		    fin = true; ret = false;
		    break;
		default:
		    i = find(it->kba.begin(), it->kba.end(), k);

		    if(i != it->kba.end()) {
		      if(it->bar) {
			it->bar->item = i->baritem;
			it->bar->redraw();
			fin = ret = true;
		      }
		    }
		    else if(otherkeys)
		      if((k = otherkeys(*this, k)) != -1) {
			menuitem = k;
			fin = true;
		      }
		    break;
	    } else {
		if(idle) idle(*this);
	    }
	}
    }

    if(bar) {
	baritem = bar->item;
    } else {
	baritem = 0;
    }

    return ret;
}

bool dialogbox::open(int &menuitem) {
    int bi;
    return open(menuitem, bi);
}

bool dialogbox::open() {
    int menuitem, bi;
    return open(menuitem, bi);
}

void dialogbox::redraw() {
    if(window) {
	if(window->isopen()) window->redraw(); else window->open();

	if(bar) {
	    if(window->isbordered()) {
		window->separatey(window->y2-window->y1-2);
	    }

	    bar->setcoords(window->x2 - (window->isbordered() ? 1 : 2), window->y2-1);
	    bar->align(baleft);
	    bar->redraw();
	}

	int x1 = window->x1 + (window->isbordered() ? 1 : 0);

	if(menu) {
	    menu->setcoords(x1, window->y1+1, window->x2, bar ? window->y2-2 : window->y2);
	    menu->idle = &menuidle;
	    menu->otherkeys = &menukeys;
	} else if(tree) {
	    tree->setcoords(x1, window->y1+1, window->x2, bar ? window->y2-2 : window->y2);
	    tree->redraw();
	    tree->menu.idle = &menuidle;
	    tree->menu.otherkeys = &menukeys;
	} else if(browser) {
	    int x2 = window->x2 - (window->isbordered() ? 1 : 0);

	    browser->setcoords(window->x1+1, window->y1+1, x2,
		bar ? window->y2-2 : window->y2);

	    browser->redraw();
	    browser->idle = &browseridle;
	    browser->otherkeys = &browserkeys;
	}
    }

    first = false;
}

void dialogbox::close() {
    if(window) window->close();
}

void dialogbox::clearkeys() {
    kba.clear();
}

void dialogbox::addkey(int key, int baritem) {
    kba.push_back(keybarassociation(key, baritem));
}

void dialogbox::addautokeys() {
    int key;
    vector<string>::const_iterator i;
    string::const_iterator c;

    if(bar) {
	for(i = bar->items.begin(); i != bar->items.end(); i++) {
	    for(c = i->begin(); (c != i->end()) && !isupper(*c); c++);
	    key = c == i->end() ? key = *i->begin() : *c;
	    addkey(tolower(key), i-bar->items.begin());
	    addkey(toupper(key), i-bar->items.begin());
	}
    }
}

void dialogbox::menuidle(verticalmenu &caller) {
    if(it->idle) {
	it->idle(*it);
    } else {
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(0, &fds);
	select(1, &fds, 0, 0, 0);
    }
}

void dialogbox::browseridle(textbrowser &caller) {
    if(it->idle) {
	it->idle(*it);
    } else {
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(0, &fds);
	select(1, &fds, 0, 0, 0);
    }
}

int dialogbox::menukeys(verticalmenu &caller, int k) {
    list<keybarassociation>::iterator i;
    bool found;
    int ip;
    dialogbox *sit;

    switch(k) {
	case KEY_LEFT:
	case KEY_RIGHT:
	    if(it->bar) {
		it->bar->movebar(k);
		it->bar->redraw();
	    }
	    break;
	case '-':
	case '+':
	    if(it->tree)
	    if(it->tree->collapsable) {
		int nid = it->tree->getid(it->tree->menu.getpos());

		if(it->tree->isnode(nid)) {
		    if(it->tree->isnodeopen(nid)) it->tree->closenode(nid);
		    else it->tree->opennode(nid);

		    it->tree->menu.redraw();
		}
	    }

	default:
	    i = find(it->kba.begin(), it->kba.end(), k);

	    if(i != it->kba.end()) {
		if(it->bar) {
		    it->bar->item = i->baritem;
		    it->bar->redraw();
		}

		if(it->menu) {
		    return it->menu->getpos()+1;
		} else if(it->tree) {
		    return it->tree->menu.getpos()+1;
		}
	    } else {
		if(it->otherkeys) {
		    sit = it;
		    ip = it->otherkeys(*it, k);
		    it = sit;
		    return ip;
		}
	    }
	    break;
    }

    return -1;
}

int dialogbox::browserkeys(textbrowser &caller, int k) {
    list<keybarassociation>::iterator i;
    bool found;
    int ip;
    dialogbox *sit;

    switch(k) {
	case KEY_LEFT:
	case KEY_RIGHT:
	    if(it->bar) {
		it->bar->movebar(k);
		it->bar->redraw();
	    }
	    break;
	case '\r':
	    return 1;

	default:
	    i = find(it->kba.begin(), it->kba.end(), k);

	    if(i != it->kba.end()) {
		if(it->bar) {
		    it->bar->item = i->baritem;
		    it->bar->redraw();
		}

		return 1;
	    } else {
		if(it->otherkeys) {
		    sit = it;
		    ip = it->otherkeys(*it, k);
		    it = sit;
		    return ip;
		}
	    }
	    break;
    }

    return -1;
}

// ----------------------------------------------------------------------------

dialogbox::keybarassociation::keybarassociation(int nkey, int nbarit) {
    key = nkey;
    baritem = nbarit;
}

bool dialogbox::keybarassociation::operator == (const int akey) {
    return key == akey;
}

bool dialogbox::keybarassociation::operator != (const int akey) {
    return !(*this == akey);
}
