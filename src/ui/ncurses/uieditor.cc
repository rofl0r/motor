#include "uieditor.h"
#include "motorconfiguration.h"
#include "motormisc.h"
#include "motorproject.h"
#include "motordebugger.h"
#include "kkiproc.h"
#include "uigrepper.h"

#include <regex.h>

uieditor ed;

uieditor::uieditor() {
    otherkeys = &keyhandler;
    idle = &idlefunc;
}

uieditor::~uieditor() {
}

void uieditor::setsyntaxhighlight() {
    int schemenumber = -1;
    vector<uiconfiguration::syntaxitem>::iterator i;

    if(getfid())
    if((i = ::find(syntaxitems.begin(), syntaxitems.end(), getfid())) == syntaxitems.end())
	i = ::find(syntaxitems.begin(), syntaxitems.end(), "");

    setcolorscheme(i->second);
}

void uieditor::loadsettings() {
    readsyntaxhighlight();
    setcoords(0, 1, COLS, LINES-1);
    smarttab = uiconf.getsmarttab();
}

void uieditor::readsyntaxhighlight() {
    ifstream f;
    string buf, sect, sub;
    int i, fg, bg, pairseq = 1, id, colornormal, colorbackground;
    hl_kind h;
    vector<uiconfiguration::syntaxcolorpair> syntaxcolorpairs;
    vector<uiconfiguration::syntaxcolorpair>::iterator iscp;
    vector<uiconfiguration::syntaxitem>::iterator si = syntaxitems.end();

    f.open(conf.getconfigfname("syntax").c_str());

    if(f.is_open()) {
        while(getconf(sect, buf, f)) {
	    if((sect == "pairs") || (sect == ("pairs " + uiconf.getschemename()))) {
                if(pairseq <= 32) {
                    id = atol((sub = getword(buf)).c_str());
                    sub = getword(buf);

                    if(i = sub.find("/")) {
                        fg = findcolor(sub.substr(0, i));
                        bg = findcolor(sub.substr(i+1));
                    }

		    init_pair(pairseq++, fg, bg);

		    syntaxcolorpairs.push_back(
			uiconfiguration::syntaxcolorpair(
			    id, pairseq-1, buf == "bold"));
                }
	    } else if(sect == "lang") {
                sub = getword(buf);

                if(sub == "tag") {

		    syntaxitems.push_back(uiconfiguration::syntaxitem("", 0));
		    si = syntaxitems.end()-1;

                } else if(sub == "mask") {

		    if(si != syntaxitems.end())
			si->first = getword(buf);

                } else if(sub == "all") {

                    colornormal = atol(getword(buf).c_str());
		    colorbackground = atol(getword(buf).c_str());
                    si->second = addscheme(colornormal, colorbackground, false, 0);

                    if(normalcolor(cp_debug_current) != uiconf.getcolor(cp_debug_current))
                        addcolordif(si->second, cp_debug_current);

                    if(normalcolor(cp_debug_breakpoint) != uiconf.getcolor(cp_debug_breakpoint))
                        addcolordif(si->second, cp_debug_breakpoint);

		    for(iscp = syntaxcolorpairs.begin(); iscp != syntaxcolorpairs.end(); iscp++) {
			if(iscp->isbold()) {
			    addcolordif(si->second, iscp->getpairnumber());
			}
		    }

                } else if(si != syntaxitems.end()) {

                    if(sub == "alone") h = h_alone; else
                    if(sub == "eol") h = h_eol; else
                    if(sub == "symbol") h = h_symbol; else
                    if(sub == "quotes") h = h_quotes; else
                    if(sub == "block") h = h_block; else
                    if(sub == "comment") h = h_comment; else
			continue;

                    sub = getrword(buf);
                    addhighlight(si->second, buf.c_str(), atol(sub.c_str()), h);

                }
	    }
	}

	f.close();
    } else {
        init_pair(1, COLOR_WHITE, COLOR_BLUE);
	syntaxitems.push_back(uiconfiguration::syntaxitem("", addscheme(1, 1, false, 0)));
    }
}

int uieditor::keyhandler(texteditor &e, int k) {
    int modifiers = getctrlkeys();

    switch(k) {
	case KEY_F( 2):
	    ed.fsave(savecurrent);
	    break;

	case KEY_F( 3):
	    ed.switchmark();
	    break;

        case KEY_DC:
            if((modifiers & CONTROL_PRESSED) || (modifiers & SHIFT_PRESSED))
        	ed.clipboard(cut);
            break;

        case KEY_IC:
            if(modifiers & SHIFT_PRESSED) ed.clipboard(paste); else
	    if(modifiers & CONTROL_PRESSED) ed.clipboard(copy);
            break;

        case CTRL('d'):
	    ed.closecurrentfile();
	    break;

        case CTRL('u'):
	    ed.undo();
	    break;

        case ALT('x'):
	    ed.clipboard(cut);
	    break;

        case ALT('c'):
	    ed.clipboard(copy);
	    break;

        case ALT('v'):
	    ed.clipboard(paste);
	    break;

        case ALT('l'):
	    ed.gotoline();
	    break;
        
        case ALT(KEY_BACKSPACE):
	    ed.shiftident(-1);
	    ed.redraw();
	    break;

        case ALT(KEY_TAB):
	    ed.shiftident(+1);
	    ed.redraw();
	    break;

	default:
	    thisui.hotkey(k);
	    break;
    }

    ed.updatecursor();
    return thisui.isterminated() ? -1 : 0;
}

void uieditor::switchwindow(int delta) {
    int next = getfnum() + delta;

    if(next >= getfcount()) next = 0; else
    if(next < 0) next = getfcount()-1;

    setfnum(next);

    thisui.statusupdate();
}

void uieditor::clipboard(clipboardaction a) {
    FILE *f;

    if(f = fopen(uiconf.getclipboardfname().c_str(),
    ((a == cut) || (a == copy)) ? "w" : "r")) {
	switch(a) {
    	    case cut:
	        copymark(f);
                delmark();
        	break;
    	    case copy:
	        copymark(f);
		endmark();
        	break;
    	    case paste:
                insert(f);
                clearmark();
        	break;
	}

	fclose(f);
    }
}

bool uieditor::fsave(savemode amode) {
    bool fin, ret = true;
    ofstream f;
    const char *fname;
    int i, savefnum = -1;
    string buf;

    if(!getfcount()) return true;

    switch(amode) {
	case savecurrent:
	    fname = getfid();
	    break;
	case saveas:
	    if(ui.input(motorui::file, buf = getfid(), _("save as: ")) == motorui::yes) {
		fname = buf.c_str();
	    } else {
		return false;
	    }
	    break;
	case saveall:
	case saveallask:
	    savefnum = getfnum();
	    break;
    }

    for(i = 0; ret && (i < getfcount()); i++) {
	switch(amode) {
	    case saveallask:
		setfnum(i);
                switch(asksave()) {
                    case motorui::cancel:
                        ret = false;
                    case motorui::no:
                        continue;
		}
	    case saveall:
		setfnum(i);
		fname = getfid();
                if(!modified) continue;
		break;
	    default:
		i = getfcount();
		break;
	}

	for(fin = false; !fin; ) {
	    f.clear();
	    f.open(fname);

    	    if(fin = f.is_open()) {
        	ui.logf(_("Saving %s .."), justfname(getfid()).c_str());
        	save(f, "\n");
        	f.close();
        	ui.logf(_("File %s saved successfully"), getfid());
    	    } else {
        	fin = ui.askf("YN", "unable to save the file %s. Retry?", getfid())
		    == motorui::no;
    	    }
	}
    }

    setfnum(savefnum);
    return ret;
}

motorui::askresult uieditor::asksave() {
    motorui::askresult ret = motorui::no;

    if(getfcount() && modified) {
        draw();
	ret = ui.askf("YNC", _("file %s was modified. save changes?"), justfname(getfid()).c_str());
    }

    return ret;
}

bool uieditor::closecurrentfile() {
    switch(asksave()) {
	case motorui::cancel:
	    return false;;

	case motorui::yes:
	    fsave(savecurrent);
	default:
	    close();
	    ui.redraw();
	    return true;
    }
}

void uieditor::loadfile() {
    string fname;

    if(ui.input(motorui::file, fname, _("load file: ")) == motorui::yes) {
	ui.loadeditfile(motorui::editfile(fname, 0, 0));
        ui.redraw();
    }
}

void uieditor::idlefunc(texteditor &ref) {
    static int x = -1, y = -1;
    static bool imode, bmode;
    int nx, ny;

    ed.getpos(&nx, &ny);

    if((nx != x) || (ny != y) ||
    (ed.insertmode != imode) ||
    (ed.ismark() != bmode)) {
	thisui.statusupdate();
        x = nx, y = ny, imode = ed.insertmode, bmode = ed.ismark();
    } else {
	dataready(0, 1);
    }
}

void uieditor::gotoline() {
    int n;
    static string sline;

    if(ui.input(motorui::text, sline, _("goto line: ")) == motorui::yes) {
	if(n = atol(sline.c_str())) n--;
	setpos(0, n);
    }
}

void uieditor::markbreakpoints() {
    vector<breakpoint> bps;
    vector<breakpoint>::iterator ibp;
    string fname;
    int i;

    bps = debugger.getbreakpoints();

    for(i = 0; i < getfcount(); i++) {
        fname = getfid(i);
        clearlight(i);

	for(ibp = bps.begin(); ibp != bps.end(); ibp++) {
	    if(ibp->permanent() && (ibp->getfname() == fname))
		if(!(debugger.running() && (ibp->getline() == debugger.getline())))
	    	    highlight(i, ibp->getline(), cp_debug_breakpoint);
	}
    }

    redraw();
}

#define atcuraccept(c) (c && (isalnum(c) || strchr("_", c)))

string uieditor::atcursor() {
    int row, col, i;
    const char *p, *st;
    string ret;
    
    getpos(&col, &row);

    if(p = getline(row))
    if(atcuraccept(p[col])) {
        for(st = p+col; (st != p) && atcuraccept(*(st-1)); st--);
        for(ret = st, i = 0; (i < ret.size()) && atcuraccept(ret[i]); i++);
        ret.resize(i);
    }

    return ret;
}

void uieditor::windowlist() {
    int i, b;
    char *p;
    dialogbox db;
    
    db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT,
        uiconf.getcolor(cp_menufr), TW_CENTERED,
        uiconf.getcolor(cp_menuhl),
        _(" Window list ")));

    db.setbar(new horizontalbar(uiconf.getcolor(cp_menu), uiconf.getcolor(cp_menusel),
        _("Open"), _("Close"), _("Switch to"), 0));

    db.setmenu(new verticalmenu(uiconf.getcolor(cp_menu),
        uiconf.getcolor(cp_menusel)));

    verticalmenu &m = *db.getmenu();

    db.getbar()->item = 2;
    m.setpos(getfnum());
    db.addkey(KEY_IC, 0);
    db.addkey(KEY_DC, 1);

    for(bool fin = false; !fin; ) {    
        m.clear();

        for(i = 0; i < getfcount(); i++)
	    m.additem(0, getfid(i), " " +
		project.transformfname(projectpaths::relative,
		    getfid(i)));

	if(!(fin = !db.open(i, b)))
        switch(b) {
            case 0:
		loadfile();
		db.redraw();
		break;

            case 1:
                setfnum(i-1);
		closecurrentfile();
		db.redraw();
                break;

            case 2:
                if(fin = (p = (char *) m.getref(i-1))) {
		    db.close();
		    ui.loadeditfile(motorui::editfile(p, -1, 0));
		}
                break;
        }
    }

    db.close();
    redraw();
}

void uieditor::resizebottom(int lines) {
    setcoords(0, 1, COLS, LINES-lines-1);
    redraw();
}

bool uieditor::anymodified() {
    int cfile, i;
    bool r;

    r = false;
    cfile = getfnum();
    for(i = 0; (i < getfcount()) && !r; i++) {
        setfnum(i);
	r = modified;
    }

    setfnum(cfile);
    return r;
}

int uieditor::load(const string &buf, const string &id) {
    texteditor::load(buf, id);
    setsyntaxhighlight();
    return 0;
}

int uieditor::load(ifstream &f, const string &id) {
    texteditor::load(f, id);
    setsyntaxhighlight();
    return 0;
}
