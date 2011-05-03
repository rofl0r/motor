#include "uiregexper.h"
#include "uieditor.h"

uiregexper regexper;

uiregexper::uiregexper() {
    initialized = false;
}

uiregexper::~uiregexper() {
}

void uiregexper::setregexp(const string &aregexp) {
    regexp = aregexp;
}

void uiregexper::setexample(const string &aexample) {
    example = aexample;
}

string uiregexper::getregexp() const {
    return regexp;
}

void uiregexper::exec() {
    regex_t r;
    regmatch_t rm[64];
    int i, b, n, nmatched;
    string clipbuf, buf;
    ofstream f;

    if(!initialized) {
	db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT,
	    uiconf.getcolor(cp_menufr), TW_CENTERED,
	    uiconf.getcolor(cp_menuhl), _(" Regexper tool ")));

	db.setbar(new horizontalbar(uiconf.getcolor(cp_menufr),
	    uiconf.getcolor(cp_menusel), _("Change"), _("Copy"), _("Insert"), 0));

	db.settree(new treeview(uiconf.getcolor(cp_menu),
	    uiconf.getcolor(cp_menusel), uiconf.getcolor(cp_menuhl),
	    uiconf.getcolor(cp_menu)));

	initialized = true;
    }

    db.redraw();
    treeview &tree = *db.gettree();

    for(bool fin = false; !fin; ) {
	tree.clear();
	nmatched = 0;

	n = tree.addnode(0, 0, 0, _(" Regular expression "));
	tree.addleaf(n, 0, 0, " " + regexp + " ");
	n = tree.addnode(0, 0, 0, _(" Test string "));
	tree.addleaf(n, 0, 0, " " + example + " ");
	n = tree.addnode(0, 0, 0, _(" Match results "));

	if(!regcomp(&r, regexp.c_str(), REG_EXTENDED)) {
	    if(!regexec(&r, example.c_str(), 64, rm, 0)) {
		for(i = 0; (i < 64) && (rm[i].rm_so != -1) && (rm[i].rm_eo != -1); i++) {
		    tree.addleaff(n, 0, 0, " [%d, %d] '%s' ", rm[i].rm_so, rm[i].rm_eo,
			example.substr(rm[i].rm_so, rm[i].rm_eo-rm[i].rm_so).c_str());
		    nmatched++;
		}
	    } else {
		tree.addleaf(n, 0, 0, _(" Does not match "));
	    }
	    regfree(&r);
	} else {
	    tree.addleaf(n, 0, 0, _(" Error in the expression "));
	}

	fin = !db.open(n, b);

	if(!fin) {
	    switch(b) {
		case 0:
		    switch(n) {
			case 2:
			    ui.input(motorui::text, regexp, _("regexp: "));
			    break;
			case 4:
			    ui.input(motorui::text, example, _("test string: "));
			    break;
		    }
		    break;

		case 1:
		case 2:
		    clipbuf = "";

		    switch(n) {
			case 2: clipbuf = regexp; break;
			case 4: clipbuf = example; break;
			default:
			    if((n >= 6) && (n-6 < nmatched)) {
				clipbuf = example.substr(rm[n-6].rm_so,
				    rm[n-6].rm_eo-rm[n-6].rm_so);
			    }
			    break;
		    }

		    if(!clipbuf.empty())
		    switch(b) {
			case 1:
			    f.open(uiconf.getclipboardfname().c_str());
			    if(f.is_open()) {
			        f << clipbuf;
			        f.close();
			    }
			    break;
			case 2:
			    if(fin = ed.getfcount()) {
				ed.insert(clipbuf);
			    }
			    break;
		    }

		    break;
	    }
	}
    }

    db.close();
    ed.redraw();
}
