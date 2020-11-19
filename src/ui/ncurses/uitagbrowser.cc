#include "uitagbrowser.h"
#include "uieditor.h"

uitagbrowser uitb;

static string getscopename(motortagbrowser::viewscope nscope) {
    switch(nscope) {
	case motortagbrowser::File:
	    return _("Current file");
	case motortagbrowser::Project:
	    return _("Current project");
	case motortagbrowser::Everything:
	    return _("Everything");
    }

    return "";
}

uitagbrowser::uitagbrowser(): mpos(0) {
}

uitagbrowser::~uitagbrowser() {
}

int uitagbrowser::mkeys(verticalmenu &m, int k) {
    string oldsearch = uitb.search;

    switch(k) {
        case KEY_BACKSPACE:
	    if(!uitb.search.empty()) {
                uitb.modified = true;
		uitb.search = oldsearch.substr(0, oldsearch.size()-1);
	    }
            break;

        case KEY_DC:
	    uitb.search = ed.atcursor() != uitb.search ? ed.atcursor() : "";
	    break;

	case KEY_F(2):
	    uitb.changemode();
	    break;

	case KEY_F(3):
	    uitb.changefilter();
	    break;

	case CTRL('r'):
	    uitb.reload();
	    break;

        default:
            if(isprint(k)) {
                if(!uitb.modified) {
                    uitb.search = "";
                    uitb.modified = true;
                }
                
                uitb.search += k;
            }
            break;
    }

    if(oldsearch != uitb.search) {
        if((uitb.search.size() > oldsearch.size()) && !m.getcount()) {
            uitb.input();
        } else {
            uitb.makemenu(m);
        }
    }

    return -1;
}

void uitagbrowser::title() {
    w.set_titlef(uiconf.getcolor(cp_menuhl), _(" Symbols [%s] "),
	getscopename(tagbrowser.getscope()).c_str());
}

void uitagbrowser::changemode() {
    int n;
    verticalmenu cm(uiconf.getcolor(cp_menufr), uiconf.getcolor(cp_menusel));
    cm.setwindow(textwindow(w.x1, w.y1, w.x1+20, 0, uiconf.getcolor(cp_menufr)));

    for(motortagbrowser::viewscope scope = motortagbrowser::File;
    scope != motortagbrowser::viewscope_size; scope = (motortagbrowser::viewscope) ( scope + 1 ) ) {
	cm.additem(0, scope, (string) " " + getscopename(scope));
	if(scope == tagbrowser.getscope())
	    cm.setpos(cm.getcount()-1);

    }

    cm.scale();
    n = cm.open();
    cm.close();

    if(n) {
	int c = (intptr_t) cm.getref(n-1);

	if(c != tagbrowser.getscope()) {
	    tagbrowser.setscope((motortagbrowser::viewscope) c);
	    reload();
	}
    }
}

int uitagbrowser::multiplekeys(verticalmenu &m, int k) {
    switch(k) {
	case ' ':
	case 'x':
	case 'X':
	    return -2;
    }
    return -1;
}

void uitagbrowser::changefilter() {
    bool finished = false;
    int saveelem, pos;
    char c;
    bool iter;
    string nfilter = filter;
    vector<pair<char, string > >::const_iterator ik;

    verticalmenu cm(uiconf.getcolor(cp_menufr), uiconf.getcolor(cp_menusel));
    cm.setwindow(textwindow(w.x1, w.y1, w.x1+20, 0, uiconf.getcolor(cp_menufr)));

    cm.otherkeys = &multiplekeys;

    while(!finished) {
	saveelem = cm.getpos();
	cm.clear();

	for(ik = project.tagnamebegin(); ik != project.tagnameend(); ++ik) {
	    cm.additemf(0, ik->first, "[%c] %s", (nfilter.find(ik->first) != -1) ? 'x' : ' ',
		ik->second.c_str());
	}

	cm.scale();
	cm.setpos(saveelem);

	switch(cm.open()) {
	    case -2:
		iter = false;
		c = (char) (intptr_t) cm.getref(cm.getpos());

		while((pos = nfilter.find(c)) != -1) {
		    nfilter.erase(pos, 1);
		    iter = true;
		}

		if(!iter)
		    nfilter += c;
		break;

	    default:
		if(!nfilter.empty())
		    finished = true;
		break;
	}
    }

    cm.close();

    if((cm.getlastkey() != KEY_ESC) && (filter != nfilter)) {
	filter = nfilter;
	makemenu(m);
    }
}

void uitagbrowser::reload() {
    ui.autosave();
    tagbrowser.clear();
    makemenu(m);
    title();
}

void uitagbrowser::makemenu(verticalmenu &m) {
    int len;
    string buf, desc, skind;
    vector<motortagbrowser::record>::iterator it;

    tags = tagbrowser.fetch(search, filter);
    m.clear();

    for(it = tags.begin(); it != tags.end(); it++) {
        buf = " " + it->getname();
	desc = it->getbrowsertagname();

        if((skind = it->getinfo()).size() && (skind[skind.size()-1] != ':')) {
            if(desc.size()) desc += ", ";
            desc += skind;
        }

        if((len = w.x2-w.x1-buf.size()-desc.size()-2) < 2) {
            len = 2;
            buf.resize(w.x2-w.x1-buf.size()-4);
        }

        buf += skind.assign(len, ' ') + desc;
        m.additem(0, it-tags.begin(), buf);
    }

    ui.log(_("Symbols: type to find, ~Del~ clear, ~F2~ scope, ~F3~ filter, ~^R~ reload, ~Enter~ source"));
    m.redraw();
    input();
}

void uitagbrowser::input() {
    attrset(uiconf.getcolor(cp_menufr));
    mvhline(w.y2-1, w.x1+2, ' ', w.x2-w.x1-2);
    w.write(2, w.y2-w.y1-1, uiconf.getcolor(cp_menuhl), search);
}

void uitagbrowser::exec() {
    int n, k;
    bool r;

    if(search.empty())
	search = ed.atcursor();

    tags = tagbrowser.fetch(search);
    if(!tagbrowser.ok()) {
	return;
    }

    w = textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT, uiconf.getcolor(cp_menufr), TW_CENTERED);

    title();
    w.open();
    w.separatey(w.y2-w.y1-2);

    m = verticalmenu(w.x1+1, w.y1+1, w.x2, w.y2-2, uiconf.getcolor(cp_menufr),
	uiconf.getcolor(cp_menusel));

    m.otherkeys = &mkeys;
    modified = false;

    makemenu(m);
    m.setpos(mpos);

    if(r = (n = m.open())) {
        k = (intptr_t) m.getref(n-1);
        mpos = n-1;
    }

    w.close();

    if(r)
    if(!tags.empty())
	ui.loadeditfile((tags.begin()+k)->geteditfile());

    ui.log();
}
