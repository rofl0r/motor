#include "uiwatcher.h"
#include "uieditor.h"
#include "motordebugger.h"

uiwatcher watcher;

uiwatcher::uiwatcher() {
    startpos = curpos = 0;
}

uiwatcher::~uiwatcher() {
}

int uiwatcher::keyhandler(verticalmenu &m, int k) {
    vector<string> ws;
    int r;

    m.clearonfocuslost = false;
    r = -1;

    switch(k) {
        case CTRL('d'):
	    watcher.close();
        case ALT('w'):
            r = 0;
            break;
            
        case KEY_IC:
        case CTRL('w'):
	    watcher.add();
            break;

        case KEY_DC:
            ws = debugger.getwatches();
            if(!ws.empty()) {
                debugger.removewatch(*(ws.begin()+m.getpos()));
                watcher.update();
            }
            break;
    }

    m.clearonfocuslost = true;
    return r;
}

void uiwatcher::activate() {
    textwindow w;

    if(!visible()) {
	ed.resizebottom(uiconf.getwatchlines());

        m = verticalmenu(uiconf.getcolor(cp_menufr), uiconf.getcolor(cp_menusel));
        w = textwindow(0, LINES-uiconf.getwatchlines()-1, COLS-1, LINES-2, uiconf.getcolor(cp_menufr));
        w.open();

        m.setwindow(w);
	m.otherkeys = &keyhandler;
        m.clearonfocuslost = true;
	fvisible = true;
    }

    update();
}

void uiwatcher::add() {
    string buf;

    if(ui.input(motorui::text, buf = ed.atcursor(), _("Watch expression: "))
    == motorui::yes) {
	if(!debugger.iswatch(buf)) {
	    debugger.addwatch(buf);
	}

	if(visible()) {
	    activate();
	    update();
	}
    }
}

void uiwatcher::modify(const string &expr, const string &val) {
    string buf;

    if(ui.input(motorui::text, buf = val, _("change the value to: ")) == motorui::yes) {
	debugger.setvar(expr, buf);
    }
}

void uiwatcher::update() {
    string val;
    vector<string> ws;
    vector<string>::iterator iw;

    if(visible()) {
	m.clear();
	ws = debugger.getwatches();

	for(iw = ws.begin(); iw != ws.end(); iw++) {
	    val = debugger.getvar(*iw);
	    m.additem(" " + *iw + ": " + (val.empty() ? _("(not avail)") : val));
	}

	m.redraw();
    }
}

void uiwatcher::exec() {
    int i;
    vector<string> ws;
    vector<string>::iterator iw;

    if(!visible()) activate();

    ui.log(_("Watches: ~Ins~ add, ~Del~ remove, ~^D~ close, ~A-W~ return to editor"));

    while(1) {
        update();
        if(!(i = m.open())) break;

        if(debugger.running() && i) {
	    ws = debugger.getwatches();
	    iw = ws.begin()+i-1;
	    modify(*iw, debugger.getvar(*iw));
        }
    }

    m.getpos(startpos, curpos);
    ui.log();
}

bool uiwatcher::visible() {
    return fvisible;
}

void uiwatcher::close() {
    m.close();
    ed.resizebottom(0);
    fvisible = false;
}
