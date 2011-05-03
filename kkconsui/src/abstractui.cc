/*
*
* kkconsui abstract ui control class
* $Id: abstractui.cc,v 1.7 2003/11/05 09:10:27 konst Exp $
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

#include "abstractui.h"

abstractuicontrol::abstractuicontrol()
: fisopen(false), aborted(false), emacs(false), finished(false), x1(0), x2(0), y1(0), y2(0)
{ }

abstractuicontrol::abstractuicontrol(const abstractuicontrol &a)
: fisopen(a.fisopen), finished(a.finished), emacs(a.emacs),
  aborted(a.aborted), x1(a.x1), x2(a.x2), y1(a.y1), y2(a.y2)
{ }

abstractuicontrol::~abstractuicontrol()
{ }

bool abstractuicontrol::empty() const {
    return !x1 && !x2 && !y1 && !y2;
}

void abstractuicontrol::redraw() {
}

void abstractuicontrol::close() {
    screenbuffer.restore();
    fisopen = false;
}

bool abstractuicontrol::isopen() const { return fisopen; }
bool abstractuicontrol::getaborted() const { return aborted; };

void abstractuicontrol::abort() {
    finished = aborted = true;
}
