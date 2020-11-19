/*
*
* kkconsui common routines
* $Id: conscommon.cc,v 1.27 2004/06/21 13:04:31 konst Exp $
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

#include "conscommon.h"
#include <iostream>

bool kintf_graph = true, kintf_refresh = true;

#ifdef KTOOL_USE_FRIBIDI
bool use_fribidi = true;
#endif

void (*kt_resize_event)(void);

void kreinit(int sn) {
    struct winsize size;

    if(ioctl(fileno(stdout), TIOCGWINSZ, &size) == 0) {
	resizeterm(size.ws_row, size.ws_col);
	wrefresh(curscr);

	if(kt_resize_event)
	    kt_resize_event();
    }

    signal(sn, &kreinit);
}

void kinterface() {
#ifdef KTOOL_USE_FRIBIDI
    // an experimental thingie
    std::cout << ((char) 27) << "(K" << std::flush;
#endif

    initscr();
    nonl();
    cbreak();
    noecho();
    keypad(stdscr, 1);
//    raw();
    start_color();
    use_default_colors();
    atexit(kendinterface);
//      nodelay(stdscr, TRUE);
    ESCDELAY = 0;
    signal(SIGWINCH, &kreinit);
    kt_resize_event = 0;
}

void kendinterface() {
    endwin();
}

int getkey() {
    int k, n, fin = 0, r = 0;
    fd_set rd;
    struct timeval tv;

    refresh();

    while(!fin) {
	FD_ZERO(&rd);
	FD_SET(STDIN_FILENO, &rd);
	while(!select(STDIN_FILENO+1, &rd, 0, 0, 0));
	ioctl(STDIN_FILENO, FIONREAD, &r);

	switch(k = getch()) {
	    case 27:
		tv.tv_sec = 10;
		tv.tv_usec = 0;

		if(r <= 1) r = select(STDIN_FILENO+1, &rd, 0, 0, &tv);

		if(r) {
		    n = getch();
		    if(isdigit(n)) k = KEY_F(n-'0' ? n-'0' : 10); else
		    if(n != 27) k = ALT(n);
		} else {
		    k = 0;
		}

		fin = 1;
		break;

	    case CTRL('l'):
		endwin();
		refresh();
		fin = 1;
		break;

	    default:
		fin = 1;
		break;
	}
    }

    return k;
}

int getctrlkeys() {
#ifdef __linux__
    unsigned char modifiers = 6;
    if(ioctl(0, TIOCLINUX, &modifiers) < 0) return 0;
    return (int) modifiers;
#else
    return 0;
#endif
}

int emacsbind(int k) {
    switch(k) {
	case CTRL('b'): return KEY_LEFT;
	case CTRL('f'): return KEY_RIGHT;
	case CTRL('n'): return KEY_DOWN;
	case CTRL('p'): return KEY_UP;
	case CTRL('d'): return KEY_DC;
	case CTRL('a'): return KEY_HOME;
	case CTRL('e'): return KEY_END;
	case CTRL('v'): return KEY_NPAGE;
	case CTRL('k'): return CTRL('y');
	default: return k;
    }
}

void printchar(char c) {
    printw("%c", KT_DISP_FILTER(c));
}

void printstring(const string &s) {
    int i;
    string ds;

    for(i = 0; i < s.size(); i++)
	ds += KT_DISP_FILTER(s[i]);

    printw("%s", ds.c_str());
}

int kwherex() {
    return getcurx(stdscr);
}

int kwherey() {
    return getcury(stdscr);
}

void kwriteatf(int x, int y, int c, const char *fmt, ...) {
    char buf[65535];
    va_list ap;
    
    va_start(ap, fmt);    
    attrset(c);
    vsprintf(buf, fmt, ap);
    mvprintw(y, x, "");
    printstring(buf);
    refresh();
}

void kwriteat(int x, int y, const char *msg, int c) {
    attrset(c);
    mvprintw(y, x, "");
    printstring(msg);
    refresh();
}

int keypressed(bool wait) {
    struct timeval tv;
    fd_set readfds;
    tv.tv_sec = tv.tv_usec = 0;

    FD_ZERO(&readfds);
    FD_SET(0, &readfds);
    select(1, &readfds, 0, 0, wait ? 0 : &tv);

    return FD_ISSET(0, &readfds);
}

void kgotoxy(int x, int y) {
    move(y, x);
}

void setbeep(int freq, int duration) {
    // freq: 21-32766
    // duration: 0-2000
    // freq or duration == -2  ==>  set the default value

    if(freq >= 21) printf("\e[10;%d]", freq); else
    if(freq == -2) printf("\e[10]");

    if(duration >= 0) printf("\e[11;%d]", duration); else
    if(duration == -2) printf("\e[11]");

    fflush(stdout);
}

int string2key(const string &adef) {
    int r;
    string d, ck;
    string::iterator ic;

    r = 0;
    d = adef;
    if((ck = d).find("-") != -1) ck = getrword(d, "-");

    if(!ck.empty()) {
	if((ck.size() > 1) && (toupper(ck[0]) == 'F')) {
	    ck.erase(0, 1);
	    r = atol(ck.c_str());
	} else {
	    r = ck[0];
	}
    }

    if(!d.empty() && r) {
	for(ic = d.begin(); ic != d.end(); *ic = toupper(*ic), ic++);

	if(d == "CTRL") r = CTRL(r); else
	if(d == "ALT") r = ALT(r); else
	    r = 0;
    }

    return r ? r : -1;
}

#ifdef KTOOL_USE_FRIBIDI

extern "C" {
#include <fribidi/fribidi.h>
}

string makebidi(const string &buf, int lpad) {
    if(!use_fribidi) return buf;

    FriBidiChar *us, *out_us;
    char *outstring, *cbuf;
    int size;
    FriBidiCharType base;
    string r, pad;

    size = buf.size()+1;

    us = new FriBidiChar[size];
    out_us = new FriBidiChar[size];
    outstring = new char[size];
    cbuf = strdup(buf.c_str());
	// really sick thing

    base = FRIBIDI_TYPE_N;
    fribidi_iso8859_8_to_unicode(cbuf, buf.size(), us);
    fribidi_log2vis(us, buf.size(), &base, out_us, 0, 0, 0);
    fribidi_unicode_to_iso8859_8(out_us, buf.size(), outstring);

    r = outstring;

    delete us;
    delete out_us;
    delete outstring;
    delete cbuf;

    if(lpad) {
	pad.assign(lpad-r.size(), ' ');
	r.insert(0, pad);
    }

    return r;
}

#else

string makebidi(const string &buf, int lpad) {
    return buf;
}

#endif

int findcolor(const string &s) {
    int i;
    string::iterator is;

    string colors[] = {"TRANSPARENT", "BLACK", "RED", "GREEN",
	"YELLOW", "BLUE", "MAGENTA", "CYAN", "WHITE", ""};

    string ls = s;

    for(is = ls.begin(); is != ls.end(); *is = toupper(*is), is++);
    for(i = 0; !colors[i].empty() && (ls != colors[i]); i++);

    return i-1;
}
