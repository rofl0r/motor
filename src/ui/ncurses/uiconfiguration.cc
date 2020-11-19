#include "uiconfiguration.h"
#include "motorconfiguration.h"
#include "motormisc.h"
#include "motorproject.h"

#include <regex.h>

uiconfiguration uiconf;

uiconfiguration::uiconfiguration() {
    mcedclip = false;
    smarttab = true;
}

uiconfiguration::~uiconfiguration() {
}

int uiconfiguration::getcolor(int paircode) const {
    return find(boldcolors.begin(), boldcolors.end(), paircode) != boldcolors.end() ?
        boldcolor(paircode) : normalcolor(paircode);
}

void uiconfiguration::readthemes() {
    ifstream f;
    string buf, sub, color, sect;
    int nspos, ntpos, npos, npair, nfg, nbg, i;
    bool schemeread = false;
    
    boldcolors.clear();

    // Setting default color parameters

    init_pair(cp_input, COLOR_BLACK, COLOR_CYAN);
    init_pair(cp_bottom, COLOR_BLACK, COLOR_WHITE);
    init_pair(cp_bottomhl, COLOR_RED, COLOR_WHITE);
    init_pair(cp_menu, COLOR_BLACK, COLOR_CYAN);
    init_pair(cp_menufr, COLOR_BLACK, COLOR_CYAN);
    init_pair(cp_menusel, COLOR_WHITE, COLOR_BLACK);
    init_pair(cp_menuhl, COLOR_WHITE, COLOR_CYAN);
    init_pair(cp_debug_current, COLOR_BLACK, COLOR_CYAN);
    init_pair(cp_debug_breakpoint, COLOR_WHITE, COLOR_RED);

    // Now read the themes file

    f.open(conf.getconfigfname("themes").c_str());

    if(f.is_open()) {
        while(getconf(sect, buf, f)) {
            sub = getword(sect);
            
            if(sub == "general") {
                if(buf.substr(0, 6) == "scheme") {
                    getword(buf);
                    schemename = getword(buf);
                }
            } else if((sub == "colorscheme") && (sect == schemename)) {
                schemeread = true;

                sub = getword(buf);
                color = getword(buf);

                if(i = color.find("/")) {
                    nfg = findcolor(color.substr(0, i));
                    nbg = findcolor(color.substr(i+1));
                }

                if(sub == "input") npair = cp_input; else
                if(sub == "status_norm") npair = cp_bottom; else
                if(sub == "status_hlight") npair = cp_bottomhl; else
                if(sub == "border") npair = cp_menufr; else
                if(sub == "menu_norm") npair = cp_menu; else
                if(sub == "menu_bar") npair = cp_menusel; else
                if(sub == "menu_hlight") npair = cp_menuhl; else
                if(sub == "debug_current") npair = cp_debug_current; else
                if(sub == "debug_breakp") npair = cp_debug_breakpoint;
                else continue;

                init_pair(npair, nfg, nbg);
                if(buf == "bold") boldcolors.push_back(npair);
            } else if(schemeread) {
                break;
            }
        }
        f.close();
    }
}

void uiconfiguration::load() {
    ifstream f((conf.getmotordir() + "/ncursesuirc").c_str());
    string sect, buf;

    if(f.is_open()) {
        while(getconf(sect, buf, f)) {
	    if(sect == "lastproject") {
		lastproject = buf;
	    } else if(sect == "mcedclip") {
		mcedclip = buf == "1";
	    } else if(sect == "smarttab") {
		smarttab = buf == "1";
	    }
	}

	f.close();
    }

    readthemes();
}

void uiconfiguration::save() {
    ofstream f((conf.getmotordir() + "/ncursesuirc").c_str());

    if(f.is_open()) {
	f <<
	    "%lastproject" << endl << (project.empty() ? "" : project.getname()) << endl <<
	    "%mcedclip" << endl << (mcedclip ? "1" : "0") << endl <<
	    "%smarttab" << endl << (smarttab ? "1" : "0") << endl;

	f.close();
    }
}

string uiconfiguration::getschemename() const {
    return schemename;
}

string uiconfiguration::getclipboardfname() const {
    string r;

    if(mcedclip) {
	r = (string) getenv("HOME") + "/.cedit";
	if(access(r.c_str(), F_OK))
	    r = (string) getenv("HOME") + "/.mc/cedit";

	r += "/cooledit.clip";

    } else {
	r = conf.getmotordir() + "clipboard";

    }

    return r;
}

int uiconfiguration::getwatchlines() const {
    return 6;
}

string uiconfiguration::getlastproject() const {
    return lastproject;
}

bool uiconfiguration::getmcedclip() const {
    return mcedclip;
}

void uiconfiguration::setmcedclip(bool af) {
    mcedclip = af;
}

bool uiconfiguration::getsmarttab() const {
    return smarttab;
}

void uiconfiguration::setsmarttab(bool asmarttab) {
    smarttab = asmarttab;
}

// ----------------------------------------------------------------------------

uiconfiguration::syntaxcolorpair::syntaxcolorpair(int aid, int apair, bool abold) {
    id = aid;
    pair = apair;
    bold = abold;
}

uiconfiguration::syntaxcolorpair::~syntaxcolorpair() {
}

int uiconfiguration::syntaxcolorpair::getpairnumber() {
    return pair;
}

bool uiconfiguration::syntaxcolorpair::isbold() {
    return bold;
}

bool uiconfiguration::syntaxcolorpair::operator == (const int &aid) {
    return id == aid;
}

bool uiconfiguration::syntaxcolorpair::operator != (const int &aid) {
    return !(*this == aid);
}

// ----------------------------------------------------------------------------

uiconfiguration::syntaxitem::syntaxitem(const string &afname, int id) {
    first = afname;
    second = id;
}

bool uiconfiguration::syntaxitem::operator == (const string &afname) {
    bool ret = false;
    regex_t r;

    if(first.empty()) {
	ret = afname.empty();
    } else if(!regcomp(&r, first.c_str(), REG_EXTENDED)) {
	ret = !regexec(&r, afname.c_str(), 0, 0, 0);
	regfree(&r);
    }

    return ret;
}

bool uiconfiguration::syntaxitem::operator != (const string &afname) {
    return !(*this == afname);
}
