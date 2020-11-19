#include "uigrepper.h"
#include "motorproject.h"
#include "motormisc.h"
#include "uieditor.h"

uigrepper uigrep;

uigrepper::uigrepper() {
    initmode = true;
    fregexp = fcase = fwhole = false;
    grepmode = motorgrepper::currentfile;
}

uigrepper::~uigrepper() {
}

bool uigrepper::init() {
    dialogbox db;
    int i, b, n;
    bool found, start, r;
    string buf;
    vector<projectname> plst;
    vector<string> gplst, gflst;
    vector<motorfolder>::iterator ifold;
    vector<projectname>::iterator ipn;
    vector<motorfile>::iterator ifile;
    vector<string>::iterator is;

    db.setwindow(new textwindow(0, 0, (int) (DIALOG_WIDTH*0.65), (int) (DIALOG_HEIGHT*0.8),
	uiconf.getcolor(cp_menufr), TW_CENTERED, uiconf.getcolor(cp_menuhl),
	replacemode ? _(" Motor grepper: replace mode ") : _(" Motor grepper ")));

    db.setbar(new horizontalbar(uiconf.getcolor(cp_menu),
	uiconf.getcolor(cp_menusel), _("Change/Mark"), _("Grep"), 0));

    db.settree(new treeview(uiconf.getcolor(cp_menu), uiconf.getcolor(cp_menusel),
        uiconf.getcolor(cp_menu), uiconf.getcolor(cp_menu)));

    db.addkey(' ', 0);

    r = true;
    start = false;
    treeview &tree = *db.gettree();

    for(bool fin = false, first = true; !fin; first = false) {
	tree.clear();

	i = tree.addnode(0, uiconf.getcolor(cp_menuhl), 0, _(" Search "));
    	n = tree.addleaff(i, 0, 1, _(" Look for: %s "), pattern.c_str());
	if(first) tree.setcur(n);

	if(replacemode) {
	    tree.addleaff(i, 0, 10, _(" Replace with: %s "), replacement.c_str());
	}

	tree.addleaff(i, 0, 2, _(" Area: %s "),
	    grepmode == motorgrepper::currentfile ? _("current file") :
	    grepmode == motorgrepper::folders ? _("selected folders") :
	    grepmode == motorgrepper::allfiles ? _("all the project files") :
	        _("connected projects"));

	switch(grepmode) {
	    case motorgrepper::folders:
        	i = tree.addnode(0, uiconf.getcolor(cp_menuhl), 0, _(" Folders "));

                for(ifold = project.foldbegin(); ifold != project.foldend(); ifold++)
		    if(ifold->getcontentkind() == motorfile::source) {
		        n = 50+(ifold-project.foldbegin());
		        found = find(gflst.begin(), gflst.end(), ifold->gettagname()) != gflst.end();

			tree.addleaff(i, 0, n, " [%c] %s ",
			    found ? 'x' : ' ', ifold->gettagname().c_str());
		    }
		break;

	    case motorgrepper::connected:
        	i = tree.addnode(0, uiconf.getcolor(cp_menuhl), 0, _(" Projects "));
		n = 50;

                for(ifold = project.foldbegin(); ifold != project.foldend(); ifold++) {
		    if(ifold->getcontentkind() == motorfile::project) {
			for(ifile = ifold->begin(); ifile != ifold->end(); ifile++) {
			    found = find(gflst.begin(), gflst.end(), ifile->getfname()) != gflst.end();
			    tree.addleaff(i, 0, n++, " [%c] %s ",
				found ? 'x' : ' ', ifile->getfname().c_str());
			}
		    }
		}
		break;
	}

	i = tree.addnode(0, uiconf.getcolor(cp_menuhl), 0, _(" Options "));
	tree.addleaff(i, 0, 3, _(" Case sensitive: %s "), BOOL_TO_STR(fcase));
	tree.addleaff(i, 0, 4, _(" Whole words only: %s "), BOOL_TO_STR(fwhole));
	tree.addleaff(i, 0, 5, _(" Regular expression: %s "), BOOL_TO_STR(fregexp));

	if(first) {
	    buf = ed.atcursor();
	    n = 1;
	    b = 0;
	} else {
	    buf = pattern;
	    if(!db.open(i, b, (void **) &n)) break;
	}

	if(!b) {
	    switch(n) {
		case 1:
		    ui.input(motorui::text, pattern = buf, _("Look for: "));
		    r = !(fin = pattern.empty());
		    break;
		case 2:
		    grepmode =
			grepmode == motorgrepper::currentfile ? motorgrepper::allfiles :
			grepmode == motorgrepper::allfiles ? motorgrepper::folders :
			grepmode == motorgrepper::folders ? motorgrepper::connected :
			    motorgrepper::currentfile;
		    break;
		case 3:
		    fcase = !fcase;
		    break;
		case 4:
		    fwhole = !fwhole;
		    break;
		case 5:
		    fregexp = !fregexp;
		    break;
		case 10:
		    ui.input(motorui::text, replacement, _("Replace with: "));
		    break;
		default:
		    if(n >= 50) {
			n -= 50;

			switch(grepmode) {
			    case motorgrepper::folders:
				ifold = project.foldbegin()+n;
				
				if((is = find(gflst.begin(), gflst.end(),
				ifold->gettagname())) != gflst.end()) {
				    gflst.erase(is);
				} else {
				    gflst.push_back(ifold->gettagname());
				}

				break;

			    case motorgrepper::connected:
				i = 0;
				found = false;

            			for(ifold = project.foldbegin(); !found && (ifold != project.foldend()); ifold++)
				    if(ifold->getcontentkind() == motorfile::project)
					for(ifile = ifold->begin(); !found && (ifile != ifold->end()); ifile++)
					    if(found = i++ == n)
						break;

				if(found) {
				    if((is = find(gflst.begin(), gflst.end(),
				    ifile->getfname())) != gflst.end()) {
					gflst.erase(is);
				    } else {
					gflst.push_back(ifile->getfname());
				    }
				}

				break;
			}
		    }
	    }
	} else {
	    start = fin = true;
	}
    }

    db.close();

    if(start) {
	initmode = false;
	grepper.clear();
	currentgrep = grepper.begin();

	grepoptions = 0;
	if(fcase) grepoptions |= MG_CASESENS;
	if(fregexp) grepoptions |= MG_REGEXP;
	if(fwhole) grepoptions |= MG_WHOLEWORD;

	if(grepmode != motorgrepper::currentfile || replacemode) {
	    grepper.setadditional(gflst);
	    grepper.exec(grepmode, pattern, grepoptions);
	}
    }

    return r && db.gettree()->menu.getlastkey() != KEY_ESC;
}

bool uigrepper::go() {
    return
	grepmode == motorgrepper::currentfile ?
	    currentgo() : collectgo();
}

bool uigrepper::collectgo() {
    int n, b, citem;
    dialogbox db;
    vector<motorui::editfile>::const_iterator ir;
    bool openf = false;

    db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT,
	uiconf.getcolor(cp_menufr), TW_CENTERED, uiconf.getcolor(cp_menuhl),
	_(" Motor grepper: results ")));

    db.setbar(new horizontalbar(uiconf.getcolor(cp_menu),
	uiconf.getcolor(cp_menusel), _("Switch to"), _("New search"), 0));

    db.settree(new treeview(uiconf.getcolor(cp_menu), uiconf.getcolor(cp_menusel),
        uiconf.getcolor(cp_menu), uiconf.getcolor(cp_menu)));

    treeview &tree = *db.gettree();

    for(ir = grepper.begin(); ir != grepper.end(); ir++) {
	n = tree.addleaff(0, 0, ir-grepper.begin()+1,
	    " %s:%d ", ir->fname.c_str(), ir->y);

	if(ir == currentgrep) {
	    tree.setcur(n);
	}
    }

    for(bool fin = false; !fin; ) {
	if(!db.open(n, b, (void **) &citem)) break;

	if(!b) {
	    fin = openf = (bool) citem;
	} else {
	    initmode = fin = true;
	}
    }

    db.close();

    if(openf) {
	currentgrep = grepper.begin()+citem-1;
	ui.loadeditfile(*currentgrep);
    }

    return (db.gettree()->menu.getlastkey() != KEY_ESC) && !openf;
}

bool uigrepper::currentgo() {
    string fopt;
    int x, y;

    if(!ed.getfcount()) {
	return false;

    } else if(!(grepoptions & MG_REGEXP) && !(grepoptions & MG_WHOLEWORD) && !replacemode) {
	fopt = "c";
	if(grepoptions & MG_CASESENS) fopt += "s";

        if(ed.find(pattern.c_str(), fopt.c_str(), &x, &y)) {
	    ed.setpos(x, y);
	} else {
	    ui.log("Search string not found");
	}

    } else if(grepper.begin() == grepper.end()) {
        grepper.clear();
        grepper.exec(grepmode, pattern, grepoptions);

	if(grepper.begin() != grepper.end()) {
	    ui.loadeditfile(*grepper.begin());
    	    grepper.clear();
	}
    }

    return false;
}

bool uigrepper::replacego() {
    bool replall, fin;
    char *p;
    string np;
    vector<motorui::editfile>::const_iterator ir, pir;
    motorui::askresult ar;
    motorui::editfile ef;
    int offset;

    replall = false;

    for(ir = grepper.begin(), fin = false; ir != grepper.end() && !fin; ir++) {
	pir = ir-1;
	if((ir == grepper.begin()) || (pir->fname != ir->fname) || (pir->y != ir->y)) {
	    offset = 0;
	}

        ef = *ir;
        ef.x += offset;
	ui.loadeditfile(ef);

        if(!replall) {
            ed.startmark();
            ed.setpos(ef.x+pattern.size(), ef.y-1);
            ed.endmark();
            ed.redraw();
    	}

    	ar = replall ? motorui::yes :
            ui.ask("YNAC", _("Replace this occurence?"));
            
        switch(ar) {
    	    case motorui::all:
		replall = true;
            case motorui::yes:
		p = ed.getline(ef.y-1);
                np.assign(p, ef.x);
                np += replacement;
                np += p+ef.x+pattern.size();
		ed.putline(ef.y-1, strdup(np.c_str()));
                ed.setpos(ef.x+replacement.size(), ef.y-1);
		offset += replacement.size()-pattern.size();
		break;
            case motorui::cancel:
                fin = true;
		ui.log(_("Cancelled"));
		break;
        }

        ed.clearmark();
    }

    return !fin;
}

void uigrepper::again() {
    if(!initmode) {
	if(grepmode == motorgrepper::currentfile) {
	    currentgo();
	} else if(!replacemode) {
	    if(currentgrep+1 != grepper.end()) {
		ui.loadeditfile(*(++currentgrep));
	    }
	}
    }
}

void uigrepper::grep() {
    initmode = (replacemode || (grepmode == motorgrepper::currentfile));
    replacemode = false;

    for(bool r = true; r; ) {
	r = initmode ? init() : go();
    }
}

void uigrepper::replace() {
    replacemode = true;
    if(init()) replacego();
}
