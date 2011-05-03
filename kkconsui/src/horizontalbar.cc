/*
*
* kkconsui horizontalbar class
* $Id: horizontalbar.cc,v 1.3 2002/03/30 17:47:54 konst Exp $
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

#include "horizontalbar.h"

horizontalbar::horizontalbar(int x, int y, int nc, int sc, ...):
done(false), item(0), previtem(-1), scolor(sc), ncolor(nc) {
    char *p;
    va_list ap;

    setcoords(x, y);
    va_start(ap, sc);
    while(p = va_arg(ap, char *)) items.push_back((string) p);
    va_end(ap);
}

horizontalbar::horizontalbar(int nc, int sc, ...):
done(false), item(0), previtem(-1), scolor(sc), ncolor(nc) {
    char *p;
    va_list ap;

    va_start(ap, sc);
    while(p = va_arg(ap, char *)) items.push_back((string) p);
    va_end(ap);
}

horizontalbar::~horizontalbar() {
}

void horizontalbar::movebar(int k) {
    previtem = item;
    done = false;

    switch(k) {
	case KEY_RIGHT:
	case KEY_UP:
	    if(++item >= items.size()) item = 0;
	    break;

	case KEY_LEFT:
	case KEY_DOWN:
	    if(--item < 0) item = items.size()-1;
	    break;

	case '\r':
	    done = true;
	    break;

	case 27:
	    done = true;
	    item = -1;
	    break;
    }
    
    switch(k) {
	case KEY_RIGHT:
	case KEY_UP:
	case KEY_LEFT:
	case KEY_DOWN:
	    update();
	    break;
    }
}

void horizontalbar::redraw() {
    int cx = x1, n;
    vector<string>::iterator i;

    for(i = items.begin(), n = 0; i != items.end(); n++, i++) {
	kwriteatf(cx, y1, item == n ? scolor : ncolor, " %s ", i->c_str());
	cx += i->size() + 3;
    }
}

void horizontalbar::update() {
    int cx = x1, n;
    vector<string>::iterator i;

    for(i = items.begin(), n = 0; i != items.end(); i++, n++) {
	if((n == previtem) || (n == item))
	    kwriteatf(cx, y1, item == n ? scolor : ncolor, " %s ", i->c_str());

	cx += i->size() + 3;
    }
}

void horizontalbar::align(bamode m) {
    vector<string>::iterator i;
    
    switch(m) {
	case baleft:
	    for(i = items.begin(); i != items.end(); i++) {
		x1 -= i->size()+2;
	    }
	    x1 -= items.size()-1;
	    break;
    }
}

void horizontalbar::setcoords(int x, int y) {
    x1 = x, y1 = y;
}

