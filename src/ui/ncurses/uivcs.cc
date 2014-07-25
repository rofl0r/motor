#include "uivcs.h"

#include "motormisc.h"
#include "motormanager.h"

#define getcolor(c)	uiconf.getcolor(c)

uivcs uv;

uivcs::uivcs() {
}

uivcs::~uivcs() {
}

bool uivcs::enabled() {
    if(!vcs.enabled()) {
	ui.log(_("The project is not under the VCS control"));
    }

    return vcs.enabled();
}

#define makeflst(kind) \
    for(is = affect.begin(); is != affect.end(); is++) \
    if(find(ch.begin(), ch.end(), pair<string, \
    motorvcs::changekind>(*is, kind)) != ch.end()) { \
	flst.push_back(*is); \
    }

void uivcs::check() {
    bool found;
    dialogbox db;
    INT n, b, i, nodes[motorvcs::remotely_modified+1];
    string comment, buf;

    if(!enabled()) return;

    vector< pair<string, motorvcs::changekind> > ch;
    vector< pair<string, motorvcs::changekind> >::iterator ic, it;

    vector<string> affect;
    vector<string> flst;
    vector<string>::iterator is;

    vector<motorvcs::changekind> naffect;
    vector<motorvcs::changekind>::iterator ik;

    const string nodenames[motorvcs::remotely_modified+1] = {
	_(" Locally Added "),
	_(" Locally Removed "),
	_(" Locally Modified "),
	_(" Remotely Modified ")
    };

    db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT,
        getcolor(cp_menufr), TW_CENTERED,
	getcolor(cp_menuhl), _(" VCS checking results ")));

    db.settree(new treeview(getcolor(cp_menu), getcolor(cp_menusel),
        getcolor(cp_menu), getcolor(cp_menu)));

    db.setbar(new horizontalbar(getcolor(cp_menu),
        getcolor(cp_menusel), _("Mark"), _("Diff"), _("Check in"), _("Check out"), 0));

    treeview &tree = *db.gettree();
    db.addkey(' ', 0);

    for(bool fin = false; !fin; ) {
	if(tree.empty()) {
	    if((ch = vcs.getchanges()).empty()) break;

	    for(is = affect.begin(), found = false; is != affect.end() && !found; is++)
		for(ic = ch.begin(); ic != ch.end(); ic++)
		    found = ic->first == *is;
	}

        tree.clear();
	for(n = 0; n <= motorvcs::remotely_modified; nodes[n++] = 0);

	for(ic = ch.begin(); ic != ch.end(); ic++) {
	    if(!(n = nodes[ic->second])) {
		found = find(naffect.begin(), naffect.end(), ic->second) != naffect.end();
		n = nodes[ic->second] = tree.addnodef(0, getcolor(cp_menuhl),
		    NULL, " [%c]%s", found ? 'x' : ' ', nodenames[ic->second].c_str());
	    }

	    found = find(affect.begin(), affect.end(), ic->first) != affect.end();

	    tree.addleaff(n, 0, ic-ch.begin()+1,
		" [%c] %s ", found ? 'x' : ' ', ic->first.c_str());
	}

	if(!db.open(n, b, (void **) &i)) break;

        ic = ch.begin()+i-1;

	switch(b) {
	    case 0:
		if(!i) {
                    i = (INT) tree.getref(tree.getid(n));
		    ic = ch.begin()+i-1;

                    found = (ik = find(naffect.begin(), naffect.end(),
		        ic->second)) != naffect.end();

                    for(it = ch.begin(); it != ch.end(); it++) {
			if(it->second == ic->second) {
			    is = find(affect.begin(), affect.end(), it->first);

                            if(found && is != affect.end()) {
			        affect.erase(is);
                            } else if(!found && is == affect.end()) {
				affect.push_back(it->first);
                            }
                        }
                    }

                    if(found) naffect.erase(ik);
                    else naffect.push_back(ic->second);
                } else {
		    if((is = find(affect.begin(), affect.end(), ic->first)) != affect.end()) {
		        affect.erase(is);
                    } else {
		        affect.push_back(ic->first);
                    }

                    if((ik = find(naffect.begin(), naffect.end(),
                    ic->second)) != naffect.end()) {
                        naffect.erase(ik);
                    }
                }
		break;

            case 1:
                if(i)
		if(!(buf = vcs.getdiff(ic->first)).empty()) {
		    char tbuf[512];
		    sprintf(tbuf, _(" Changes made to %s "), ic->first.c_str());
		    thisui.textbox(buf, tbuf);
		}
                break;

            case 2:
		flst.clear();
		makeflst(motorvcs::locally_modified);
		makeflst(motorvcs::locally_added);
		makeflst(motorvcs::locally_removed);

		if(!flst.empty())
		    if(thisui.texteditbox(_(" Comment the changes you made "), comment)) {
			vcs.setcommitcomment(comment);
            		vcs.commit(flst);
            		tree.clear();
		    }
                break;

            case 3:
		flst.clear();
                makeflst(motorvcs::remotely_modified);

                if(!flst.empty()) {
		    vcs.update(flst);
		    tree.clear();
		}
		    // First check out remote changes

		flst.clear();
                makeflst(motorvcs::locally_modified);

		if(!flst.empty())
		    if(ui.askf("YN", _("This will cancel your local modifications. Proceed?")) == motorui::yes) {
			vcs.localunmodify(flst);
			tree.clear();
		    }
		    // Then check out the remote files to cancel
		    // local modifications if requested

                break;
	}
    }

    db.close();
}

void uivcs::tag() {
    string buf;
    if(!enabled()) return;

    if(ui.input(motorui::text, buf = "", _("release name to tag: ")) == motorui::yes) {
	vcs.tag(buf);
    }
}

void uivcs::import() {
    dialogbox db;
    vector<string> vcses;
    vector<string>::iterator iv;
    string module, repos;
    INT i, n, b, citem;
    bool start = false;

    if((vcses = manager.getvcslist()).empty()) {
	ui.log(_("No version control systems defined"));
	return;
    } else {
	iv = vcses.begin();
    }

    db.setwindow(new textwindow(0, 0, (INT) (DIALOG_WIDTH*0.9), (INT) (DIALOG_HEIGHT*0.45),
        getcolor(cp_menufr), TW_CENTERED,
	getcolor(cp_menuhl), _(" Import into VCS ")));

    db.settree(new treeview(getcolor(cp_menu), getcolor(cp_menusel),
        getcolor(cp_menu), getcolor(cp_menu)));

    db.setbar(new horizontalbar(getcolor(cp_menu),
        getcolor(cp_menusel), _("Change"), _("Import"), 0));

    module = project.getname();
    treeview &tree = *db.gettree();

    for(bool fin = false; !fin; ) {
        tree.clear();

        i = tree.addnode(0, getcolor(cp_menuhl), NULL, _(" VCS name "));
        tree.addleaf(i, 0, 1, " " + *iv + " ");

	i = tree.addnode(0, getcolor(cp_menuhl), NULL, _(" Import parameters "));
	tree.addleaff(i, 0, 2, _(" Repository : %s "), repos.c_str());
	tree.addleaff(i, 0, 3, _(" Module name : %s "), module.c_str());

	if(!db.open(n, b, (void **) &citem)) break;

	if(!b)
	switch(citem) {
	    case 1:
		if(++iv == vcses.end())
		    iv = vcses.begin();
		break;
	    case 2:
		ui.input(motorui::text, repos, _("repository: "));
		break;
	    case 3:
		ui.input(motorui::text, module, _("module name: "));
		break;
	}
	else {
	    fin = start = true;
	}
    }

    db.close();

    if(start) {
	project.setvcs(*iv);
	project.setvcsmodule(module);
	project.setvcsroot(repos);
	vcs.import();
    }
}

void uivcs::forkbranch() {
    dialogbox db;
    INT n, b, i, citem;
    string tag, destdir, buf;
    bool fcreat, firstiter;

    if(!enabled()) return;

    db.setwindow(new textwindow(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT,
        getcolor(cp_menufr), TW_CENTERED,
	getcolor(cp_menuhl), _(" Fork a branch of the program source ")));

    db.settree(new treeview(getcolor(cp_menu), getcolor(cp_menusel),
        getcolor(cp_menu), getcolor(cp_menu)));

    db.setbar(new horizontalbar(getcolor(cp_menu),
        getcolor(cp_menusel), _("Change"), _("Fork"), 0));

    treeview &t = *db.gettree();

    fcreat = firstiter = true;

    for(bool fin = false; !fin; ) {
        t.clear();

        i = t.addnode(0, getcolor(cp_menuhl), NULL, _(" Branch tag "));
        t.addleaf(i, 0, 1, " " + tag + " ");

        i = t.addnode(0, getcolor(cp_menuhl), NULL, _(" Fork options "));
	t.addleaff(i, 0, 2, _(" Create a separate project : %s "), BOOL_TO_STR(fcreat));

	if(fcreat) {
	    t.addleaff(i, 0, 3, _(" Location for the branch : %s "), destdir.c_str());
	}

	if(firstiter) {
	    db.redraw();
	    b = 0;
	    citem = 1;
	} else {
	    if(!db.open(n, b, (void **) &citem)) break;
	}

	switch(b) {
	    case 0:
		switch(citem) {
		    case 1:
			buf = tag;
			ui.input(motorui::text, tag, _("Branch tag: "));
			if(destdir.empty() || (destdir == project.getrootdir() + "-" + buf)) {
			    if(!tag.empty()) {
				destdir = project.getrootdir() + "-" + tag;
			    } else {
				destdir = "";
			    }
			}
			break;
		    case 2:
			fcreat = !fcreat;
			break;
		    case 3:
			ui.input(motorui::directory, destdir,
			    _("Target directory: "));
			break;
		}
		break;
	    case 1:
		if(tag.empty()) {
		    ui.log(_("Branch tag name must be set"));
		    continue;
		}

		if(fcreat && destdir.empty()) {
		    ui.log(_("Target directory must be set"));
		    continue;
		}

		if(fcreat && (destdir == project.getrootdir())) {
		    ui.log(_("Branch directory must not be equal to the project root"));
		    continue;
		}

		vcs.forkbranch(tag, fcreat ? destdir : "");
		fin = vcs.good();
		break;
	}

	firstiter = false;
    }

    db.close();
}

void uivcs::explore() {
}

void uivcs::merge() {
}
