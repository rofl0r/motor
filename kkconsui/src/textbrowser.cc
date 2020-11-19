/*
*
* kkconsui textbrowser class
* $Id: textbrowser.cc,v 1.13 2002/11/23 15:42:08 konst Exp $
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

#include "textbrowser.h"

textbrowser::textbrowser(int nx1, int ny1, int nx2, int ny2, int clr) {
    setcoords(nx1, ny1, nx2, ny2);
    setcolor(clr);
    idle = 0;
    otherkeys = 0;
}

textbrowser::textbrowser(int clr) {
    setcolor(clr);
    idle = 0;
    otherkeys = 0;
}

textbrowser::~textbrowser() {
}

void textbrowser::setcolor(int clr) {
    ncolor = clr;
}

void textbrowser::setcoords(int nx1, int ny1, int nx2, int ny2) {
    x1 = nx1;
    x2 = nx2;
    y1 = ny1;
    y2 = ny2;
    setbuf(buf);
}

void textbrowser::redraw() {
    draw(line);
}

bool textbrowser::empty() {
    return lines.empty();
}

void textbrowser::draw(int line) {
    int i, k;
    string buf;
    attrset(ncolor);

    for(i = line; (i-line < y2-y1) && (i < lines.size()); i++) {
	mvprintw(y1+i-line, x1, "");
	buf = makebidi(lines[i], x2-x1);
	printstring(buf);
	for(k = buf.size(); k < x2-x1; k++) printw(" ");
    }

    for(; i-line < y2-y1; i++) mvhline(y1+i-line, x1, ' ', x2-x1);
    refresh();
}

void textbrowser::setbuf(const string &p) {
    line = 0;
    buf = p;

    if(x2-x1 > 1)
	breakintolines(buf = p, lines, x2-x1);

    if((endline = lines.size()-y2+y1+1) < 0) endline = 0;
}

int textbrowser::open() {
    bool go;
    int res, ch;

    finished = aborted = false;
    
    screenbuffer.save(x1, y1, x2, y2);
    draw(line);

    while(!finished) {
	if(idle) go = keypressed(); else go = true;

	if(go) switch(ch = getkey()) {
	    case KEY_UP:
	    case KEY_DOWN:
	    case KEY_PPAGE:
	    case KEY_NPAGE:
	    case KEY_END:
	    case KEY_HOME:
		move(ch);
		draw(line);
		break;
	    case KEY_F(10):
	    case 27:
		res = 0;
		finished = true;
		break;
		
	    default:
		if(otherkeys) {
		    res = (*otherkeys)(*this, ch);
		    if(res >= 0) finished = true;
		}

	} else {
	    if(idle) (*idle)(*this);
	}
    }
    
    return res;
}

void textbrowser::move(int k) {
    switch(k) {
	case KEY_UP: if(--line < 0) line = 0; break;
	case KEY_DOWN: if(++line > endline) line = endline; break;
	case KEY_PPAGE: if((line -= y2-y1) < 0) line = 0; break;
	case KEY_NPAGE: if((line += y2-y1) > endline) line = endline; break;
	case KEY_END: line = endline; break;
	case KEY_HOME: line = 0; break;
    }
}
