#include "ncursesui.h"
#include "motormanager.h"
#include "motordebugger.h"
#include "motorconfiguration.h"
#include "motormisc.h"
#include "motorvcs.h"

#include "uieditor.h"
#include "uigrepper.h"
#include "uiwatcher.h"
#include "uiregexper.h"
#include "uivcs.h"

#include "kkfsys.h"

#include <strstream>
#include <iostream>

#include <sys/wait.h>

ncursesui thisui;

#define LOOKER_HEIGHT		(LINES-2)
#define OUTLINES_LIMIT		512
#define NULL_CONST_STR		((const char *) 0)

#define getcolor(c)	uiconf.getcolor(c)

ncursesui::ncursesui(): terminate(false), outopen(false) {
}

ncursesui::~ncursesui() {
    if(terminate) {
	kendinterface();
	for(int i = 0; i < LINES; i++) cout << endl;
    }
}

void ncursesui::sighandler(int signum) {
    switch(signum) {
	case SIGCHLD:
	    while(wait3(0, WNOHANG, 0) > 0);
	    break;
    }

    signal(SIGCHLD, &sighandler);
    signal(SIGINT, &sighandler);
}

void ncursesui::execute(int argc, char **argv) {
    string pname;

    kinterface();
    uiconf.load();
    ed.loadsettings();

    sighandler(0);

    commandline(argc, argv);
    outwindow = textwindow(0, 1, COLS, LOOKER_HEIGHT, normalcolor(0), TW_SPACEBORDER);
    workareaupdate();
    initelements();
    log();

    if(project.empty()) pload();

    if(!project.empty()) {
        initmenubar();

	do {
	    mainloop();
            terminate = project.close();
	} while(!terminate);
    }

    uiconf.save();
}

void ncursesui::pload(const projectname pname) {
    projectname nname = pname;
    motorproject np;
    bool fin;

    for(fin = false; !fin; ) {
	if(nname.empty()) {
	    nname = selectproject(selectorcreate);
	}

        if(!(fin = nname.empty())) {
            np = motorproject(nname);

            if(!np.empty())
               if(project.close()) {
                   project = motorproject(static_cast<projectname> (np));
                   onprojectload();
                   fin = true;
               }
        }

	nname = projectname();
    }

    terminate = project.empty();
}

motorui::askresult ncursesui::ask(const string &answersallowed, const string &text) {
    int line = /*watcher.visible() ? LINES-uiconf.getwatchlines()-2 :*/ LINES-2;
    screenarea sarea(0, line, COLS, line);
    string msg;
    string::const_iterator c;
    int key;

    attrset(getcolor(cp_input));
    mvhline(line, 0, ' ', COLS);

    msg = text + " (";
    for(c = answersallowed.begin(); c != answersallowed.end(); c++) {
        msg += c != answersallowed.begin() ? tolower(*c) : toupper(*c);
        if(c != answersallowed.end()-1) msg += '/';
    }

    msg += ") ";
    kwriteat(0, line, msg.c_str(), getcolor(cp_input));

    while(1) {
        key = toupper(getch());
        if(key == '\r') key = toupper(*answersallowed.begin());
        if(answersallowed.find(key) != -1) break;
    }

    sarea.restore();

    switch(key) {
        case 'Y': return motorui::yes;
        case 'C': return motorui::cancel;
        case 'A': return motorui::all;
        case 'N': return motorui::no;
    }

    return motorui::no;
}

void ncursesui::log(const string &text) {
    bool hl = false;
    string::const_iterator i;

    attrset(getcolor(cp_bottom));
    mvhline(LINES-1, 0, ' ', COLS);
    mvprintw(LINES-1, 0, "");

    for(i = text.begin(); i != text.end(); i++) {
	if(*i == '~') {
	    if(text.substr(i-text.begin(), 2) != "~~") {
		if(hl = !hl) {
		    attrset(getcolor(cp_bottomhl));
		} else {
		    attrset(getcolor(cp_bottom));
		}
	    } else {
		printw("~");
		i++;
	    }
	} else {
	    printw("%c", *i);
	}
    }

    refresh();
}

void ncursesui::loadeditfile(const motorui::editfile ef, int options) {
    ifstream f;
    int i;
    char buf[512];
    string fname;
    bool found;
    vector<motorfolder>::iterator ifold, icfold;
    vector<motorfile>::iterator ifile, icfile;
    motorproject pf;

    if(ef.fname.empty()) return;

    if((fname = ef.fname).substr(0, 1) != "/") {
	fname = (string) getcwd(buf, 512) + "/" + fname;
    }

    if(access(fname.c_str(), F_OK)) {
	fname = justfname(fname);

	for(ifold = project.foldbegin(), found = false; !found && ifold != project.foldend(); ifold++)
    	for(ifile = ifold->begin(); !found && ifile != ifold->end(); ifile++)
	    switch(ifold->getcontentkind()) {
		case motorfile::source:
		    if(found = (justfname(ifile->getfname()) == fname))
		        fname = project.transformfname(projectpaths::absolute, ifile->getfname());
		    break;

		case motorfile::project:
		    pf = motorproject(ifile->getfname());

		    for(icfold = pf.foldbegin(); !found && icfold != pf.foldend(); icfold++) {
		        if(icfold->getcontentkind() == motorfile::source) {
			    for(icfile = icfold->begin(); !found && icfile != icfold->end(); icfile++) {
			        if(found = (justfname(icfile->getfname()) == fname))
				    fname = pf.transformfname(projectpaths::absolute,
				        icfile->getfname());
			    }
			}
		    }
		    break;
	    }
    }

    for(i = 0, found = false; i < ed.getfcount() && !found; i++) {
	found = samefile(ed.getfid(i), fname);
    }

    if(found) {
	ed.setfnum(i-1);
    } else {
	if((options & LF_CREATE_IF_NOTFOUND) && access(fname.c_str(), F_OK)) {
	    ed.load("", fname);
	} else {
	    f.open(fname.c_str());
	    if(f.is_open()) {
	        ed.load(f, fname);
		ed.markbreakpoints();
		f.close();
	    }
	}
    }

    if(ef.x != -1 && ef.y != -1)
        ed.setpos(ef.x, ef.y-1);

    statusupdate();
}

void ncursesui::commandline(int argc, char **argv) {
    int r;
    string slong;

    for(r = 1; argv[r]; r++) {
	slong = argv[r];

	if(slong == "--ascii" || slong == "-a") {
	    kintf_graph = 0;

	} else if((slong == "--debugtty" || slong == "-t") && argv[++r]) {
	    debugger.forceusetty(argv[r]);

	} else if(slong == "--help" || slong.substr(0, 1) == "-") {
	    usage();
	    exit(0);

	} else if(project.empty()) {
	    project = motorproject(projectname(slong));
	    onprojectload();

	}
    }
}

void ncursesui::usage() {
    kendinterface();
    cout << "usage: motor [options] [project name]" << endl <<
    "options are as follows:" << endl <<
    "\t-a, --ascii\tuse ascii characters to draw lines" << endl <<
    "\t-t, --debugtty\tuse the specified tty for debugging" << endl <<
    "\t-h, --help\tdisplay this help screen" << endl;
}

bool ncursesui::regproject() {
    bool r = false;
    string fname, pname, lfname, postfix = ".motor";
    char buf[512];

    if(input(motorui::file, fname, _("add project to the registry: ")) == motorui::yes) {
	if(!access(fname.c_str(), R_OK)) {
	    pname = justfname(fname);

	    if(pname.size() > postfix.size())
	    if(pname.substr(pname.size()-postfix.size()) == postfix)
		pname.erase(pname.size()-postfix.size());

	    if(pname != justfname(fname)) {
		bool updname = false;

		while(!pname.empty() && !projectname(pname).empty()) {
		    updname = true;
		    sprintf(buf, _("Already registered! Rename %s (%s) to: "),
			pname.c_str(), justfname(fname).c_str());
		    if(input(text, pname, buf) != motorui::yes)
			pname = "";
		}

		if(!pname.empty()) {
		    if(updname) {
			string nfname = justpathname(fname) + "/" + pname + postfix;
			rename(fname.c_str(), nfname.c_str());
			fname = nfname;
		    }

		    lfname = conf.getprojectdir() + pname;
		    symlink(fname.c_str(), lfname.c_str());

		    motorproject np(pname);
		    r = !np.empty();
		    if(!r) {
			logf(_("~%s~ is not a motor project"), justfname(fname).c_str());
			unlink(lfname.c_str());
		    }
		}
	    } else {
		logf(_("~%s~ is an invalid name for a motor project file"));
	    }

	} else {
	    logf(_("Cannot access ~%s~"), justfname(fname).c_str());
	}
    }

    return r;
}

const projectname ncursesui::selectproject(mgrmode amode, vector<string> templs) {
    vector<projectname>::iterator i;
    vector<projectname> pnames;
    vector<string>::iterator is;
    vector< pair<string, int> >::iterator itnode;
    vector< pair<string, int> > tnodes;

    dialogbox db;
    int j, baritem, menuitem, cid, lpid;
    string templatelast, head;
    projectname pname;
    motorproject mp;
    bool iterfirst = true;

    if(amode == selectorcreate) {
	db.setbar(new horizontalbar(getcolor(cp_menu), getcolor(cp_menusel),
    	    _("Create/Import.."), _("Add"), _("Remove"), _("Open"), NULL_CONST_STR));

	db.addkey(KEY_IC, 1);
	db.addkey(KEY_DC, 2);

        db.getbar()->item = 3;

	db.setwindow(new textwindow(0, 0, (int) (DIALOG_WIDTH*0.8),
    	    DIALOG_HEIGHT, getcolor(cp_menufr), TW_CENTERED,
    	    getcolor(cp_menuhl), _(" Projects registry ")));

    } else {
	db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT,
	    getcolor(cp_menufr), TW_CENTERED, getcolor(cp_menuhl),
	    _(" Add a project ")));
    }

    db.settree(new treeview(getcolor(cp_menu), getcolor(cp_menusel),
	getcolor(cp_menu), getcolor(cp_menu)));

    treeview &tree = *db.gettree();

    while(pname.empty()) {
	if(tree.empty()) {
    	    tree.clear();
	    tnodes.clear();
	    pnames = manager.getprojectlist();

	    for(i = pnames.begin(); i != pnames.end(); i++) {
		for(itnode = tnodes.begin(); itnode != tnodes.end(); itnode++)
		if(itnode->first == i->gettemplatename()) break;

		if(!templs.empty())
            	    if(find(templs.begin(), templs.end(), i->gettemplatename()) == templs.end())
                	continue;

		if(itnode == tnodes.end()) {
		    j = tree.addnode(0, getcolor(cp_menuhl), 0, " " + i->gettemplatename() + " ");
		    tnodes.push_back(pair<string, int>(i->gettemplatename(), j));
		    itnode = tnodes.end()-1;
		}

		cid = tree.addleaf(itnode->second, 0,
		    i-pnames.begin()+1, " " + i->getname() + " ");

                if(iterfirst)
		if((project.empty() && (i->getname() == uiconf.getlastproject()))
		|| (i->getname() == project.getname())) {
            	    lpid = cid;
		}
	    }

            if(iterfirst) tree.setcur(lpid);
	}

/*        if(tree.empty() && (amode == selectorcreate)) {
            if(iterfirst) baritem = 0; else break;
        } else {*/
            if(!db.open(menuitem, baritem, (void **) &j)) break;
//        }

        iterfirst = false;

	if((amode == selectonly) && j) {
	    pname = pnames[j-1];
	} else
	switch(baritem) {
	    case 0:
		if(createproject()) {
                    pname = static_cast<projectname> (project);
		}
	        break;
	    case 1:
		if(regproject())
		    tree.clear();
		break;
	    case 2:
		if(j) {
		    mp = motorproject(pnames[j-1], LP_NOCHECK);

		    if(!mp.empty()) {
			mp.remove();
			tree.clear();
                        iterfirst = pnames.size() == 1;
		    }
		}
	        break;
	    case 3:
		if(j) {
		    pname = pnames[j-1];
		}
	        break;
	}
    }

    db.close();
    return pname;
}

string ncursesui::selecttemplate(const string &def, const string &title) const {
    dialogbox db;
    string ret, lw, buf;
    int n, b, nnode, citem, id, cpid = -1;
    vector<string> tlist;
    vector<string>::iterator it;
    vector< pair<string, int> > nodes;
    vector< pair<string, int> >::iterator in;

    tlist = manager.gettemplatelist();

    db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT,
	getcolor(cp_menufr), TW_CENTERED, getcolor(cp_menuhl), title.c_str()));
    db.settree(new treeview(getcolor(cp_menu), getcolor(cp_menusel),
	getcolor(cp_menu), getcolor(cp_menu)));

    for(it = tlist.begin(); it != tlist.end(); it++) {
	lw = getrword(buf = *it, "/");

	for(in = nodes.begin(), nnode = 0; !nnode && (in != nodes.end()); in++)
	    if(in->first == buf) nnode = in->second;

	if(!nnode) {
	    nnode = db.gettree()->addnode(0, getcolor(cp_menuhl), 0, " " + buf + " ");
	    nodes.push_back(pair<string, int>(buf, nnode));
	}

	id = db.gettree()->addleaf(nnode, 0, it-tlist.begin()+1, " " + lw + " ");
        if(*it == def) cpid = id;
    }

    db.gettree()->setcur(cpid);

    while(1) {
        if(db.open(n, b, (void **) &citem)) {
            if(citem) {
                ret = tlist[citem-1];
                break;
            }
        } else {
            ret = def;
            break;
        }
    }

    db.close();

    return ret;
}

void ncursesui::mainloop() {
    while(!terminate) {
        workareaupdate();

	if(ed.getfcount()) {
	    ed.open();
	} else {
	    execmenubar();
	}
    }
}

void ncursesui::redraw() {
    workareaupdate();
}

void ncursesui::initmenubar() {
    verticalmenu *m;
    menubar = horizontalmenu(1, 0, getcolor(cp_menu), getcolor(cp_menusel), getcolor(cp_menufr));

    menubar.otherkeys = &horizontalmenukeys;

    menubar.additem(_(" File "));
    menubar.additem(_(" Edit "));
    menubar.additem(_(" Project "));
    menubar.additem(_(" VCS "));
    menubar.additem(_(" Debug "));
    menubar.additem(_(" Window "));

    m = menubar.pulldown(0);
    m->additem(_(" Load..         ^O"));
    m->additem(_(" Save           F2"));
    m->additem(_(" Save as.."));
    m->additem(_(" Save all"));
    m->additem(_(" Close          ^D"));
    m->additem(_(" Generate.."));
    m->addline();
    m->additem(_(" Motor settings"));
    m->additem(_(" External keys"));
    m->additem(_(" Regexper      A-R"));
    m->addline();
    m->additem(_(" Quit           ^X"));

    m = menubar.pulldown(1);
    m->additem(_(" Toggle mark         F3"));
    m->additem(_(" Cut                A-X"));
    m->additem(_(" Copy               A-C"));
    m->additem(_(" Paste              A-V"));
    m->addline();
    m->additem(_(" Toggle ins/overw   Ins"));
    m->addline();
    m->additem(_(" Find                ^F"));
    m->additem(_(" Find again          ^G"));
    m->additem(_(" Replace             ^R"));
    m->additem(_(" Go to line..       A-L"));
    m->addline();
    m->additem(_(" Shft block fwd   A-Tab"));
    m->additem(_(" Shft block back  A-Bsp"));
    m->addline();
    m->additem(_(" Undo                ^U"));

    m = menubar.pulldown(2);
    m->additem(_(" Projects registry"));
    m->addline();
    m->additem(_(" Files..             F11"));
    m->additem(_(" Directories..      ^F11"));
    m->additem(_(" Project settings  S-F11"));
    m->addline();
    m->additem(_(" Build                F9"));
    m->additem(_(" Clean              S-F9"));
    m->additem(_(" Make a target..     A-T"));
    m->additem(_(" Regen. build stuff"));
    m->additem(_(" Make a dist package"));
    m->addline();
    m->additem(_(" Find a symbol..     A-?"));
    m->additem(_(" External output.."));

    m = menubar.pulldown(3);
    m->additem(_(" Check in/out"));
    m->additem(_(" Import"));
    m->additem(_(" Tag a release"));
/*
    m->addline();
    m->additem(_(" Browse revisions.."));
    m->addline();
    m->additem(_(" Fork a branch"));
    m->additem(_(" Merge branches.."));
*/
    m = menubar.pulldown(4);
    m->additem(_(" Run program            F12"));
    m->additem(_(" Load core dump.."));
    m->additem(_(" Arguments..          S-F12"));
    m->additem(_(" Reset program         ^F12"));
    m->additem(_(" Call stack..           A-S"));
    m->addline();
    m->additem(_(" Set/Clear breakpoint    ^B"));
    m->additem(_(" Breakpoints            A-B"));
    m->addline();
    m->additem(_(" Add watch               ^W"));
    m->additem(_(" Watches                A-W"));
    m->additem(_(" Evaluate/Modify        A-E"));
    m->addline();
    m->additem(_(" Trace into              F7"));
    m->additem(_(" Step over               F8"));
    m->additem(_(" Go to cursor            F4"));

    m = menubar.pulldown(5);
    m->additem(_(" Next                 ^N"));
    m->additem(_(" Previous             ^P"));
    m->addline();
    m->additem(_(" Window list          F5"));
    m->additem(_(" Last build messages  F6"));
}

void ncursesui::execmenubar() {
    int h = 0, p = 0;
    string buf;

    if(menubar.open(&h, &p))
    switch(h) {
        case 1:
	    switch(p) {
    	        case 1: hotkey(CTRL('o')); break;
    		case 2: ed.fsave(uieditor::savecurrent); break;
		case 3: ed.fsave(uieditor::saveas); break;
		case 4: ed.fsave(uieditor::saveall); break;
    		case 5: ed.closecurrentfile(); break;
    		case 6: generate(); break;
        	case 8: settings(); break;
                case 9: extkeylist(); break;
		case 10: regexper.exec(); break;
		case 12: terminate = true; break;
    	    }
    	    break;
        case 2:
	    switch(p) {
    	        case  1: ed.switchmark(); break;
		case  2: ed.clipboard(uieditor::cut); break;
	        case  3: ed.clipboard(uieditor::copy); break;
		case  4: ed.clipboard(uieditor::paste); break;
    	        case  6: ed.insertmode = !ed.insertmode; break;
        	case  8: hotkey(CTRL('f')); break;
    	        case  9: hotkey(CTRL('g')); break;
		case 10: hotkey(CTRL('r')); break;
    	        case 11:
		    ed.gotoline();
		    break;
    	        case 13:
		    ed.shiftident(-1);
		    ed.redraw();
		    break;
    	        case 14:
		    ed.shiftident(+1);
		    ed.redraw();
		    break;
	        case 16:
		    ed.undo();
		    break;
            }
            break;
        case 3:
	    switch(p) {
	        case  1:
                    pload();
		    break;
	        case  3:
		    projectedit(pfiles);
		    break;
    		case  4:
		    projectedit(pdirs);
		    break;
	        case  5:
		    projectedit(psettings);
		    break;
	        case  7:
                    hotkey(KEY_F(9));
		    break;
	        case  8:
		    project.clean();
		    break;
		case  9:
		    maketarget();
		    break;
		case 10:
		    project.regenerate();
		    break;
	        case 11:
		    dist();
		    break;
	        case 13:
		    uitb.exec();
		    break;
                case 14:
		    showoutput();
		    break;
    	    }
            break;
        case 4:
	    switch(p) {
	        case 1:
		    uv.check();
		    break;
		case 2:
		    uv.import();
		    break;
		case 3:
		    uv.tag();
		    break;
		case 5:
		    uv.explore();
		    break;
		case 7:
		    uv.forkbranch();
		    break;
		case 8:
		    uv.merge();
		    break;
	    }
	    break;
        case 5:
	    switch(p) {
                case  1:
		    if(debugger.running()) {
			debugger.cont();
		    } else {
			debugger.run();
		    }
		    break;
                case  2:
		    loadcore();
		    break;
	        case  3:
		    arguments();
		    break;
	        case  4:
		    debugger.kill();
		    break;
                case  5:
		    hotkey(ALT('s'));
		    break;
	        case  7:
		    hotkey(CTRL('b'));
		    break;
	        case  8:
		    hotkey(ALT('b'));
		    break;
	        case 10:
		    hotkey(CTRL('w'));
		    break;
	        case 11:
		    hotkey(ALT('w'));
		    break;
                case 12:
		    evaluate("");
		    break;
                case 14:
		    debugger.step();
		    break;
	        case 15:
		    debugger.next();
		    break;
                case 16:
		    hotkey(KEY_F(4));
		    break;
            }
            break;
        case 6:
            switch(p) {
	        case 1: ed.switchwindow(+1); break;
	        case 2: ed.switchwindow(-1); break;
		case 4: ed.windowlist(); break;
		case 5: hotkey(KEY_F(6)); break;
	    }
            break;
    }
}

bool ncursesui::horizontalmenukeys(horizontalmenu &hm, int k) {
    bool r = thisui.hotkey(k);
    return thisui.terminate || r;
}

void ncursesui::workareaupdate() {
    int i;

    if(ed.getfcount()) {
	ed.redraw();
    } else {
	attrset(0);
	for(i = 1; i < LINES-1; i++)
	    mvhline(i, 0, ' ', COLS);
    }

    statusupdate();
    watcher.update();
}

void ncursesui::statusupdate() {
    int x, y;
    string pname, fname;

    if(!project.empty())
	pname = (string) _("PROJECT: ") + project.getname();

    if(ed.getfid()) {
	fname = (string) _("FILE: ") + justfname(ed.getfid()) + "  ";
	ed.getpos(&x, &y);
    }

    attrset(getcolor(cp_bottom));
    mvhline(0, 0, ' ', COLS);
    mvprintw(0, 0, "  MOTOR %s   %s%s", VERSION, fname.c_str(), pname.c_str());

    if(ed.getfcount())
	mvprintw(0, COLS-18, "[%c%c%c] %5d:%5d",
	    ed.insertmode ? '-' : 'O',
	    ed.ismark() ? 'B' : '-',
	    ed.modified ? 'M' : '-',
	    x, y+1);

    ed.updatecursor();
}

bool ncursesui::hotkey(int k) {
    int modifiers = getctrlkeys(), line;
    vector<motorconfiguration::extkey>::const_iterator ek;
    string buf;
    bool r = true;

    switch(k) {
        case KEY_F( 1):
	    help();
	    break;

	case KEY_F( 4):
	    if(ed.getfid()) {
                ed.getpos(0, &line);
		debugger.tocursor(ed.getfid(), line+1);
	    }
	    break;

        case KEY_F( 5):
	    ed.windowlist();
	    break;

        case KEY_F( 6):
	    showmessages();
	    break;

        case KEY_F( 7):
            debugger.step();
	    break;

        case KEY_F( 8):
            debugger.next();
	    break;

        case KEY_F( 9):
            project.build();
	    if(executor.begin() != executor.end()) showmessages();
	    break;

        case KEY_F(19):
	    project.clean();
	    break;

        case KEY_F(11):
            if(modifiers & SHIFT_PRESSED) projectedit(psettings); else
            if(modifiers & CONTROL_PRESSED) projectedit(pdirs, true); else
        	projectedit(pfiles);
            break;

        case KEY_F(12):
            if(modifiers & SHIFT_PRESSED) {
		arguments();
	    } else if(modifiers & CONTROL_PRESSED) {
		debugger.kill();
	    } else {
		if(debugger.running()) {
		    debugger.cont();
		} else {
		    debugger.run();
		}
	    }
            break;

        case CTRL('e'):
	    nextmessage();
	    break;

        case CTRL('n'):
	    ed.switchwindow(+1);
	    break;

        case CTRL('p'):
	    ed.switchwindow(-1);
	    break;

        case CTRL('o'):
	    ed.loadfile();
	    break;

        case CTRL('b'):
	    if(ed.getfid()) {
                ed.getpos(0, &line);
		breakpoint bp(ed.getfid(), line+1);

		if(debugger.isbreakpoint(bp)) {
		    debugger.removebreakpoint(bp);
		} else {
		    debugger.addbreakpoint(bp);
		}

		ed.markbreakpoints();
	    }
	    break;

        case CTRL('w'):
	    watcher.add();
	    break;

        case CTRL('f'):
	    uigrep.grep();
	    break;

        case CTRL('r'):
	    uigrep.replace();
	    break;

        case CTRL('g'):
	    uigrep.again();
	    break;

        case ALT('b'):
	    showbreakpoints();
	    break;

        case ALT('w'):
	    watcher.exec();
	    break;

        case ALT('e'):
	    evaluate("");
	    break;

        case ALT('?'):
	    uitb.exec();
	    break;

        case ALT('s'):
	    showstack();
	    break;

        case ALT('r'):
	    regexper.exec();
	    break;

        case ALT('t'):
	    maketarget();
	    break;

        case CTRL('x'):
	    terminate = true;
	    break;

	case KEY_F(10):
	    execmenubar();
	    break;

	default:
	    if(r = ((ek = find(conf.extkeybegin(), conf.extkeyend(), k))
	    != conf.extkeyend())) {
		ek->exec();
	    }
	    break;
    }

    return r;
}

bool ncursesui::isterminated() {
    return terminate;
}

void ncursesui::textboxf(const string &title, const char *fmt, ...) {
    va_list ap;
    char buf[512];
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    textbox(buf, title);
}

void ncursesui::textbox(const string &text, const string &title) {
    dialogbox db;

    db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT,
        getcolor(cp_menufr), TW_CENTERED));

    db.setbrowser(new textbrowser(getcolor(cp_menu)));

    db.setbar(new horizontalbar(getcolor(cp_menu),
	getcolor(cp_menusel), _("Ok"), NULL_CONST_STR));

    db.getwindow()->set_title(getcolor(cp_menuhl), title);
    db.getbrowser()->setbuf(text);

    db.open();
    db.close();
}

int ncursesui::texteditboxkeys(texteditor &ed, int k) {
    switch(k) {
        case CTRL('x'):
	    thisui.texteditok = true;
        case 27:
	    return -1;
    }

    return 1;
}

bool ncursesui::texteditbox(const string &title, string &text) {
    char *sp;
    texteditor ed;

    textwindow w(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT, getcolor(cp_menufr),
	TW_CENTERED, getcolor(cp_menuhl), "%s", title.c_str());

    w.open();

    w.separatey(w.y2-w.y1-2);
    w.write(w.x2-w.x1-20, w.y2-w.y1-1, getcolor(cp_menufr), "^X save  ESC cancel");
    w.write(w.x2-w.x1-20, w.y2-w.y1-1, getcolor(cp_menuhl), "^X");
    w.write(w.x2-w.x1-11, w.y2-w.y1-1, getcolor(cp_menuhl), "ESC");

    ed.setcoords(w.x1+1, w.y1+1, w.x2-1, w.y2-2);
    ed.addscheme(cp_menufr, cp_menufr, 0, 0);

    ed.otherkeys = &texteditboxkeys;
    ed.wrap = false;

    texteditok = false;
    ed.load(text, "");
    ed.open();

    if(texteditok) {
	text = sp = ed.save("\r\n");
        delete sp;
    }

    w.close();
    return texteditok;
}

void ncursesui::help() {
    string helptext;

    helptext += _("F1           Display this help screen\n");
    helptext += _("F10          Activate menu\n");
    helptext += _("^X           Quit motor\n\n");
    helptext += _("F2           Save current file\n");
    helptext += _("^O           Load file\n");
    helptext += _("^D           Close current editor window\n");
    helptext += _("F5           Editor files..\n");
    helptext += _("^N           Next editor window\n");
    helptext += _("^P           Previous editor window\n\n");
    helptext += _("Ins          Toggle insert/overwrite mode\n");
    helptext += _("F3           Toggle block marking mode\n");
    helptext += _("Alt-X, ^Del  Cut selected text\n");
    helptext += _("Alt-C, ^Ins  Copy selected text\n");
    helptext += _("Alt-V, ^Ins  Paste previously copied text\n");
    helptext += _("Alt-Tab      Shift selected block forward\n");
    helptext += _("Alt-Backsp   Shift selected block backward\n\n");
    helptext += _("^U           Undo previous editor operation\n");
    helptext += _("^F           Find text in current file\n");
    helptext += _("^G           Find again\n");
    helptext += _("^R           Search and replace\n");
    helptext += _("Alt-L        Goto line..\n\n");
    helptext += _("Shift-F11    Project settings..\n");
    helptext += _("F11          Project files..\n");
    helptext += _("^F11         Project directories..\n\n");
    helptext += _("F9           Build a project\n");
    helptext += _("Shift-F9     Clean\n");
    helptext += _("Alt-T        Make a specific target..\n");
    helptext += _("F12          Run program\n");
    helptext += _("Shift-F12    Specify program arguments\n");
    helptext += _("Alt-?        Find a symbol definition in the source\n\n");
    helptext += _("F4           Go to cursor\n");
    helptext += _("F7           Debug mode: trace into\n");
    helptext += _("F8           Step over\n\n");
    helptext += _("^B           Set breakpoint at current line\n");
    helptext += _("Alt-B        Breakpoints..\n");
    helptext += _("^W           Add watch\n");
    helptext += _("Alt-W        Watches..\n");
    helptext += _("Alt-E        Evaluate/modify..\n\n");
    helptext += _("Alt-R        Invoke the regexper tool\n\n");
    helptext += _("Note: If your terminal doesn't allow using Alt-keys or\n");
    helptext += _("      Fx keys, you can use ESC 'letter or number' instead.\n");
    helptext += _("      '^' char means the key should be pressed with Ctrl.");

    textbox(helptext, _(" Help "));
}

vector<motorui::editfile> ncursesui::geteditfiles() {
    int saven, i, x, y;
    vector<motorui::editfile> v;

    saven = ed.getfnum();
    for(i = 0; i < ed.getfcount(); i++) {
        ed.setfnum(i);
        ed.getpos(&x, &y);
	v.push_back(motorui::editfile(ed.getfid(i), x, y+1));
    }

    if(ed.getfcount() > 1) {
        v.push_back(v[saven]);
        v.erase(v.begin()+saven);
    }

    ed.setfnum(saven);
    return v;
}

vector<string> ncursesui::geteditfile(const string &fname) {
    int saven, i;
    string buf;
    strstream st;
    vector<string> r;

    for(i = 0; i < ed.getfcount(); i++) {
	if(samefile(fname, ed.getfid(i))) {
	    saven = ed.getfnum();
    	    ed.setfnum(i);
	    buf = ed.save("\n");
	    ed.setfnum(saven);
	    break;
	}
    }

    if(!buf.empty())
        breakintolines(buf, r);

    return r;
}

string ncursesui::inputskel(const string &initval, const string &text, bool selmode) {
    int line = LINES-2;
    int mright = COLS-text.size();
    screenarea sarea(0, line, COLS, line);

    attrset(getcolor(cp_input));
    mvhline(line, 0, ' ', COLS);
    kwriteatf(0, line, getcolor(cp_input), "%s", text.c_str());

    if(selmode) {
        mright -= 8;
        kwriteatf(COLS-8, line, getcolor(cp_input), "[Ctrl-T]");
	inp.connectselector(selector);
    } else {
	inp.removeselector();
    }

    inp.setvalue(initval);
    inp.setcoords(text.size(), line, mright);
    inp.exec();

    sarea.restore();
    return inp.getvalue();
}

void ncursesui::initelements() {
    inp.setcolor(getcolor(cp_input), getcolor(cp_menufr));

    selwindow = textwindow(0, 0, (int) (DIALOG_WIDTH*0.8),
	DIALOG_HEIGHT, getcolor(cp_menufr), TW_CENTERED);

    selector.setcolor(getcolor(cp_menu), getcolor(cp_menuhl),
	getcolor(cp_menusel), getcolor(cp_menu));
}

bool ncursesui::editcloseall() {
    bool ret;

    if(ret = ed.fsave(uieditor::saveallask))
	while(ed.getfcount()) ed.close();

    return ret;
}

bool ncursesui::changetemplate() {
    string nt = selecttemplate(project.gettemplatename());

    if(!nt.empty() && nt != project.gettemplatename())
    if(ask("NY", _("Warning: changing template will rearrange files in project. Proceed?")) == motorui::yes) {
	project.settemplate(nt);
	return true;
    }

    return false;
}

void ncursesui::populatesettingstree(treeview &tree) {
    string cflags, lflags;

    tree.clear();
    project.getflags(cflags, lflags);

    int nmain = tree.addnode(0, getcolor(cp_menuhl), 0, _(" Main "));
    int nvcs = tree.addnode(0, getcolor(cp_menuhl), 0, _(" Version control "));
    int nmake = tree.addnode(0, getcolor(cp_menuhl), 0, _(" Make "));

    tree.addleaff(nmain, 0, 10, _(" Version : %s "), project.getversion().c_str());
    tree.addleaff(nmain, 0, 11, _(" Template : %s "), project.gettemplatename().c_str());
    tree.addleaff(nmain, 0, 12, _(" Root directory : %s "), project.getrootdir().c_str());
    tree.addleaff(nmain, 0, 13, _(" Use GNU gettext for internationalization : %s "), BOOL_TO_STR(project.isgettextized()));

    if(vcs.enabled()) {
	tree.addleaff(nvcs, 0, 14, _(" %s: %s in %s "),
	    project.getvcs().c_str(), project.getvcsmodulename().c_str(),
	    project.getvcsroot().c_str());
    } else {
	tree.addleaf(nvcs, 0, 14, _(" Not under version control "));
    }

    tree.addleaff(nmake, 0, 16, _(" Makefile mode : %s "), MAKEMODE_TO_STR(project.getmakemode()));
    tree.addleaff(nmake, 0, 17, _(" Compiler options : %s "), cflags.c_str());
    tree.addleaff(nmake, 0, 18, _(" Linker options : %s "), lflags.c_str());

    tree.addnode(0, getcolor(cp_menuhl), 30, _(" Files.. "));
    tree.addnode(0, getcolor(cp_menuhl), 31, _(" Directories.. "));
}

bool ncursesui::projectsettings() {
    int n, b, nfiles;
    void *p;
    dialogbox db;
    string cflags, lflags, buf;
    bool rc = false, trc;

    project.getflags(cflags, lflags);

    db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT,
	getcolor(cp_menufr), TW_CENTERED, getcolor(cp_menuhl),
	_(" Project settings ")));
    db.settree(new treeview(getcolor(cp_menu), getcolor(cp_menusel),
	getcolor(cp_menu), getcolor(cp_menu)));
    db.setbar(new horizontalbar(getcolor(cp_menu), getcolor(cp_menusel),
	_("Change"), NULL_CONST_STR));

    nfiles = ed.getfcount();
    for(bool fin = false; nfiles == ed.getfcount() && !fin; ) {
	populatesettingstree(*db.gettree());
        if(fin = !db.open(n, b, &p)) break;

        switch((size_t) p) {
            case 10:
		if(input(motorui::text, buf = project.getversion(),
		_("version: ")) == motorui::yes) {
		    project.setversion(buf);
		}
		break;

            case 11:
		if(changetemplate())
		    rc = true;
		break;

            case 12:
		log(_("Cannot modify the root directory. Please move the files manually"));
                break;

            case 13:
		project.setgettextized(!project.isgettextized());
		break;

            case 14:
		if(vcs.enabled()) {
		    if(ask("NY", _("Do you want to disable version control for the project?")) == motorui::yes) {
			project.setvcsmodule("");
			project.setvcsroot("");
		    }
		} else {
		    uv.import();
		}
		break;

            case 16:
		project.setmakemode(
		    project.getmakemode() == motorproject::automake ?
			motorproject::manual :
			motorproject::automake);
                break;

            case 17:
		if(input(motorui::text, cflags, _("additional compiler command line options: "))
		!= motorui::cancel) {
		    project.setflags(cflags, lflags);
		}
		break;

            case 18:
		if(input(motorui::text, lflags, _("additional linker command line options: "))
		!= motorui::cancel) {
		    project.setflags(cflags, lflags);
		}
		break;

            case 30:
		if(projectcontents(pfiles, false))
		    rc = true;
		break;
            case 31:
		if(projectcontents(pdirs, false))
		    rc = true;
		break;
        }
    }

    db.close();
    return rc;
}

motorui::askresult ncursesui::input(motorui::inputkind kind, string &result,
const string &prompt) {
    string text;
    bool fselmode;
    int fseloptions = 0;

    selwindow.set_title(getcolor(cp_menuhl), _(" <Ins> select; <Space> confirm; <Esc> cancel "));

    switch(kind) {
	case motorui::filechroot:
	    fseloptions |= FSEL_CHROOT;
	    fseloptions |= FSEL_MULTI;
	case motorui::file:
	    fselmode = true;
	    break;

	case motorui::directorychroot:
	    fseloptions |= FSEL_CHROOT;
	case motorui::directory:
	    fselmode = true;
	    fseloptions |= FSEL_DIRSELECT;
	    selector.setstartpoint(result);
	    break;

	case motorui::text:
	    fselmode = false;
	    break;
    }

    selector.setwindow(selwindow);
    selector.setoptions(fseloptions);

    text = inputskel(result, prompt, fselmode);

    if(inp.getlastkey() == KEY_ESC) {
	return motorui::cancel;
    }

    result = text;
    return result.empty() ? motorui::no : motorui::yes;
}

typedef pair<motorfile *, motorfolder *> shitpair;
vector<shitpair> treeshit;

void ncursesui::populatecontentstree(treeview &tree, motorproject &mp, projeditaction pea, bool setcurrent) {
    int foldid, id, cfid = -1, i, sid;
    vector<motorfolder>::iterator ifold;
    vector<motorfile>::iterator ifile;
    static vector<string> cnodes;

    cnodes.clear();

    if(!tree.empty()) {
	for(i = 0; i < tree.menu.getcount(); i++) {
	    id = tree.getid(i);

	    if(tree.isnode(id))
		if(sid = (size_t) tree.getref(id))
		    if(!tree.isnodeopen(id))
			cnodes.push_back(treeshit[sid-1].second->gettagname());
	}

	tree.clear();
	treeshit.clear();
    }

    for(ifold = mp.foldbegin(); ifold != mp.foldend(); ifold++)
    if((pea == pfiles && ifold->getcontentkind() != motorfile::directory)
    || (pea == pdirs && ifold->getcontentkind() == motorfile::directory)) {

        treeshit.push_back(shitpair(0, &(*ifold)));
	foldid = tree.addnode(0, getcolor(cp_menuhl), treeshit.size(), " " + ifold->getname() + " ");

	if(find(cnodes.begin(), cnodes.end(), ifold->gettagname()) != cnodes.end()) {
	    tree.closenode(foldid);
	} else {
	    tree.opennode(foldid);
	}

	for(ifile = ifold->begin(); ifile != ifold->end(); ifile++) {
            treeshit.push_back(shitpair(&(*ifile), &(*ifold)));
	    id = tree.addleaf(foldid, 0, treeshit.size(), " " + ifile->getfname() + " ");

            if(setcurrent)
            if(ifold->getcontentkind() == motorfile::source)
            if(ifile->getfname() == mp.transformfname(motorproject::relative, ed.getfid()))
                cfid = id;
	}
    }

    tree.setcur(cfid);
}

bool ncursesui::projectcontents(projeditaction pea, bool setcurrent) {
    int n, b, citem, id;
    dialogbox db;
    string buf, head, fname;
    motorfolder *fold;
    motorfile *file;
    motorui::askresult ar;
    vector<string> templnames;
    verticalmenu m;
    projectname pname;

    bool fpass = true, tregen, rc = false;

    switch(pea) {
	case pfiles:
	    head = _(" Project files ");
    	    db.setbar(new horizontalbar(getcolor(cp_menu),
		getcolor(cp_menusel), _("Add"), _("Remove"), _("Edit"), NULL_CONST_STR));
    	    db.getbar()->item = 2;
	    break;
	case pdirs:
	    head = _(" Project directories ");
    	    db.setbar(new horizontalbar(getcolor(cp_menu),
		getcolor(cp_menusel), _("Add"), _("Remove"), NULL_CONST_STR));
	    break;
    }

    db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT,
	getcolor(cp_menufr), TW_CENTERED, getcolor(cp_menuhl), head.c_str()));

    db.settree(new treeview(getcolor(cp_menu), getcolor(cp_menusel),
	getcolor(cp_menu), getcolor(cp_menu)));

    treeview &tree = *db.gettree();

    tree.collapsable = true;
    db.addkey(KEY_IC, 0);
    db.addkey(KEY_DC, 1);

    for(bool fin = false; !fin; ) {
        if(tree.empty() || tregen) {
            populatecontentstree(tree, project, pea, setcurrent && ed.getfid() && fpass);
            tregen = false;

	    if(tree.empty()) {
		log(_("No folders of this kind are defined in the template"));
		rc = false;
		break;
	    }
        }

        if(fin = !db.open(n, b, (void **) &citem)) break;

	fpass = false;
        file = treeshit[citem-1].first;
        fold = treeshit[citem-1].second;

	switch(b) {
	    case 0:
		ar = motorui::cancel;
		buf = "";

		switch(fold->getcontentkind()) {
		    case motorfile::source:
			ar = input(motorui::filechroot, buf, _("add file: "));
			break;
		    case motorfile::directory:
			ar = input(motorui::directory, buf, _("add directory: "));
			break;
		    case motorfile::symbol:
			ar = input(motorui::text, buf, _("add symbol: "));
			break;
		    case motorfile::project:
			pname = selectproject(selectonly, fold->gettempltotakefrom());
			if(!pname.empty()) {
			    buf = pname.getname();
                            ar = motorui::yes;
			}
			break;
		}

		if(ar == motorui::yes) {
		    while(!buf.empty()) {
			fname = getword(buf, "\"");
			if(!fname.empty()) {
			    fname = project.transformfname(projectpaths::relative, fname);

    			    if(find(fold->begin(), fold->end(), fname) == fold->end()) {
				if(fold->addfile(motorfile(fname), AF_VCS | AF_CHECKDIR | AF_TAKE)) {
				    tregen = rc = true;
				} else {
				    logf(_("Cannot add ~%s~ to the ~%s~ folder"),
					fname.c_str(), fold->getname().c_str());
				}
			    }
			}
		    }

		    if(tregen) db.redraw();
		}
		break;
	    case 1:
                if(fold && file) {
		    fold->removefile(*file);
		    tregen = rc = true;
		    db.redraw();
		}
		break;
	    case 2:
		if(fold && file)
		switch(fold->getcontentkind()) {
		    case motorfile::source:
			ui.loadeditfile(editfile(file->getfname(), -1, 0),
			    LF_CREATE_IF_NOTFOUND);
                        fin = true;
			break;

		    case motorfile::symbol:
			break;

                    case motorfile::project:
			m = verticalmenu(getcolor(cp_menufr), getcolor(cp_menusel));
			m.setwindow(
			    textwindow(db.getwindow()->x2-17,
				db.getwindow()->y2-3, db.getwindow()->x2+3,
				0, getcolor(cp_menufr)
			    )
			);
			m.additem(_(" Files.."));
			m.additem(_(" Open the project"));
/*
			if(project.getmakemode() == motorproject::automake)
			    m.additem(_(" Build options.."));
*/
			m.scale();
			n = m.open();
			m.close();

			switch(n) {
			    case 1:
				buf = selectprojectfile(projectname(file->getfname()));
				if(fin = !buf.empty()) {
				    ui.loadeditfile(editfile(buf, -1, 0), LF_CREATE_IF_NOTFOUND);
				}
				break;
			    case 2:
				pload(file->getfname());
				rc = false;
				fin = true;
				break;
			    case 3:
				if(editbuildoptions(*file))
				    rc = true;
				break;
			}
			break;

		} else if(fold && !file) {
		    id = tree.getid((void *) citem);

		    if(tree.isnodeopen(id)) {
                        tree.closenode(id);
                    } else {
                        tree.opennode(id);
                    }

                    tree.redraw();
		}

		break;
	}
    }

    db.close();
    return project.modified() || rc;
}

string ncursesui::selectprojectfile(motorproject mp) {
    int n, b, citem;
    string rc;
    dialogbox db;
    motorfolder *fold;
    motorfile *file;
    vector<shitpair> savetreeshit = treeshit;
    bool success;

    if(!mp.empty()) {
	db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT,
	    getcolor(cp_menufr), TW_CENTERED, getcolor(cp_menuhl),
	    _(" Project %s "), mp.getname().c_str()));

	db.settree(new treeview(getcolor(cp_menu), getcolor(cp_menusel),
	    getcolor(cp_menu), getcolor(cp_menu)));

	db.setbar(new horizontalbar(getcolor(cp_menu),
	    getcolor(cp_menusel), _("Open"), NULL_CONST_STR));

	treeview &tree = *db.gettree();
	populatecontentstree(tree, mp, pfiles, false);

	while(1) {
	    success = db.open(n, b, (void **) &citem);
    	    file = treeshit[citem-1].first;
    	    fold = treeshit[citem-1].second;
	    if(!success || file) break;
	}

	if(success && file) {
	    if(fold->getcontentkind() == motorfile::source) {
		rc = file->getfname();
	    }
	}

	db.close();
    }

    treeshit = savetreeshit;
    return rc;
}

bool ncursesui::autosave() {
    ed.fsave(uieditor::saveall);
    return true;
}

void ncursesui::writeoutput(const string &text) {
    vector<string>::iterator il;
    int pos, len = COLS-2, ln;
    string line;

    if(text.empty()) {
	outlines.push_back("");
    } else {
	for(pos = 0; pos < text.size(); pos += len) {
	    line = text.substr(pos, len);
	    outlines.push_back(line);
	}
    }

    while(outlines.size() > OUTLINES_LIMIT)
	outlines.erase(outlines.begin());

    if(!outputblocked) {
	if(!outopen) {
	    outwindow.open();
	    outopen = true;
	}

	ln = outlines.size() >= LOOKER_HEIGHT ? 1 : LOOKER_HEIGHT-outlines.size();
	il = ln == 1 ? outlines.end()-LOOKER_HEIGHT+1 : outlines.begin();

	for(; il != outlines.end(); il++, ln++) {
    	    mvhline(ln, 0, ' ', COLS);

	    if(il->substr(0, 1) == "\001") {
        	kwriteat(0, ln, il->c_str()+1, boldcolor(0));
    	    } else {
        	kwriteat(0, ln, il->c_str(), normalcolor(0));
    	    }
	}

	mvhline(ln, 0, ' ', COLS);
	kwriteat(0, ln, "", normalcolor(0));
    }
}

void ncursesui::doneoutput() {
    if(outopen) {
	outwindow.close();
	outopen = false;
    }
}

void ncursesui::showoutput() {
    textbrowser b(0, 1, COLS, LOOKER_HEIGHT+1, normalcolor(0));
    vector<string>::iterator i;
    string text;

    if(outlines.size() < LOOKER_HEIGHT)
        text.assign(LOOKER_HEIGHT-outlines.size()+1, '\n');

    for(i = outlines.begin(); i != outlines.end(); i++) {
        if(!text.empty()) text += "\n";
        text += *i;
//        text += (i->substr(0, 1) == "\001" ? i->substr(1) : *i);
    }

    log(_("External output viewer, ~ESC~ to close"));
    b.setbuf(text.c_str());
    b.move(KEY_END);
    b.open();
    ed.redraw();
    log("");
}

void ncursesui::pointmessage() {
    string lmsg;
    string::iterator i;
    int k;

    if(!currentmsg->fname.empty())
	loadeditfile(editfile(currentmsg->fname, 0, currentmsg->line));

    k = 0;
    lmsg = currentmsg->description;
    i = lmsg.begin();

    while((i = find(i, lmsg.end(), '~')) != lmsg.end()) {
	lmsg.insert(i-lmsg.begin()+k, "~");
	i += 2;
	k++;
    }

    log(lmsg);
}

void ncursesui::showmessages() {
    int h, pos;
    verticalmenu rm;
    vector<motorexecutor::message>::const_iterator i;
    bool curfound = false, start = false;

    if(executor.begin() != executor.end()) {
	rm = verticalmenu(getcolor(cp_menufr), getcolor(cp_menusel));

	for(i = executor.begin(); i != executor.end(); i++) {
    	    if(!i->fname.empty()) rm.additem(" " + justfname(i->fname) + ":" + i->description + " ");
	    else rm.additem(" " + i->description);
	    if(i == currentmsg) {
		rm.setpos(i-executor.begin());
		curfound = true;
	    }
	}

	if(!curfound) currentmsg = executor.begin();
	if((h = rm.getcount()) > LINES-3) h = LINES-3;
	rm.setwindow(textwindow(0, LINES-3-h, COLS-1, LINES-2, getcolor(cp_menufr)));

        if(start = (pos = rm.open())) {
	    currentmsg = executor.begin()+pos-1;
	}

        rm.close();

	if(start) {
	    pointmessage();
	}
    } else {
	ui.log(_("No build messages"));
    }
}

void ncursesui::nextmessage() {
    if(executor.begin() != executor.end()) {
	if(++currentmsg != executor.end()) {
	    pointmessage();
	} else {
	    currentmsg--;
	    log(_("At the end of the messages"));
	}
    }
}

bool ncursesui::createproject() {
    dialogbox db;
    int nopt, nproj, citem, n, b;
    bool ret = false, gnudoc, gettextized, gensource;
    enum {cmscratch, cmfiles, cmvcs} mode = cmscratch;
    string templname, pname, rootdir, vcsroot, vcsmodule, vcsrevision, buf, savename;
    motorproject::makemodekind makemode = motorproject::automake;
    projectname mp;
    motorproject oldproject;

    vector<string> vcses;
    vector<string>::iterator ivcs;

    gnudoc = gensource = true;
    gettextized = false;

    vcses = manager.getvcslist();
    ivcs = vcses.begin();

    db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT,
        getcolor(cp_menufr), TW_CENTERED, getcolor(cp_menuhl),
        _(" Create a new project ")));

    db.setbar(new horizontalbar(getcolor(cp_menu), getcolor(cp_menusel),
	_("Change"), _("Create"), NULL_CONST_STR));

    db.settree(new treeview(getcolor(cp_menu), getcolor(cp_menusel),
        getcolor(cp_menu), getcolor(cp_menu)));

    treeview &tree = *db.gettree();

    for(bool fin = false; !fin; ) {
        tree.clear();

        nproj = tree.addnode(0, getcolor(cp_menuhl), 0, _(" Project "));

        tree.addleaff(nproj, 0, 10, _(" Creation mode : %s "),
            mode == cmscratch ? _("from scratch") :
            mode == cmfiles ? _("from files") : _("from VCS"));

        tree.addleaff(nproj, 0, 11, _(" Template : %s "), templname.empty() ? _("none") : templname.c_str());
        tree.addleaff(nproj, 0, 12, _(" New project name : %s "), pname.c_str());
        tree.addleaff(nproj, 0, 13, _(" Root directory : %s "), rootdir.c_str());
        tree.addleaff(nproj, 0, 14, _(" Makefile mode : %s "), MAKEMODE_TO_STR(makemode));
        tree.addleaff(nproj, 0, 15, _(" GNU standard documentation : %s "), BOOL_TO_STR(gnudoc));
        tree.addleaff(nproj, 0, 19, _(" Use GNU gettext for internationalization : %s "), BOOL_TO_STR(gettextized));

        if(mode != cmfiles) {
            nopt = tree.addnode(0, getcolor(cp_menuhl), 0, _(" Creation "));

	    if(mode == cmscratch) {
        	tree.addleaff(nopt, 0, 16, _(" Generate inital source: %s "), BOOL_TO_STR(gensource));
	    } else if(mode == cmvcs) {
                tree.addleaff(nopt, 0, 20, _(" VCS name : %s "), ivcs->c_str());
                tree.addleaff(nopt, 0, 17, _(" Repository : %s "), vcsroot.c_str());
                tree.addleaff(nopt, 0, 18, _(" Module name : %s "), vcsmodule.c_str());
                tree.addleaff(nopt, 0, 21, _(" Revision : %s "), vcsrevision.c_str());
	    }
        }

        if(!db.open(n, b, (void **) &citem)) {
            ret = false;
            fin = true;
            continue;
        }

        switch(b) {
            case 0:
                switch(citem) {
                    case 10:
                        mode = mode == cmscratch ? cmfiles :
                    	    mode == cmfiles ? cmvcs :
                    	    cmscratch;

                        if(mode == cmfiles)
			    makemode = motorproject::manual;

			if((mode == cmvcs) && vcses.empty()) {
			    log(_("No version control systems defined"));
			    mode = cmscratch;
			}

                        break;

                    case 11:
                        templname = selecttemplate(templname);
                        break;

                    case 12:
                        if(input(motorui::text, pname, _("project name: ")) == motorui::yes) {
                            buf = rootdir.substr(0, conf.getdefaultprojectsdir().size());
                            if(vcsmodule.empty()) vcsmodule = pname;
                            if(rootdir.empty() || (buf == conf.getdefaultprojectsdir())) {
                                rootdir = trailcut(conf.getdefaultprojectsdir(), "/") + "/" + pname;
                            }
                        }
                        break;

                    case 13:
			input(motorui::directory, rootdir, _("root directory: "));
                        if(pname.empty()) pname = justfname(rootdir);
                        break;

                    case 14:
                        makemode =
                            makemode == motorproject::manual ?
				motorproject::automake :
                        	motorproject::manual;
                        break;

                    case 15: gnudoc = !gnudoc; break;
                    case 16: gensource = !gensource; break;

                    case 17:
			input(motorui::text, vcsroot, _("VCS repository to check out from: "));
                        break;

                    case 18:
			input(motorui::text, vcsmodule, _("VCS module name: "));
                        break;

		    case 19: gettextized = !gettextized; break;

		    case 20:
			if(++ivcs == vcses.end())
			    ivcs = vcses.begin();
			break;

		    case 21:
			input(motorui::text, vcsrevision, _("VCS revision: "));
			break;
                }
                break;

            case 1:
		ret = true;
		break;
        }

	if(ret) {
	    if(rootdir.empty()) {
		log(_("Project root directory must be set"));
		ret = false;
	    }

	    if(ret)
	    if(pname.empty()) {
		log(_("Project name must be set"));
		ret = false;
	    }

	    if(ret)
	    if(templname.empty()) {
		log(_("Template must be set"));
		ret = false;
	    }

	    if(ret)
	    if(!(mp = projectname(pname)).empty()) {
		logf(_("Project named %s already exists"), pname.c_str());
		ret = false;
	    }

	    if(ret)
	    if(pname.substr(0, 1).find_first_of("0123456789") != -1) {
		log(_("Project name cannot start with a number"));
		ret = false;
	    }

	    if(ret)
	    if(mode == cmvcs)
	    if(vcsroot.empty() || vcsmodule.empty()) {
		log(_("VCS name, repository and module name must be specified"));
		ret = false;
            }

	    if(ret)
	    if(mode != cmfiles)
	    if(access(rootdir.c_str(), F_OK))
	    if(ret = (ask("YNC", _("root directory you specified does not exist. create?")) == motorui::yes)) {
        	mksubdirs(rootdir);
	    }

	    if(ret)
	    if(access(rootdir.c_str(), R_OK | W_OK | X_OK)) {
                log(_("Access denied for the directory specified"));
		ret = false;
            }

	    fin = ret;
	}
    }

    savename = project.getname();

    if(ret)
    if(project.close()) {
	project = motorproject();

	project.setname(pname);
	project.setversion("0.1");
	project.settemplate(templname);
	project.setrootdir(rootdir);
	project.setgettextized(gettextized);
	project.setmakemode(makemode);

	n = 0;
	if(gnudoc) n |= CR_GNU_DOC;

	switch(mode) {
	    case cmscratch:
		if(gensource) n |= CR_GENERATE_SOURCE;
		break;
	    case cmvcs:
		project.setvcs(*ivcs);
		project.setvcsroot(vcsroot);
		project.setvcsmodule(vcsmodule, vcsrevision);
		break;
	}

	if(!(ret = project.create(n))) {
            if(!savename.empty()) {
                project = motorproject(projectname(savename));
	        onprojectload();
            }
	} else {
	    vector<motordist>::const_iterator di;
	    vector<motordist::distparam>::const_iterator pi;

	    for(di = project.distbegin(); di != project.distend(); ++di)
	    for(pi = di->parambegin(); pi != di->paramend(); ++pi)
	        distparams[pi->name] = pi->defval;
	}
    }

    db.close();
    return ret;
}

void ncursesui::dist() {
    int maxl, i, b, fid;
    bool r, fin, first;
    textwindow *wdist;
    dialogbox db;
    vector<motordist>::const_iterator di;
    vector<motordist::distparam>::const_iterator pi;

    verticalmenu mdist(getcolor(cp_menu), getcolor(cp_menusel));

    if(project.distbegin() == project.distend()) {
        logf(_("No distribution methods defined in template %s"),
	    project.gettemplatename().c_str());
	return;
    }

    if(distdir.empty())
	distdir = project.getrootdir();

    for(maxl = 0, di = project.distbegin(); di != project.distend(); di++) {
	if(maxl < di->getname().size()) maxl = di->getname().size();
	mdist.additem(" " + di->getname());
    }

    db.setwindow(wdist = new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT, getcolor(cp_menufr),
	TW_CENTERED, getcolor(cp_menuhl), _(" Distribution package generation ")));

    mdist.setwindow(textwindow(wdist->x1+6, wdist->y1+1, wdist->x1+9+maxl,
	wdist->y1+2+mdist.getcount(), getcolor(cp_menufr)));

    db.settree(new treeview(getcolor(cp_menu), getcolor(cp_menusel),
        getcolor(cp_menu), getcolor(cp_menu)));

    db.setbar(new horizontalbar(getcolor(cp_menu),
        getcolor(cp_menusel), _("Change"), _("Go!"), NULL_CONST_STR));

    di = project.distbegin();
    treeview &tree = *db.gettree();

    for(fin = r = false, first = true; !fin && !r; ) {
        tree.clear();

        i = tree.addnode(0, getcolor(cp_menuhl), 0, _(" Format "));
        fid = tree.addleaf(i, 0, 0, " " + di->getname() + " ");

        i = tree.addnode(0, getcolor(cp_menuhl), 0, _(" Out directory "));
        tree.addleaf(i, 0, 0, " " + distdir + " ");

	for(pi = di->parambegin(); pi != di->paramend(); ++pi) {
	    if(pi == di->parambegin())
	        i = tree.addnode(0, getcolor(cp_menuhl), 0, _(" Parameters "));

	    tree.addleaff(i, 0, 0, " %s : %s ", pi->title.c_str(), distparams[pi->name].c_str());
	}

        if(first) {
            tree.setcur(fid);
            b = 0;
            i = 2;
            first = false;
            db.redraw();
        } else {
            if(!db.open(i, b)) break;
        }

	if(!(r = (b == 1))) {
	    if(i == 2) {
		if(mdist.getcount() == 1) {
		    di = project.distbegin();
		} else {
                    if(i = mdist.open()) {
                        di = project.distbegin()+i-1;
                    }
                    mdist.close();
                }
	    } else if(i == 4) {
		input(motorui::directory, distdir, _("directory to put a dist to: "));
	    } else if(i > 5) {
		pi = di->parambegin()+i-6;
		input(motorui::directory, distparams[pi->name], pi->title + ": ");
	    }
	}
    }

    db.close();
    if(r) {
	for(map<string, string>::const_iterator mi = distparams.begin(); mi != distparams.end(); ++mi)
	    executor.setvar(mi->first, mi->second);

	di->exec(distdir);
    }
}

void ncursesui::executordone(const string &target, int nerr, int nwarn, int ninfo) {
    if(nerr || nwarn || ninfo) {
	logf(_("Finished ~%s~: ~%lu~ errors, ~%lu~ warnings, ~%lu~ infos. ~F6~ to see the list"),
    	    target.c_str(), nerr, nwarn, ninfo);
    } else {
	logf(_("~%s~ finished sucessfully"), target.c_str());
    }
}

void ncursesui::projectedit(projeditaction pea, bool setcurrent) {
    bool modified = false;

    switch(pea) {
	case psettings: modified = projectsettings(); break;
	case pfiles: modified = projectcontents(pea, setcurrent); break;
	case pdirs: modified = projectcontents(pea); break;
    }

    if(project.modified() || modified) {
	log(_("The project has been modified. Regenerating the build stuff.."));
	project.regenerate();
	project.save();
    }
}

void ncursesui::maketarget() {
    static int pos = 0;
    static vector<string> targets;
    dialogbox db;
    string buf, tomake;
    vector<string>::iterator is;
    int i, b;

    db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT,
	getcolor(cp_menufr), TW_CENTERED, getcolor(cp_menuhl),
	_(" Make a target.. ")));
    db.setmenu(new verticalmenu(getcolor(cp_menu), getcolor(cp_menusel)));
    db.setbar(new horizontalbar(getcolor(cp_menu), getcolor(cp_menusel),
	_("Add"), _("Remove"), _("Make"), NULL_CONST_STR));

    db.addkey(KEY_IC, 0);
    db.addkey(KEY_DC, 1);
    db.getbar()->item = 2;

    for(bool fin = false; !fin; ) {
	db.getmenu()->clear();
	for(is = targets.begin(); is != targets.end(); is++)
	    db.getmenu()->additem(" " + *is);
	db.getmenu()->setpos(pos);

	if(!(fin = !db.open(i, b))) {
	    pos = db.getmenu()->getpos();

	    switch(b) {
		case 0:
		    if(input(motorui::text, buf = "", _("make a target: ")) == motorui::yes) {
			targets.push_back(buf);
	    		i = targets.size();
		    }
    		case 2:
		    if(fin = !targets.empty()) {
			tomake = targets[i-1];
		    }
		    break;
		case 1:
		    if(!targets.empty())
			targets.erase(targets.begin()+i);
		    break;
	    }
	}
    }

    db.close();

    if(!tomake.empty()) {
	project.runtarget(tomake);
    }
}

void ncursesui::setdebugcurrentline(const editfile ef) {
    static int prevfn, prevline;
    static string prevfname;
    vector<breakpoint> bps;
    vector<breakpoint>::iterator ibp;

    if((prevfname != ef.fname) || (prevline != ef.y)) {
        ed.unlight(prevfn, prevline);

        if(ed.getfid(prevfn)) {
            bps = debugger.getbreakpoints();

            if((ibp = find(bps.begin(), bps.end(), pair<string, int>(ed.getfid(prevfn), prevline))) != bps.end())
                if(ibp->permanent())
                    ed.highlight(prevfn, ibp->getline(), cp_debug_breakpoint);
        }

        if(!ef.fname.empty()) {
            loadeditfile(ef);
            ed.highlight(ed.getfnum(), ef.y, cp_debug_current);
        }

        prevfname = ef.fname;
        prevfn = ed.getfnum();
        prevline = ef.y;

        ed.redraw();
    }
}

void ncursesui::evaluate(const string &e) {
    dialogbox db;
    string val, buf;
    int i, b, nexp;
    static string re;

    db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, (int) (DIALOG_HEIGHT*0.45),
        getcolor(cp_menufr), TW_CENTERED,
	getcolor(cp_menuhl), _(" Evaluate/Modify ")));

    db.settree(new treeview(getcolor(cp_menu), getcolor(cp_menusel),
        getcolor(cp_menu), getcolor(cp_menu)));

    db.setbar(new horizontalbar(getcolor(cp_menu),
        getcolor(cp_menusel), _("Change"), NULL_CONST_STR));

    if(!e.empty()) re = e;

    treeview &tree = *db.gettree();

    for(bool fin = false; !fin; ) {
        if(!re.empty()) {
            val = debugger.getvar(re);
        }

        tree.clear();

        i = tree.addnode(0, getcolor(cp_menuhl), 0, _(" Expression "));
        nexp = tree.addleaf(i, 0, 0, " " + re + " ");

        i = tree.addnode(0, getcolor(cp_menuhl), 0, _(" Value "));
        tree.addleaf(i, 0, 0, " " + (val.empty() ? _("not available") : val) + " ");

        if(re.empty()) {
            i = 2;
            tree.setcur(nexp);
        } else {
            if(!db.open(i, b)) break;
        }

        switch(i) {
            case 2:
		fin = input(motorui::text, re, _("expression: ")) != motorui::yes;
                break;
            case 4:
		watcher.modify(re, val);
                break;
        }
    }

    db.close();
}

void ncursesui::settings() {
    dialogbox db;
    int n, b, citem;
    bool fdispvcs, fdispmake, fdispcomp, fdebugstd, fchecktty, fmcedclip, fsmarttab;
    string fdefprojdir;

    db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT,
	getcolor(cp_menufr), TW_CENTERED, getcolor(cp_menuhl),
	_(" Motor settings ")));

    db.setbar(new horizontalbar(getcolor(cp_menu),
	getcolor(cp_menusel), _("Change"), _("Done"), NULL_CONST_STR));

    db.settree(new treeview(getcolor(cp_menu), getcolor(cp_menusel),
        getcolor(cp_menu), getcolor(cp_menu)));

    treeview &tree = *db.gettree();

    fdebugstd = conf.getdebugstd();
    fdefprojdir = conf.getdefaultprojectsdir();
    fchecktty = conf.getchecktty();
    fmcedclip = uiconf.getmcedclip();
    fsmarttab = uiconf.getsmarttab();

    conf.getdisplay(fdispmake, fdispcomp, fdispvcs);

    for(bool fin = false; !fin; ) {
	tree.clear();

	n = tree.addnode(0, getcolor(cp_menuhl), 0, _(" Show the run-time output "));
	tree.addleaff(n, 0, 1, _(" show compiler output : %s "), BOOL_TO_STR(fdispcomp));
	tree.addleaff(n, 0, 2, _(" show VCS output : %s "), BOOL_TO_STR(fdispvcs));
	tree.addleaff(n, 0, 3, _(" show make output : %s "), BOOL_TO_STR(fdispmake));

	n = tree.addnode(0, getcolor(cp_menuhl), 0, _(" Debugger "));
        tree.addleaff(n, 0, 5, _(" debug standard headers : %s "), BOOL_TO_STR(fdebugstd));
        tree.addleaff(n, 0, 7, _(" disable free tty check : %s "), BOOL_TO_STR(!fchecktty));

	n = tree.addnode(0, getcolor(cp_menuhl), 0, _(" Editor "));
	tree.addleaff(n, 0, 9, _(" enable \"smart tabs\" feature : %s "), BOOL_TO_STR(fsmarttab));
	tree.addleaff(n, 0, 8, _(" enable mcedit clipboard integration : %s "), BOOL_TO_STR(fmcedclip));

	n = tree.addnode(0, getcolor(cp_menuhl), 0, _(" Directories and paths "));
	tree.addleaff(n, 0, 6, _(" default directory for projects : %s "), fdefprojdir.c_str());

	if(!db.open(n, b, (void **) &citem)) break;

	if(!b)
	switch(citem) {
	    case 1: fdispcomp = !fdispcomp; break;
	    case 2: fdispvcs = !fdispvcs; break;
	    case 3: fdispmake = !fdispmake; break;
	    case 5: fdebugstd = !fdebugstd; break;
	    case 6:
		input(motorui::directory, fdefprojdir, _("default directory for projects: "));
		break;
	    case 7: fchecktty = !fchecktty; break;
	    case 8: fmcedclip = !fmcedclip; break;
	    case 9: fsmarttab = !fsmarttab; break;
	} else {
	    conf.setdefaultprojectsdir(fdefprojdir);
	    conf.setdisplay(fdispmake, fdispcomp, fdispvcs);
	    conf.setdebugstd(fdebugstd);
	    conf.setchecktty(fchecktty);
	    uiconf.setmcedclip(fmcedclip);

	    uiconf.setsmarttab(fsmarttab);
	    ed.smarttab = fsmarttab;

	    conf.save();
	    fin = true;
	}
    }

    db.close();
}

void ncursesui::extkeylist() {
    int count, i;
    vector<motorconfiguration::extkey>::const_iterator ik;
    verticalmenu m(getcolor(cp_menufr), getcolor(cp_menusel));

    if(count = conf.extkeyend()-conf.extkeybegin()) {
	for(ik = conf.extkeybegin(); ik != conf.extkeyend(); ik++) {
	    m.additem(" " + ik->description());
	}

	m.setwindow(textwindow(1, 1, (int) (COLS*0.8), count > LINES-4 ? LINES-4 : count+2,
	    getcolor(cp_menufr), 0, getcolor(cp_menuhl), _(" External programs ")));

	i = m.open();
	m.close();

	if(i) (conf.extkeybegin()+i-1)->exec();
    } else {
	log(_("No external keys defined"));
    }
}

void ncursesui::reloadeditfiles() {
    vector<motorui::editfile> openfiles;
    vector<motorui::editfile>::iterator iof;

    openfiles = geteditfiles();

    if(autosave()) {
	editcloseall();

	for(iof = openfiles.begin(); iof != openfiles.end(); iof++) {
	    loadeditfile(*iof);
	}
    }
}

void ncursesui::externalexec(int options, const string &cmd) {
    int i;
    string s;

    if(autosave()) {
	attrset(normalcolor(0));
	endwin();
	for(i = 0; i <= LINES; i++) cout << endl;
	cout << flush;

	system(cmd.c_str());

	if(options & EE_PAUSE) {
	    cout << _("Press Enter to continue..") << flush;
	    getstring(cin, s);
	}

	refresh();
	reloadeditfiles();
    }
}

void ncursesui::showbreakpoints() {
    dialogbox db;
    int n, b, i;
    vector<breakpoint> bps;
    vector<breakpoint>::iterator ib;
    editfile ef;

    db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT,
        getcolor(cp_menufr), TW_CENTERED,
	getcolor(cp_menuhl), _(" Breakpoints ")));

    db.settree(new treeview(getcolor(cp_menu), getcolor(cp_menusel),
        getcolor(cp_menu), getcolor(cp_menu)));

    db.setbar(new horizontalbar(getcolor(cp_menu),
        getcolor(cp_menusel), _("Remove"), _("Goto"), NULL_CONST_STR));

    db.addkey(KEY_DC, 0);
    db.getbar()->item = 1;

    treeview &tree = *db.gettree();

    for(bool fin = false; !fin; ) {
        tree.clear();

	if(ed.getfid()) {
	    int x, y;
	    ed.getpos(&x, &y);
	    ef = editfile(ed.getfid(), x, y+1);
	}

	for(bps = debugger.getbreakpoints(), ib = bps.begin(); ib != bps.end(); ib++) {
	    i = tree.addleaff(0, 0, (ib-bps.begin()+1), " %s:%lu ",
		ib->getfname().c_str(), ib->getline());

	    if((ib->getfname() == ef.fname) && (ib->getline() == ef.y))
		tree.setcur(i);
	}

        ef = editfile();
	if(!db.open(n, b, (void **) &i)) break;

	if(i)
	switch(b) {
	    case 0:
		ib = bps.begin()+i-1;
		debugger.removebreakpoint(*ib);
		break;
	    case 1:
		fin = true;
		ib = bps.begin()+i-1;
		ef = editfile(ib->getfname(), 0, ib->getline());
		break;
	}
    }

    db.close();

    if(!ef.fname.empty()) {
	loadeditfile(ef);
    }

    ed.markbreakpoints();
}

void ncursesui::generate() {
    int n, b;
    dialogbox db;
    vector<sourcetemplate> tg;
    vector<sourcetemplate>::const_iterator it;
    vector<sourcetemplate>::iterator itg;
    bool start = false;

    db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT,
	getcolor(cp_menufr), TW_CENTERED, getcolor(cp_menuhl),
	_(" Files to generate ")));

    db.setmenu(new verticalmenu(getcolor(cp_menu), getcolor(cp_menusel)));

    db.setbar(new horizontalbar(getcolor(cp_menu), getcolor(cp_menusel),
	_("Mark"), _("Generate"), NULL_CONST_STR));

    db.addkey(' ', 0);
    verticalmenu &menu = *db.getmenu();

    for(bool fin = false; !fin; ) {
	menu.clear();

	for(it = project.templbegin(); it != project.templend(); it++) {
	    menu.additemf(" [%c] %s ",
		find(tg.begin(), tg.end(), it->getfname()) != tg.end() ? 'x' : ' ',
		it->getfname().c_str());
	}

	if(!db.open(n, b)) break;

	if(!b) {
	    it = project.templbegin()+n-1;

	    if((itg = find(tg.begin(), tg.end(), it->getfname())) != tg.end()) {
		tg.erase(itg);
	    } else {
		tg.push_back(*it);
	    }
	} else {
	    start = fin = true;
	}
    }

    db.close();

    if(start) {
	for(it = tg.begin(); it != tg.end(); it++) {
	    it->generate();
	}
    }
}

void ncursesui::arguments() {
    string buf;

    if(input(motorui::text, buf = project.getarguments(),
    _("arguments: ")) != motorui::cancel) {
	project.setarguments(buf);
    }
}

void ncursesui::loadcore() {
    dialogbox db;
    string corefname, r;
    int n, b;

    if(input(motorui::file, corefname = project.getrootdir() + "/core",
    _("Core dump to explore: ")) == motorui::yes) {
        if(!(r = debugger.loadcore(corefname)).empty()) {
            log();

            db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT,
                getcolor(cp_menufr), TW_CENTERED, getcolor(cp_menuhl),
	        _(" Core dump load results ")));

            db.setbar(new horizontalbar(getcolor(cp_menu),
                getcolor(cp_menusel), _("Inspect stack"), _("Close"), NULL_CONST_STR));

            db.setbrowser(new textbrowser(getcolor(cp_menu)));
            db.getbrowser()->setbuf(r);

	    for(bool fin = false; !fin; ) {
		fin = !db.open(n, b) || b;
		if(!fin) fin = showstack();
	    }

            db.close();
        }
    }
}

bool ncursesui::showstack() {
    bool start;
    dialogbox db;
    vector<stackitem> stack;
    vector<stackitem>::iterator i;
    static int savecount = 0, citem;
    int n, b;
    string desc;

    start = false;

    if(!(stack = debugger.getstack()).empty()) {
	db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT,
    	    getcolor(cp_menufr), TW_CENTERED,
    	    getcolor(cp_menuhl), _(" Call stack ")));

	db.setmenu(new verticalmenu(getcolor(cp_menu), getcolor(cp_menusel)));
        db.setbar(new horizontalbar(getcolor(cp_menu), getcolor(cp_menusel),
	    _("Info"), _("Go to"), NULL_CONST_STR));

	db.getbar()->item = 1;

	for(i = stack.begin(); i != stack.end(); i++) {
	    db.getmenu()->additemf(" %s() ", i->getname().c_str());
	}

	if(savecount != stack.size()) {
    	    savecount = stack.size();
    	    citem = 0;
	} else {
    	    db.getmenu()->setpos(citem);
	}

	for(bool fin = false; !fin; ) {
	    if(!(fin = !db.open(n, b))) {
		i = stack.begin()+n-1;

    		switch(b) {
		    case 0:
                        desc = _("Function:");
                        desc += "\n  " + i->getname() + "\n\n";
                        desc += _("Location:");

                        desc += "\n  ";
                        if(i->getlocation().fname.empty()) desc += "unknown";
                        else desc += i->getlocation().fname + ":" + i2str(i->getlocation().y);
                        desc += "\n\n";

                        desc += _("Arguments:");
                        desc += "\n  " + i->getarguments();

			textbox(desc, _(" Stack item details "));
			break;
        	    case 1:
			start = fin = true;
                        citem = n-1;
			break;
    		}
	    }
	}

	db.close();

	if(start) {
	    loadeditfile(i->getlocation());
	}
    }

    return start;
}

void ncursesui::setdesktop(const vector< pair<string, string> > &asettings) {
    string afilter, param, buf;
    vector< pair<string, string> >::const_iterator i;
    vector< pair<char, string> >::const_iterator ik;

    watcher.close();
    distparams.clear();

    for(ik = project.tagnamebegin(); ik != project.tagnameend(); ++ik)
	afilter += ik->first;

    uitb.setfilter(afilter);

    for(i = asettings.begin(); i != asettings.end(); i++) {
	if(i->first == "ncurses_watchactive") {
	    if(i->second == "1") watcher.activate();

	} else if(i->first == "distdir") {
	    distdir = i->second;

	} else if(i->first.substr(0, 9) == "distparam") {
	    buf = i->second;
	    param = getword(buf);
	    distparams[param] = buf;

	} else if(i->first == "tagbrowser_scope") {
	    tagbrowser.setscope((motortagbrowser::viewscope)
		strtoul(i->second.c_str(), 0, 0));

	} else if(i->first == "tagbrowser_filter") {
	    uitb.setfilter(i->second);

	}
    }
}

vector< pair<string, string > > ncursesui::getdesktop() const {
    int pn;
    vector< pair<string, string> > r;

    r.push_back(pair<string, string>("ncurses_watchactive",
	watcher.visible() ? "1" : "0"));

    r.push_back(pair<string, string>("distdir", distdir));
    r.push_back(pair<string, string>("tagbrowser_scope", i2str((int) tagbrowser.getscope())));
    r.push_back(pair<string, string>("tagbrowser_filter", uitb.getfilter()));

    pn = 0;
    for(map<string, string>::const_iterator ip = distparams.begin(); ip != distparams.end(); ++ip, pn++)
	r.push_back(make_pair(string("distparam") + i2str(pn), ip->first + " " + ip->second));

    return r;
}

void ncursesui::updatewatches() {
    watcher.update();
}

bool ncursesui::editmodified() const {
    return ed.anymodified();
}

void ncursesui::onprojectload() {
    project.editload();
    project.debugload();
    ed.markbreakpoints();
    selector.setstartpoint(project.getrootdir());
    logf(_("Project %s has been loaded"), project.getname().c_str());
}

motorui::askresult ncursesui::notemplate(string &templname) const {
    char buf[512];
    sprintf(buf, _("%s not found, please select template"), templname.c_str());
    string nt = selecttemplate("", buf);

    if(!nt.empty()) templname = nt; else
	return cancel;

    return yes;
}

bool ncursesui::editbuildoptions(motorfile &f) const {
    dialogbox db;
    int i, n, b, citem;
    bool enabled, start;
    motorfile::build bd = f.getbuild();

    db.setwindow(new textwindow(0, 0, (int) (DIALOG_WIDTH*0.9), (int) (DIALOG_HEIGHT*0.6),
        getcolor(cp_menufr), TW_CENTERED,
	getcolor(cp_menuhl), _(" %s build options "),
	f.getfname().c_str()));

    db.settree(new treeview(getcolor(cp_menu), getcolor(cp_menusel),
        getcolor(cp_menu), getcolor(cp_menu)));

    db.setbar(new horizontalbar(getcolor(cp_menu),
        getcolor(cp_menusel), _("Change"), _("Done"), NULL_CONST_STR));

    treeview &tree = *db.gettree();

    start = false;
    enabled = !bd.param.empty();

    for(bool fin = false; !fin; ) {
        tree.clear();

        i = tree.addnode(0, getcolor(cp_menuhl), 0, _(" General "));
        tree.addleaff(i, 0, 1, _(" Enable optional build : %s "), BOOL_TO_STR(enabled));

	if(enabled) {
	    i = tree.addnode(0, getcolor(cp_menuhl), 0, _(" Build "));
	    tree.addleaff(i, 0, 2, _(" Parameter name (--enable-xxx) : %s "), bd.param.c_str());
	    tree.addleaff(i, 0, 3, _(" Parameter help text : %s "), bd.help.c_str());
	    tree.addleaff(i, 0, 4, _(" Enabled by default : %s "), BOOL_TO_STR(bd.def));
	}

	if(!db.open(n, b, (void **) &citem)) break;

	if(!b)
	switch(citem) {
	    case 1: enabled = !enabled; break;
	    case 2: ui.input(motorui::text, bd.param, _("configure parameter name (--enable-xxx): ")); break;
	    case 3: ui.input(motorui::text, bd.help, _("help text: ")); break;
	    case 4: bd.def = !bd.def; break;
	}
	else {
	    fin = start = true;
	}
    }

    db.close();

    if(start)
	f.setbuild(bd);

    return start;
}
