/*
*
* kkconsui treeview class
* $Id: treeview.cc,v 1.10 2003/10/19 23:25:05 konst Exp $
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

#include "treeview.h"

treeview::treeview(INT nx1, INT ny1, INT nx2, INT ny2, INT nbgcolor, INT nselectcolor, INT nnodecolor, INT nleafcolor) {
    bgcolor = nbgcolor;
    selectcolor = nselectcolor;
    nodecolor = nnodecolor;
    leafcolor = nleafcolor;
    init();
    setcoords(nx1, ny1, nx2, ny2);
}

treeview::treeview(INT nbgcolor, INT nselectcolor, INT nnodecolor, INT nleafcolor) {
    bgcolor = nbgcolor;
    selectcolor = nselectcolor;
    nodecolor = nnodecolor;
    leafcolor = nleafcolor;
    init();
}

treeview::treeview() {
    init();
}

treeview::~treeview() {
}

void treeview::init() {
    curelem = 0;
    idseq = 0;
    collapsable = false;

    menu = verticalmenu(x1, y1, x2, y2, bgcolor, selectcolor);
    clear();
}

INT treeview::addnodef(INT parent, INT color, void *ref, const char *fmt, ...) {
    string buf;
    VGETSTRING(buf, fmt);
    return addnode(parent, color, ref, buf);
}

INT treeview::addnodef(const char *fmt, ...) {
    string buf;
    VGETSTRING(buf, fmt);
    return addnode(buf);
}

INT treeview::addleaff(INT parent, INT color, void *ref, const char *fmt, ...) {
    string buf;
    VGETSTRING(buf, fmt);
    return addleaf(parent, color, ref, buf);
}

INT treeview::addleaff(const char *fmt, ...) {
    string buf;
    VGETSTRING(buf, fmt);
    return addleaf(buf);
}

INT treeview::addnodef(INT parent, INT color, INT ref, const char *fmt, ...) {
    string buf;
    VGETSTRING(buf, fmt);
    return addnode(parent, color, (void *) ref, buf);
}

INT treeview::addleaff(INT parent, INT color, INT ref, const char *fmt, ...) {
    string buf;
    VGETSTRING(buf, fmt);
    return addleaf(parent, color, (void *) ref, buf);
}

INT treeview::addnode(const string &text) {
    return addnode(0, 0, NULL, text);
}

INT treeview::addnode(INT parent, INT color, void *ref, const string &text) {
    treeviewnode node;

    node.id = idseq++;
    node.text = text;
    node.ref = ref;
    node.parentid = parent;
    node.isnode = true;
    node.isopen = false;
    node.color = color ? color : nodecolor;

    items.push_back(node);
    return node.id;
}

INT treeview::addleaf(const string &text) {
    return addleaf(0, 0, NULL, text);
}

INT treeview::addleaf(INT parent, INT color, void *ref, const string &text) {
    treeviewnode node;

    node.id = idseq++;
    node.text = text;
    node.ref = ref;
    node.parentid = parent;
    node.isnode = false;
    node.color = color ? color : leafcolor;

    items.push_back(node);
    return node.id;
}

INT treeview::addnode(INT parent, INT color, INT ref, const string &text) {
    return addnode(parent, color, (void *) ref, text);
}

INT treeview::addleaf(INT parent, INT color, INT ref, const string &text) {
    return addleaf(parent, color, (void *) ref, text);
}

INT treeview::getid(void *ref) {
    INT id = -1;
    vector<treeviewnode>::iterator i;

    if((i = find(items.begin(), items.end(), (void *) ref)) != items.end())
	id = i->id;

    return id;
}

INT treeview::getid(INT mpos) {
    if((mpos >= 0) && (mpos < refdeps.size())) {
	return refdeps[mpos].id;
    } else {
	return -1;
    }
}

void treeview::opennode(INT id) {
    vector<treeviewnode>::iterator i;
    i = find(items.begin(), items.end(), id);

    if(i != items.end()) {
	if(i->isnode && collapsable) i->isopen = true;
	genmenu(0);
    }
}

void treeview::closenode(INT id) {
    vector<treeviewnode>::iterator i;
    i = find(items.begin(), items.end(), id);

    if(i != items.end()) {
	if(i->isnode && collapsable) i->isopen = false;
	genmenu(0);
    }
}

void treeview::clear() {
    treeviewnode node;

    items.clear();
    idseq = 1;

    node.id = 0;
    node.ref = 0;
    node.parentid = -1;
    node.isnode = true;

    items.push_back(node);
}

bool treeview::islast(INT id) {
    INT lastid, nid;
    vector<treeviewnode>::iterator i;

    if((i = find(items.begin(), items.end(), id)) != items.end())
    if((i = find(items.begin(), items.end(), i->parentid)) != items.end()) {
	nid = i->id;
	for(i = items.begin(); i != items.end(); i++)
	if(i->parentid == nid) lastid = i->id;
    }

    return lastid == id;
}

void treeview::genmenu(INT parent) {
    INT nproc = 0;
    vector<treeviewnode>::iterator i, k;

    if(!parent) {
	nestlevel.clear();
	refdeps.clear();
	menu.clear();
    } else {
	if((i = find(items.begin(), items.end(), parent)) != items.end())
	    nestlevel.push_back(*i);
    }

    for(i = items.begin(); i != items.end(); i++) {
	if(i->parentid == parent) {
	    string buf;

	    for(k = nestlevel.begin(); k != nestlevel.end(); k++)
		buf += !islast(k->id) ? "\003  " : "   ";

	    buf += islast(i->id) ? "\007\002" : "\005\002";
	    buf.insert(0, "\001");
	    buf += "\001";

	    if(i->isnode && collapsable) {
		buf += (string) "[" + (i->isopen ? '-' : '+') + "]";
	    }

	    buf += i->text + "\001";
	    nproc++;

	    menu.additem(i->color, NULL, buf);
	    refdeps.push_back(*i);

	    if(i->isnode) if(!collapsable || (collapsable && i->isopen))
		genmenu(i->id);
	}
    }

    if(parent != -1)
    if(!nestlevel.empty()) nestlevel.pop_back();
}

void treeview::redraw() {
    genmenu(0);
    menu.redraw();
}

void *treeview::open(INT *n) {
    void *p = 0;
    INT k;

    screenbuffer.save(x1, y1, x2, y2);
    redraw();

    if(k = menu.open()) {
	p = getref(getid(k-1));
    }

    if(n) *n = k;
    return p;
}

bool treeview::isnodeopen(INT id) {
    vector<treeviewnode>::iterator i;

    if((i = find(items.begin(), items.end(), id)) != items.end()) {
	return i->isopen;
    } else {
	return true;
    }
}

bool treeview::isnode(INT id) {
    vector<treeviewnode>::iterator i;

    if((i = find(items.begin(), items.end(), id)) != items.end()) {
	return i->isnode;
    } else {
	return false;
    }
}

INT treeview::getparent(INT id) {
    INT ret = -1;
    vector<treeviewnode>::iterator i;

    if((i = find(items.begin(), items.end(), id)) != items.end())
    if((i = find(items.begin(), items.end(), i->parentid)) != items.end()) {
	ret = i->id;
    }

    return ret;
}

void *treeview::getref(INT id) {
    vector<treeviewnode>::iterator i;

    if((i = find(items.begin(), items.end(), id)) != items.end()) {
	return i->ref;
    } else {
	return 0;
    }
}

void treeview::setcur(INT id) {
    vector<treeviewnode>::iterator i;

    if(!menu.getcount()) genmenu(0);

    if((i = find(refdeps.begin(), refdeps.end(), id)) != refdeps.end()) {
	menu.setpos(i-refdeps.begin());
    }
}

INT treeview::getcount() {
    return items.size();
}

bool treeview::empty() {
    return items.size() < 2;
}

void treeview::setcoords(INT nx1, INT ny1, INT nx2, INT ny2) {
    menu.setcoords(x1 = nx1, y1 = ny1, x2 = nx2, y2 = ny2);
}

// ----------------------------------------------------------------------------

bool treeview::treeviewnode::operator == (const INT aid) {
    return id == aid;
}

bool treeview::treeviewnode::operator == (const void *aref) {
    return ref == aref;
}

bool treeview::treeviewnode::operator != (const INT aid) {
    return id != aid;
}

bool treeview::treeviewnode::operator != (const void *aref) {
    return ref != aref;
}
