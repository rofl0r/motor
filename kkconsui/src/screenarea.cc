/*
*
* a class for saving and restoring screen areas
* $Id: screenarea.cc,v 1.1 2001/06/27 13:42:07 konst Exp $
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

#include "screenarea.h"

screenarea::screenarea() {
}

screenarea::screenarea(int fx1, int fy1, int fx2, int fy2) {
    save(fx1, fy1, fx2, fy2);
}

screenarea::~screenarea() {
    freebuffer();
}

void screenarea::save() {
    save(0, 0, COLS, LINES);
}

void screenarea::save(int fx1, int fy1, int fx2, int fy2) {
    int i;
    chtype *line;

    freebuffer();

    for(i = 0; i <= fy2-fy1; i++) {
	line = new chtype[fx2-fx1+2];
	mvinchnstr(fy1+i, fx1, line, fx2-fx1+1);
	buffer.push_back(line);
    }

    x1 = fx1;
    y1 = fy1;
    x2 = fx2;
    y2 = fy2;
}

void screenarea::restore() {
    restore(x1, y1, x2, y2);
}

void screenarea::restore(int fx1, int fy1, int fx2, int fy2) {
    vector<chtype *>::iterator i;
    int k = fy1;
    chtype *line;

    if(!buffer.empty()) {
	for(i = buffer.begin(); i != buffer.end(); i++) {
	    line = *i;
	    mvaddchnstr(k++, fx1, line, fx2-fx1+1);
	}

	refresh();
    }

    freebuffer();
}

void screenarea::freebuffer() {
    while(!buffer.empty()) {
	delete *buffer.begin();
	buffer.erase(buffer.begin());
    }
}

bool screenarea::empty() {
    return buffer.empty();
}
