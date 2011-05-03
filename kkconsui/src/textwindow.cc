/*
*
* kkconsui textwindow class
* $Id: textwindow.cc,v 1.9 2002/11/23 15:42:09 konst Exp $
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

#include "textwindow.h"

textwindow::textwindow() {
    x1 = x2 = y1 = y2 = options = 0;
}

textwindow::textwindow(int xx1, int yy1, int xx2, int yy2, int wcolor,
int noptions, int tcolor, const char *tfmt, ...) {
    char buf[10240];

    setoptions(noptions);
    setcoords(xx1, yy1, xx2, yy2);
    setcolor(wcolor);

    if(tfmt) {
	va_list ap;
	va_start(ap, tfmt);
	vsprintf(buf, tfmt, ap);
	va_end(ap);
	set_title(tcolor, buf);
    }
}

textwindow::textwindow(const textwindow &aw): abstractuicontrol(aw) {
    title = aw.title;
    wc = aw.wc;
    tc = aw.tc;
    options = aw.options;
}

textwindow::~textwindow() {
}

void textwindow::setcoords(int ax1, int ay1, int ax2, int ay2) {
    x1 = ax1;
    x2 = ax2;
    y1 = ay1;
    y2 = ay2;

    if(options & TW_CENTERED) {
	int h = y2-y1, w = x2-x1;
	y1 = (LINES-h)/2;
	y2 = y1 + h;
	x1 = (COLS-w)/2;
	x2 = x1 + w;
    }
}

void textwindow::setcolor(int awc) {
    wc = awc;
}

void textwindow::setoptions(int aoptions) {
    options = aoptions;
    setcoords(x1, y1, x2, y2);
}

void textwindow::set_titlef(int color, const char *fmt, ...) {
    va_list ap;
    char buf[10240];

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    set_title(color, buf);
}

void textwindow::set_title(int color, const string &atitle) {
    title = atitle;
    tc = color;
    
    if(fisopen) {
	attrset(wc);
	mvhline(y1, x1+1, HLINE, x2-x1-1);
	write(((x2 - x1) - title.size())/2, 0, tc, title);
    }
}

void textwindow::writef(int x, int y, const char *fmt, ...) {
    char buf[10240];
    va_list ap;

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    write(x, y, wc, buf);
}

void textwindow::writef(int x, int y, int c, const char *fmt, ...) {
    char buf[10240];
    va_list ap;

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    write(x, y, c, buf);
}

void textwindow::write(int x, int y, const string &text) {
    write(x, y, wc, text);
}

void textwindow::write(int x, int y, int c, const string &text) {
    int i;
    string dtext;

    if(fisopen && (y < y2-y1)) {
	for(i = 0; (i < text.size()) && (i < x2-x1-x); i++)
	    dtext += KT_DISP_FILTER(text[i]);

	attrset(c);
	mvprintw(y1 + y, x1 + x, "%s", dtext.c_str());
	refresh();
    }
}

void textwindow::gotoxy(int x, int y) {
    if(fisopen) kgotoxy(x1 + x, y1 + y);
}

void textwindow::redraw() {
    if(fisopen) {
	int i;
	attrset(wc);

	if(options & TW_NOBORDER) {
	    for(i = y1; i < y2; i++) mvhline(i, x1, ' ', x2-x1);
	} else {
	    for(i = y1; i <= y2; i++) mvhline(i, x1, ' ', x2-x1+1);

	    if(!(options & TW_SPACEBORDER)) {
		mvaddch(y1, x1, ULCORNER); mvaddch(y2, x2, LRCORNER);
		mvaddch(y1, x2, URCORNER); mvaddch(y2, x1, LLCORNER);

		mvvline(y1 + 1, x1, VLINE, y2-y1-1);
		mvvline(y1 + 1, x2, VLINE, y2-y1-1);
		mvhline(y1, x1 + 1, HLINE, x2-x1-1);
		mvhline(y2, x1 + 1, HLINE, x2-x1-1);
	    }
	}

	if(!title.empty()) {
	    attrset(tc);
	    mvprintw(y1, x1+((x2-x1)-title.size())/2, "%s", title.c_str());
	}

	refresh();
    }
}

void textwindow::open() {
    if(!fisopen) {
	fisopen = true;
	screenbuffer.save(x1, y1, x2, y2);
	redraw();
    }
}

void textwindow::separatey(int y) {
    attrset(wc);
    mvhline(y1 + y, x1 + 1, HLINE, x2 - x1 - 1);
    mvaddch(y1 + y, x1, LTEE);
    mvaddch(y1 + y, x2, RTEE);
    refresh();
}

void textwindow::separatex(int x) {
    attrset(wc);
    mvvline(y1 + 1, x1 + x, VLINE, y2 - y1 - 1);
    mvaddch(y1, x1 + x, TTEE);
    mvaddch(y2, x1 + x, BTEE);
    refresh();
}

bool textwindow::isbordered() {
    return !(options & TW_NOBORDER);
}

void textwindow::close() {
    abstractuicontrol::close();
}
