/*
*
* A class for diplaying and operating an advanced text editor with
* syntax highlighting, scrolling and other useful features.
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
* History of changes:
*
* 04.01.2000    development started
* 09.01.2000    the first release is ready
* 11.01.2000    one-line comments hightlight feature added
* 29.01.2000    text selecting and copying feature (cut-n'-paste)
* 01.02.2000    setpos() method added
* 03.02.2000    eddelword() method added, some minor changes made
* 06.02.2000    delmark() method added, hl_comment() minor bug fixed
* 18.02.2000    bool wrap variable added
* 11.03.2000    full tab support added
* 01.04.2000    emacs keys binding option added
* 20.04.2000    getline() & putline() methods added
* 23.04.2000    colorschemes added
* 07.05.2000    setpos() changed
* 11.05.2000    whole lines highlighing feature added
* 28.06.2000    shiftident() method added
* 10.07.2000    undo() implemented
* 25.07.2000    tab support improved
* 06.09.2000    blocks handling improved, shiftmarkedblock method added
*
*/

#include "texteditor.h"

#define CURLINE       (curfile ? (curfile->sy+curfile->y) : 0)
#define CURCOL        (curfile ? (curfile->sx+curfile->x) : 0)
#define CURSTRING     (char *) curfile->lines->at(CURLINE)
#define CSTRLEN       strlen(CURSTRING ? CURSTRING : "")
#define UPDATECURRENTLINE       { kgotoxy(x1, y1+curfile->y); showline(CURLINE, curfile->sx, x2-x1); }

#define MAX_STRLEN    10240
#define ALONE_DELIM   " ;(){}[].,:-+*/^?!=<>"
#define NONCHAR_DELIM " ;(){}[].,:-+*/^?!=<>\"'"
#define WORD_DELIM    " ,"

#define EM_TAB        2
#define EM_CTRL       4
#define EM_MANUAL     8

#define CHECKLOADED     if(!getfcount()) return;

texteditor::texteditor():
otherkeys(0), fn(-1), wrap(false), abscol(0), idle(0),
insertmode(true), undolog(true), show(true), curfile(0),
prevshift(false), smarttab(true) {

    files = new linkedlist;
    files->freeitem = &editfilefree;
}

texteditor::~texteditor() {
    delete files;
}

int texteditor::load(const string abuf, const string id) {
    int newfn = addwindow(strdup(id.c_str())), k;
    string buf = abuf;
    vector<string> lst;
    vector<string>::iterator i;

    setfnum(newfn);

    breakintolines(buf, lst, wrap ? x2-x1-1 : 0);

    curfile->lines->empty();
    for(i = lst.begin(); i != lst.end(); i++) {
	curfile->lines->add(strdup(i->c_str()));
    }
    curfile->lines->add(strdup(""));

    return newfn;
}

int texteditor::load(FILE *f, const string id) {
    int i = -1;
    struct stat st;
    char *p = 0;

    if(f)
    if(!fstat(fileno(f), &st)) {
	p = new char[st.st_size+1];
	fseek(f, 0, SEEK_SET);
	fread(p, st.st_size, 1, f);
	p[st.st_size] = 0;
	i = load(p, strdup(id.c_str()));
	delete p;
    }

    return i;
}

int texteditor::load(ifstream &f, const string id) {
    int ret, size;
    char *buf;

    f.seekg(0, ios::end);
    size = f.tellg();
    f.seekg(0, ios::beg);

    buf = new char[size+1];
    f.read(buf, size);
    buf[size] = 0;
    ret = load(buf, strdup(id.c_str()));
    delete buf;

    return ret;
}

char *texteditor::save(const char *linebreak) {
    int i;
    char *buf, *p, *prev;

    buf = p = 0;
    
    for(i = 0; i < curfile->lines->count; i++) {
	prev = p;
	p = (char *) curfile->lines->at(i);

	if(!buf) {
	    buf = (char *) malloc(strlen(p) + strlen(linebreak) + 1);
	    buf[0] = 0;
	} else {
	    buf = (char *) realloc(buf, strlen(buf) + strlen(p) + strlen(linebreak) + 1);
	}

	if(i) {
	    if(!wrap) {
		strcat(buf, linebreak);
	    } else
	    if(!prev[0] ||
	    (prev[strlen(prev)-1] != ' ') &&
	    (strlen(prev) < x2-x1-1)) {
		strcat(buf, linebreak);
	    }
	}

	while(strspn(p, " ") >= TAB_SIZE) {
	    p += TAB_SIZE;
	    strcat(buf, "\t");
	}

	if(!((i == curfile->lines->count-1) && !strcmp(p, linebreak)))
	    strcat(buf, p);
    }

    return buf;
}

int texteditor::save(FILE *f, const char *linebreak) {
    char *buf = save(linebreak);
    fwrite(buf, strlen(buf), 1, f);
    delete buf;
    modified = false;
    return 0;
}

int texteditor::save(ofstream &f, const string linebreak) {
    char *buf = save(linebreak.c_str());
    f.write(buf, strlen(buf));
    delete buf;
    f.close();
    modified = false;
    return 0;
}

int texteditor::getfnum() {
    return fn;
}

int texteditor::getfcount() {
    return files->count;
}

char *texteditor::getfid() {
    return getfid(fn);
}

char *texteditor::getfid(int fnn) {
    if(files->count) {
	editfile *ef = (editfile *) files->at(fnn);
	if(ef) return ef->id; else return 0;
    } else {
	return 0;
    }
}

void texteditor::setfid(char *id) {
    setfid(fn, id);
}

void texteditor::setfid(int fnn, char *id) {
    if(fnn < files->count) {
	editfile *ef = (editfile *) files->at(fnn);
	if(ef) ef->id = id;
    }
}

void texteditor::setfnum(int n) {
    if(n < files->count && n >= 0 && n != fn) {

	// save previous window params

	if(curfile && (fn >= 0)) curfile->modified = modified;

	// set new file number

	fn = n;
	curfile = (editfile *) files->at(fn);
	modified = curfile->modified;

	colors = colorschemes[curfile->ncolorscheme];
    }
}

int texteditor::addwindow(char *id) {
    editfile *ef = new editfile;

    ef->lines = new linkedlist;
    ef->blocks = new linkedlist;
    ef->highlines = new linkedlist;
    ef->undo = new linkedlist;

    ef->blocks->freeitem = &textblockfree;
    ef->lines->freeitem = &charpointerfree;
    ef->highlines->freeitem = &highlinefree;
    ef->undo->freeitem = &undorecordfree;

    ef->markblock = new textblock;
    memset(ef->markblock, 0, sizeof(textblock));

    ef->sx = ef->x = ef->sy = ef->y = 0;
    ef->modified = ef->markmode = ef->showmarked = false;
    ef->id = id;
    ef->ncolorscheme = 0;
    
    files->add(ef);
    return files->count-1;
}

void texteditor::modification(tundoaction action, const string &data, bool connected, int curx, int cury) {
    if(undolog && !data.empty()) {
	undorecord *ur = new undorecord;
	ur->x = curx < 0 ? CURCOL  : curx;
	ur->y = cury < 0 ? CURLINE : cury;
	ur->action = action;
	ur->data = data;
	ur->prevconnected = connected;
	curfile->undo->add(ur);
    }
    
    abscol = CURCOL;
    modified = true;
    scancomments(true);
}

void texteditor::setcoords(int nx1, int ny1, int nx2, int ny2) {
    x1 = nx1; x2 = nx2;
    y1 = ny1; y2 = ny2;
    if(curfile) setpos(CURCOL, CURLINE);
}

int texteditor::addscheme(int nc, int bc, int fbold, ...) {
    va_list ap;
    int p, nscheme = colorschemes.size();

    va_start(ap, fbold);
    colorscheme s;

    s.ncolor = nc;
    s.blockcolor = bc;
    s.bold = fbold;

    while((p = va_arg(ap, int)) != 0) s.difcolors.push_back(p);

    colorschemes.push_back(s);
    return nscheme;
}

void texteditor::addhighlight(int nscheme, string text, int color, hl_kind kind) {
    int i;
    hlight h;

    if(nscheme >= 0 && nscheme < colorschemes.size()) {
	colorscheme &s = colorschemes[nscheme];

	if(kind == h_quotes) {
	    s.synt_quote = text;
	    s.qcolor = color;

	    if((i = text.find(" ")) != -1) {
		s.synt_qescape = text.substr(i+1);
		s.synt_quote.resize(i);
	    }
	} else {
	    h.text = text;
	    h.color = color;
	    h.kind = kind;
	    s.hl.push_back(h);
	    sort(s.hl.begin(), s.hl.end());
	}
    }
}

void texteditor::addcolordif(int nscheme, int pairno) {
    if(nscheme >= 0 && nscheme < colorschemes.size()) {
	colorscheme &s = colorschemes[nscheme];
	s.difcolors.push_back(pairno);
    }
}

void texteditor::setcolorscheme(int nscheme) {
    setcolorscheme(getfnum(), nscheme);
}

void texteditor::setcolorscheme(int nfn, int nscheme) {
    editfile *f = (editfile *) files->at(nfn);

    if(f) {
	f->ncolorscheme = nscheme;
	if(nfn == getfnum()) {
	    colors = colorschemes[f->ncolorscheme];
	}
    }
}

void texteditor::addblock(int x1, int y1, int x2, int y2, int color) {
    textblock *tb = new textblock;
    tb->x1 = x1;
    tb->y1 = y1;
    tb->x2 = x2;
    tb->y2 = y2;
    tb->color = color;
    curfile->blocks->add(tb);
}

void texteditor::startmark() {
    CHECKLOADED;

    if(!curfile->markmode) {
	curfile->markmode = curfile->showmarked = true;
	curfile->markreverse = false;
	curfile->markblock->x1 = curfile->markblock->x2 = CURCOL;
	curfile->markblock->y1 = curfile->markblock->y2 = CURLINE;
	curfile->markblock->color = colors.blockcolor;
	draw();
    }
}

void texteditor::endmark() {
    CHECKLOADED;

    if(curfile->markmode) {
	marktext();
	curfile->markmode = false;
    }
}

void texteditor::marktext() {
    bool corrx, corry, sameline;
    CHECKLOADED;

    if(curfile->markreverse) {
	corrx = curfile->markblock->x2 >= CURCOL;
	corry = curfile->markblock->y2 >= CURLINE;
	sameline = curfile->markblock->y2 == CURLINE;
    } else {
	corrx = curfile->markblock->x1 <= CURCOL;
	corry = curfile->markblock->y1 <= CURLINE;
	sameline = curfile->markblock->y1 == CURLINE;
    }

    if((corry && !corrx && sameline) || !corry) {
	curfile->markreverse = !curfile->markreverse;
    }

    if(curfile->markreverse) {
	curfile->markblock->x1 = CURCOL;
	curfile->markblock->y1 = CURLINE;
    } else {
	curfile->markblock->x2 = CURCOL;
	curfile->markblock->y2 = CURLINE;
    }

    draw();
    updatecursor();
}

void texteditor::copymark(FILE *f) {
    int i;
    CHECKLOADED;

    for(i = curfile->markblock->y1; i <= curfile->markblock->y2; i++) {

	if(i == curfile->markblock->y1 && curfile->markblock->y1 == curfile->markblock->y2) {

	    char *p = strdup((char *) curfile->lines->at(i) + curfile->markblock->x1);
	    p[curfile->markblock->x2-curfile->markblock->x1] = 0;
	    fprintf(f, "%s", p);
	    free(p);
	
	} else if(i == curfile->markblock->y1) {

	    fprintf(f, "%s\n", (char *) curfile->lines->at(i) + curfile->markblock->x1);

	} else if(i == curfile->markblock->y2) {

	    if(curfile->markblock->x2) {
		char *p = strdup((char *) curfile->lines->at(i));
		p[curfile->markblock->x2] = 0;
		fprintf(f, "%s", p);
		if(!CURCOL) fprintf(f, "\n");
		free(p);
	    }
	    
	} else {

	    fprintf(f, "%s\n", (char *) curfile->lines->at(i));
	    
	}
    }
}

void texteditor::copymark(char *p, int maxlen) {
}

void texteditor::delmark() {
    CHECKLOADED;

    int i, newcol, newrow, line = 0;
    char *c, *p, *sl, *el;
    string deltext;
    textblock *mb = curfile->markblock;

    if(!mb->x1 && !mb->x2 && !mb->y1 && !mb->y2) return;

    for(i = mb->y1; i <= mb->y2; i++) {
	c = (char *) curfile->lines->at(i-line);
	
	if((i == mb->y1) && (i == mb->y2)) {

	    deltext = c+mb->x1;
	    deltext.resize(mb->x2-mb->x1);

	    p = new char[strlen(c)];
	    strncpy(p, c, newcol = mb->x1);
	    strcpy(p + mb->x1, c + mb->x2);
	    p[strlen(c) - mb->x2 + mb->x1] = 0;
	    curfile->lines->replace(newrow = i, p);
	    
	} else if(i == mb->y1) {

	    deltext = c+mb->x1;
	
	    sl = strdup(c);
	    sl[mb->x1] = 0;
	    
	} else {
	    char *lch = (char *) curfile->lines->at(i-line);
	    int li = strlen(lch);

	    deltext += "\n";
	    if(i == mb->y1) deltext += lch+mb->x1; else deltext += lch;
	
	    if(i == mb->y2) {
		deltext.resize(deltext.size()-strlen(lch)+mb->x2);
		el = strdup(c + mb->x2);
		p = new char[strlen(sl)+strlen(el)+1];
		strcpy(p, sl);
		strcat(p, el);
		curfile->lines->remove(i-line);
		curfile->lines->replace(newrow = i-line-1, p);
		newcol = strlen(sl);
		free(sl);
		free(el);
	    } else {
		curfile->lines->remove(i-line);
		line++;
	    }
	}
    }

    modification(udelblock, deltext, false, mb->x1, mb->y1);
    memset(curfile->markblock, 0, sizeof(textblock));
    setpos(newcol, newrow);
    draw();
}

void texteditor::clearmark() {
    CHECKLOADED;
    memset(curfile->markblock, 0, sizeof(textblock));
    draw();
}

void texteditor::insert(FILE *f) {
    struct stat sb;
    int fsize;
    char *buf;

    if(f) {
	fseek(f, 0, SEEK_SET);
	fstat(fileno(f), &sb);
	buf = new char[(fsize = sb.st_size)+1];
	fread(buf, fsize, 1, f);
	buf[fsize] = 0;
	insert(buf);
	delete buf;
    }
}

void texteditor::insert(const string abuf) {
    CHECKLOADED;

    string sbuf;
    vector<string> lst;
    vector<string>::iterator is;

    sbuf = abuf;

    if(sbuf.find_first_of("\n\t") != -1) {
	breakintolines(sbuf, lst, 0);
	for(sbuf = "", is = lst.begin(); is != lst.end(); is++) {
	    sbuf += *is + "\n";
	}
    }

    if(!sbuf.empty()) {
	char *sl = strdup(CURSTRING), *el = strdup(CURSTRING+CURCOL), buf[1024], *s;
	const char *curpos = sbuf.c_str();
	bool firstpass = true;
	int line = 0;

	sl[CURCOL] = 0;

	while(1) {
	    if(!firstpass) {
		if(curpos = strchr(curpos, '\n')) curpos++;
		else break;
	    }

	    strncpy(buf, curpos, 1024);
	    if(s = strchr(buf, '\n')) *s = 0;
	    curpos += strlen(buf);

	    if(!line++) {
		strinsert(buf, 0, sl);
		if(!*curpos) strcat(buf, el);
		curfile->lines->replace(CURLINE, strdup(buf));
	    } else {
		if(!*curpos) strcat(buf, el);
		curfile->lines->insert(CURLINE+line, strdup(buf));
	    }
	    
	    firstpass = false;
	}
    
	delete el;
	delete sl;

	modification(uinsblock, sbuf);
    }
}

void texteditor::sethlcolor(int n) {
    int at = colors.bold;
    
    if(!n) n = colors.ncolor;

    if(::find(colors.difcolors.begin(), colors.difcolors.end(), n) !=
    colors.difcolors.end())
	at = !at;

    attrset(at ? boldcolor(n) : normalcolor(n));
}

void texteditor::draw_print(char *buf, int bcolor, int distance) {
    if(outx + strlen(buf) > distance) buf[distance-outx] = 0;
    if(buf[0]) {
	sethlcolor(bcolor);
	printw("%s", buf);
	outx += strlen(buf);
	buf[0] = 0;
    }
}

int dstralone(const char *buf, const char *startword, int wordlen, const char *delim) {
    int leftdelim = 0, rightdelim = 0;
    const char *si;

    for(si = startword-1; si != buf && *si < 32; si--);
    if(si >= buf) leftdelim = (strchr(delim, *si) != 0); else leftdelim = 1;

    for(si = startword + wordlen; *si && *si < 32; si++);
    if(*si) rightdelim = (strchr(delim, *si) != 0); else rightdelim = 1;

    return leftdelim && rightdelim;
}

void texteditor::scancomments(bool visible) {
    int sl, el, i;
    const char *rsub, *lsub;
    hl_kind hk;

    curfile->blocks->empty();
    
    if(visible) {
	sl = curfile->sy;
	el = curfile->sy+y2-y1;
	if(el > curfile->lines->count) el = curfile->lines->count;
    } else {
	sl = 0;
	el = curfile->lines->count;
    }

    vector<hlight>::iterator hi = ::find(colors.hl.begin(), colors.hl.end(), h_comment);

    if(hi != colors.hl.end()) {
	string lc, rc, comment = hi->text;
	struct textblock *tb = 0;

	lc = getword(comment, " ");
	rc = getword(comment, " ");

	for(i = sl; i < el; i++) {
	    char *p = (char *) curfile->lines->at(i);
	    const char *sub = p;

	    while(1) {
		lsub = strqstr(sub, lc.c_str(), colors.synt_quote.c_str());

		if(tb) rsub = strstr(sub, rc.c_str());
		else rsub = strqstr(sub, rc.c_str(), colors.synt_quote.c_str());

		if(rsub && (((rsub < lsub) && lsub) || !lsub)) {
		    sub = rsub;
		    if(!tb) {
			tb = new textblock;
			tb->color = hi->color;
			tb->y1 = 0;
			tb->x1 = 0;
		    }
		    tb->y2 = i;
		    tb->x2 = sub-p+rc.size();
		    curfile->blocks->add(tb);
		    tb = 0;
		    sub += rc.size();
		} else if(lsub) {
		    sub = lsub;
		    if(!tb) {
			tb = new textblock;
			tb->color = hi->color;
			tb->y1 = i;
			tb->x1 = sub-p;
		    }
		    sub += lc.size();
		} else if(lsub && rsub) {
		    if(!tb) {
			tb = new textblock;
			tb->color = hi->color;
		    }
		    tb->y1 = tb->y2 = i;
		    tb->x1 = p-lsub;
		    tb->x2 = p-rsub;
		    curfile->blocks->add(tb);
		    tb = 0;
		} else {
		    break;
		}
	    }
	}

	if(tb) {
	    tb->y2 = i+1;
	    tb->x2 = 0;
	    curfile->blocks->add(tb);
	}
    }
}

int texteditor::hl_comment(char *cp, char *txt, int color) {
    int r;
    const char *p;

    r = 0;
    if(p = strqstr(cp, txt, "\"'"))
	r = hl_comment(cp, p-cp, strlen(cp), color);

    return r;
}

int texteditor::hl_comment(char *cp, int st, int pend, int color) {
    int i, delcount, r;
    char ins[5] = "\001 ";
    int origclr = -1;

    delcount = r = 0;

    if(color && (st <= strlen(cp)) && (pend-st > 0)) {
/// !!!         for(i = 0; (i <= pend) && (i < strlen(cp)); i++)

	for(i = 0; (i <= pend) && (i < strlen(cp)); i++) {
	    switch(cp[i]) {
		case 1: origclr = cp[i+++1]; break;
		case 2: origclr = -1; break;
	    }
	}

	if(pend > strlen(cp))
	    pend = strlen(cp);

	if(cp[pend] != 2) {
	    strinsert(cp, pend, "\002");
	    r++;
	}

	if(origclr != -1) {
	    ins[1] = origclr;
	    strinsert(cp, pend+1, ins);
	    r += 2;
	}

	for(i = st; (i < pend) && (i < strlen(cp)); i++) {
	    switch(cp[i]) {
		case 1:
		    strcut(cp, i--, 2);
		    delcount += 2;
		    pend -= 2;
		    break;
		case 2:
		    strcut(cp, i--, 1);
		    delcount++;
		    pend--;
		    break;
	    }
	}

	ins[1] = color;
	strinsert(cp, st, ins);
	r += 2;
    }

    return r-delcount;
}

int texteditor::count_clrcodes(char *cp, int pos) {
    int i, j, k;
    j = k = 0;
    
    for(i = 0; i < strlen(cp) && j < pos; i++) {
	if(cp[i] == 1) {
	    k++;
	    if(i++ < strlen(cp)) k++;
	} else if(cp[i] == 2) k++; else j++;
    }

    return k;
}

void texteditor::showline(int ln, int startx, int distance, int extrax) {
    if(!show) return;

    int i, n, inscount, bcolor, sxinscount, printed, j, lastoccur, q, eolstart, npos, offs;
    char *cs, *sr, *nr, *r, ins[3] = "\001 ";

    vector<int> layout;
    vector<int>::iterator iq;
    vector<hlight>::iterator hi;

    const char *p;

    if(!(cs = (char *) curfile->lines->at(ln))) return;
    char cp[i = (strlen(cs)+1)*4];
    char buf[i];

    eolstart = i;

    strcpy(cp, cs);
    buf[0] = 0;
    inscount = sxinscount = bcolor = 0;

    highline *hline = (highline *) curfile->highlines->find(&(i = ln+1), &findhighline);
    
    if(hline) {
	ins[1] = hline->color;
	strinsert(cp, 0, ins);
	strcat(cp, "\002");
    } else {
	if(strlen(cp))
	for(hi = colors.hl.begin(); hi != colors.hl.end(); hi++) {
	    ins[1] = hi->color;
	    p = cp;
	    lastoccur = 0;

	    switch(hi->kind) {
		case h_alone:
		    for(sr = r = strdup(hi->text.c_str()); r && r != sr+hi->text.size(); ) {
			if(nr = strchr(r, ';')) {
			    *nr = 0;
			    nr++;
			}

			if(!strlen(r)) {
			    r = nr;
			    continue;
			}

			p = cp;
			lastoccur = 0;
			
			while(p = strqstr(p+lastoccur, r, colors.synt_quote.c_str(), colors.synt_qescape.c_str())) {
			    if(eolstart) eolstart += lastoccur;
			    if(p-cp > eolstart) {
				r = 0;
				break;
			    }
			    
			    lastoccur = strlen(r);
			    if(dstralone(cp, p, lastoccur, ALONE_DELIM)) {
				strinsert(cp, p-cp+lastoccur, "\002");
				strinsert(cp, p-cp, ins);
				inscount++;
				lastoccur += 3;
			    }
			}

			r = nr;
		    }
		    
		    free(sr);
		    break;

		case h_symbol:
		    layout = getsymbolpositions(string(cp).substr(0, eolstart),
			hi->text, colors.synt_quote, colors.synt_qescape);

		    for(offs = 0, iq = layout.begin(); iq != layout.end(); iq++) {
			offs += hl_comment(cp, *iq+offs, *iq+offs+1, hi->color);
		    }
		    break;

		case h_block:
		    while(p = strqpbrk(cp, p-cp+lastoccur, hi->text.c_str(),
		    colors.synt_quote.c_str(), colors.synt_qescape.c_str())) {
			if(eolstart) eolstart += lastoccur;
			if(p-cp > eolstart) break;
			lastoccur = strspn(p, hi->text.c_str());
			if(dstralone(cp, p, lastoccur, ALONE_DELIM)) {
			    strinsert(cp, p-cp+lastoccur, "\002");
			    strinsert(cp, p-cp, ins);
			    lastoccur += 3;
			}
		    }
		    break;
		    
		case h_eol:
		    if((npos = find_quoted(p, hi->text, 0,
		    colors.synt_quote, colors.synt_qescape)) != -1)
			hl_comment(cp, eolstart = npos, strlen(cp),
			hi->color);
		    
		    break;
	    }
	}

	// Quotes highlight ...

	if(!colors.synt_quote.empty()) {
	    bool qst;

	    layout = getquotelayout(string(cp).substr(0, eolstart),
		colors.synt_quote, colors.synt_qescape);

	    for(qst = false, offs = 0, iq = layout.begin(); iq != layout.end(); iq++) {
		qst = !qst;

		if(!qst) {
		    offs += hl_comment(cp, *(iq-1)+offs,
			*iq+offs+1, colors.qcolor);
		}
	    }

	    if(qst) {
		hl_comment(cp, *(layout.end()-1)+offs, strlen(cp), colors.qcolor);
	    }
	}

	// Blocks ...

	for(i = 0; i < curfile->blocks->count + 1; i++) {
	    textblock *tb;

	    if(i == curfile->blocks->count) {
		if(curfile->showmarked) {
		    tb = curfile->markblock;
		} else break;
	    } else {
		tb = (textblock *) curfile->blocks->at(i);
	    }
	
	    if(ln >= tb->y1 && ln <= tb->y2) {
		q = strlen(cp);

		if(ln == tb->y1 && tb->y1 == tb->y2) {
		    n = count_clrcodes(cp, tb->x1) + tb->x1;
		    q = count_clrcodes(cp, tb->x2) + tb->x2;
		} else if(ln == tb->y1) {
		    n = count_clrcodes(cp, tb->x1) + tb->x1;
		} else if(ln == tb->y2) {
		    n = j = 0;
		    q = count_clrcodes(cp, tb->x2) + tb->x2;
		} else n = j = 0;

		if(!(tb->x1 == tb->x2 && tb->y1 == tb->y2)) {
		    hl_comment(cp, n, q, tb->color);
		}
	    }
	}
    }

    // let's count the amount of color codes inserted
    // before start X position (startx variable)

    sxinscount = count_clrcodes(cp, startx);

    for(i = 0; i < startx+sxinscount && i < strlen(cp)+inscount*3; i++) {
	if(cp[i] == 1) bcolor = cp[++i]; else
	if(cp[i] == 2) bcolor = 0;
    }

    for(i = startx+sxinscount, n = 0, outx = 0; i < strlen(cp); i++) {
	if(cp[i] == 1) {
	    draw_print(buf, bcolor, distance);
	    bcolor = cp[++i];
	    n = 0;
	} else if(cp[i] == 2) {
	    draw_print(buf, bcolor, distance);
	    n = bcolor = 0;
	} else {
	    buf[n++] = cp[i];
	    buf[n] = 0;
	}
    }

    draw_print(buf, bcolor, distance);
    if(!hline) sethlcolor(0);

    printed = strlen(cs)-startx;
    if(printed < 0) printed = 0; else
    if(printed > distance) printed = distance;
    mvhline(y1+ln-curfile->sy, x1+extrax+printed, ' ', distance-printed);
}

void texteditor::draw(int fromline) {
    int k;

    if(show) {
	if(curfile->lines) {
	    for(k = curfile->sy+fromline; k < curfile->lines->count && k < y2-y1+curfile->sy; k++) {
		kgotoxy(x1, k-curfile->sy+y1);
		showline(k, curfile->sx, x2-x1);
	    }

	    if(k < y2-y1+curfile->sy) {
		sethlcolor(colors.ncolor);
		for(; k < y2-y1+curfile->sy; k++) mvhline(k-curfile->sy+y1, x1, ' ', x2-x1);
	    }
	}

	refresh();
    }
}

void texteditor::draw() {
    if(active && curfile) {
	if(curfile->lines) scancomments(true);
	draw(0);
    }
}

bool texteditor::endofline() {
    return CURCOL == CSTRLEN;
}

const char texteditor::currentchar() {
    char *p = CURSTRING;
    return p[CURCOL];
}

void texteditor::updatecursor() {
    if(active && curfile) {
	if(curfile->y >= y2-y1) curfile->y = y2-y1-1;
	if(curfile->x >= x2-x1) curfile->x = x2-x1-1;
	kgotoxy(x1+curfile->x, y1+curfile->y);
    }
}

bool texteditor::fix_x(bool tab) {
    int osx = curfile->sx, clen = CSTRLEN;

    if(CURCOL > clen) {
	if(clen-curfile->sx < 0) {
	    curfile->sx = CSTRLEN/(x2-x1-1);
	}
	curfile->x = CSTRLEN-curfile->sx;
    } else {
	if(abscol-curfile->sx > x2-x1-1) setpos(abscol, CURLINE);
    }

    if(tab) {
	int rm = rtabmargin(true, CURCOL, CURSTRING);
	char *p = CURSTRING;
	
	if(p[CURCOL-1] == ' ')
	if(strspn(p+CURCOL, " ") >= rm-CURCOL)
	if(CURCOL != ltabmargin(true, rm, p)) {
	    //if(rm <= curfile->sx+x2-x1) curfile->x = rm-curfile->sx;
	    curfile->x += rm-curfile->x;
	}
    }

    return osx != curfile->sx;
}

void texteditor::eddel(bool usetabs) {
    char *p = CURSTRING;
    int nextlen, todelete = 1, rm;
    string deltext;
    
    if(p) {
	if(CURCOL < strlen(p)) {
	    if(usetabs && ((rm = rtabmargin(true, CURCOL, CURSTRING)) != -1)) {
		deltext.append(todelete = rm-CURCOL, ' ');
	    } else {
		deltext = *(p+CURCOL);
	    }

	    modification(udelchar, deltext);
	    strcut(p, CURCOL, todelete);

	    UPDATECURRENTLINE;
	    updatecursor();
//          showline(CURLINE, CURCOL, x2-x1-curfile->x, curfile->x);
	    
	} else {
	    char *next = (char *) curfile->lines->at(CURLINE+1);
	    int able = x2-x1-strlen(p);

	    if(next) {
		if(wrap && (able < strlen(next))) {
		    char *anext = strdup(next), *asub, *atsub;
		    anext[able] = 0;

		    if(asub = strpbrk(anext, WORD_DELIM)) {
			for(; atsub = strpbrk(asub+1, WORD_DELIM); asub = atsub);
			char *newline = new char[strlen(p)+asub-anext+2];
			strcpy(newline, p);
			strncat(newline, next, asub-anext+1);
			strcut(next, 0, asub-anext+1);
			curfile->lines->replace(CURLINE, newline);
		    }

		    delete anext;
		} else {
		    if(next) nextlen = strlen(next); else nextlen = 0;
		    char *newline = new char[nextlen+strlen(p)+1];

		    sprintf(newline, "%s%s", p, nextlen ? next : "");
		    curfile->lines->replace(CURLINE, newline);
		    curfile->lines->remove(CURLINE+1);
		    shiftmarkedblock(-1);
		}
	    }

	    modification(udelchar, "\n");
	    draw(curfile->y);
	}

	updatecursor();
    }
}

void texteditor::edbackspace() {
    int i, bc;
    
    if(CURCOL) {
	bool spacetoend = endofline() && !currentchar();
	bool curspace = isspace(currentchar());
	
	edmove(KEY_LEFT);
	
	if(spacetoend) {
	    bc = CSTRLEN-CURCOL;
	    for(i = 0; i < bc; i++) eddel(false);
	} else {
	    if(!curspace && isspace(currentchar())) {
		eddelword();
	    } else {
		eddel();
	    }
	}
    } else if(CURLINE) {
	edmove(KEY_UP);
	edmove(KEY_END);
	eddel();
    }
}

void texteditor::eddelword() {
    char *p = CURSTRING, *e;
    string deltext, n;
    int count;

    if(!strlen(p)) {
	eddelline();
    } else if(CURCOL == strlen(p)) {
	if(CURLINE < curfile->lines->count-1) {
	    eddel();
	    if(currentchar() == ' ') eddelword();
	}
    } else {
	n = p;
	deltext = n.substr(CURCOL);
	    
	if(currentchar() == ' ') {
	    count = strspn(p+CURCOL, " ");
	    n.replace(CURCOL, count, "");
	} else {
	    n = p;
	    if(!(e = strpbrk(&p[CURCOL], NONCHAR_DELIM))) e = p + strlen(p);

	    if((count = e-p-curfile->sx-curfile->x)) {
		n.replace(CURCOL, count, "");
	    } else {
		count += strspn(n.substr(CURCOL).c_str(), NONCHAR_DELIM);
		n.replace(CURCOL, strspn(n.c_str()+CURCOL, NONCHAR_DELIM), "");
	    }
	}

	deltext.resize(count);
	curfile->lines->replace(CURLINE, strdup(n.c_str()));
	modification(udelchar, deltext);
	draw(curfile->y);
	updatecursor();
    }
}

void texteditor::eddelline() {
    char *p = (char *) curfile->lines->at(CURLINE);
    string deltext = (string) p + "\n";

    if(CURLINE+1 < curfile->lines->count) {
	curfile->lines->remove(CURLINE);

	if(!curfile->lines->count) {
	    curfile->sy = curfile->sx = curfile->y = curfile->x = 0;
	    char *p = strdup("");
	    curfile->lines->add(p);
	} else if(CURLINE >= curfile->lines->count) {
	    edmove(KEY_UP);
	}
    } else {
	char *p = strdup("");
	curfile->lines->replace(curfile->lines->count-1, p);
    }

    shiftmarkedblock(-1);
    modification(udelchar, deltext, false, 0);
    edmove(KEY_HOME);
    abscol = 0;
    draw(curfile->y);
    updatecursor();
}

void texteditor::edenter(bool countspaces) {
    char *p = CURSTRING, *r;
    string spaceins;
    
    if(wrap) strimtrail(p);
    
    int oldsx = curfile->sx;
    int spacecount = strspn(p, " ");
    int nextlen = CSTRLEN-CURCOL+spacecount+1;
    
    char *nextstr = (char *) malloc(nextlen < 1 ? 1 : nextlen);
    if(CURCOL < spacecount) spacecount = 0;

    if(!countspaces) spacecount = 0;
    r = strlen(p) > CURCOL ? p+CURCOL : p+strlen(p);
    sprintf(nextstr, "%-*s%s", spacecount, "", r);

    curfile->lines->insert(CURLINE+2, nextstr);

    modification(uinschar, "\n");
    spaceins.append(spacecount, ' ');
    modification(uinschar, spaceins);
    
    curfile->sx = p[CURCOL] = 0;
    curfile->x = spacecount;

    if(curfile->x > x2-x1) {
	curfile->sx = curfile->x-(x2-x1);
	curfile->x -= curfile->sx;
    }

    // Shift the marked block down if ENTER was
    // pressed on the line above it

    shiftmarkedblock(1);

    if(curfile->y+1 < y2-y1) {
	if(curfile->sx != oldsx) draw(); else draw(curfile->y);
    }

    abscol = CURCOL;
    edmove(KEY_DOWN);
}

void texteditor::edmove(int k, int options) {
    int i, lm;
    bool fdraw = false, acted;
    bool ctrlpressed = (options & EM_CTRL) && (getctrlkeys() & CONTROL_PRESSED);
    bool shiftpressed = (getctrlkeys() & SHIFT_PRESSED);
    char *p = CURSTRING;

    if(options & EM_MANUAL) {
	if(shiftpressed != prevshift) {
	    if(shiftpressed && !curfile->markmode) startmark();
	    if(!shiftpressed && curfile->markmode) endmark();
	}
	prevshift = shiftpressed;
    }

    if(curfile->lines->count) {
	switch(k) {
	    case KEY_UP:
		if(curfile->y) {
		    curfile->y--;
		} else if(CURLINE) {
		    i = CURLINE-1;
		    // sy -= (y2-y1)/2;
		    curfile->sy--;
		    if(curfile->sy < 0) curfile->sy = 0;
		    curfile->y = i-curfile->sy;
		    fdraw = true;
		}

		setpos(abscol, CURLINE);
		if(fix_x(options & EM_TAB) || fdraw) draw();
		updatecursor();
		break;

	    case KEY_DOWN:
		if(CURLINE < curfile->lines->count-1) {
		    if(curfile->y+1 < y2-y1) {
			curfile->y++;
		    } else {
			i = CURLINE+1;
			// sy += (y2-y1)/2;
			curfile->sy++;
			if(curfile->lines->count-curfile->sy < y2-y1) curfile->sy = curfile->lines->count-y2+y1;
			curfile->y = i-curfile->sy;
			fdraw = true;
		    }

		    setpos(abscol, CURLINE);
		    if(fix_x(options & EM_TAB) || fdraw) draw();
		    updatecursor();
		}
		break;

	    case KEY_LEFT:
		if(ctrlpressed) {
		    char *p = CURSTRING, *r, *s = p+CURCOL;

		    if(p == s) r = 0; else {
			for(r = s; (r != p) && !strchr(NONCHAR_DELIM, *r); r--);
			for(; (r != p) && strchr(NONCHAR_DELIM, *r); r--);
			for(; (r != p) && !strchr(NONCHAR_DELIM, *r); r--);

			if(strchr(NONCHAR_DELIM, *r)) {
			    if(r == p) r = 0; else r++;
			}
		    }
		    
		    if(r) {
			if((curfile->x -= s-r) < 0) {
			    curfile->sx += curfile->x;
			    curfile->x = 0;
			    draw();
			    updatecursor();
			}
		    } else if(CURLINE) {
			setpos(strlen((char *) curfile->lines->at(CURLINE-1)), CURLINE-1);
		    }
		} else {
		/*
		    if(CURCOL && (options & EM_TAB) && ((lm = ltabmargin(true, -1, CURSTRING)) != -1)) {
			setpos(lm, CURLINE);
		    } else
		*/
		    if(curfile->x) {
			curfile->x--;

			char *p = CURSTRING;
			
			if(CURCOL && (options & EM_TAB))
			if((lm = ltabmargin(true, CURCOL, p)) != -1)
			if(CURCOL+1 != lm)
			if(currentchar() == ' ') setpos(lm, CURLINE);
		    } else if(curfile->sx) {
			i = CURCOL;
			curfile->sx -= (x2-x1)/3;
			if(curfile->sx < 0) curfile->sx = 0;
			curfile->x = i-curfile->sx-1;
			draw();
		    } else if(CURLINE) {
			edmove(KEY_UP);
			edmove(KEY_END);
		    }
		}            
		break;

	    case KEY_RIGHT:
		if(ctrlpressed) {
		    char *p = CURSTRING+CURCOL+1, *r = 0;

		    if(*(CURSTRING+CURCOL)) {
			if(r = strpbrk(p, NONCHAR_DELIM))
			for(; *r && strchr(NONCHAR_DELIM, *r); r++);
		    }

		    if(r) {
			curfile->x += r-p+1;
		    } else {
			if(endofline() && (CURLINE < curfile->lines->count-1)) {
			    setpos(0, CURLINE+1);
			} else {
			    curfile->x = CSTRLEN;
			}
		    }
		} else {
		    if(CSTRLEN > CURCOL) {
			curfile->x++;
			fix_x(options & EM_TAB);
		    } else if(CURLINE < curfile->lines->count-1) {
			edmove(KEY_DOWN);
			edmove(KEY_HOME);
		    }
		}
		break;
		
	    case KEY_HOME:
		i = curfile->sx;

		if(ctrlpressed) {
		    curfile->y = 0;
		    curfile->sx = curfile->x = 0;
		} else if(curfile->x) {
		    curfile->sx = curfile->x = 0;
		}

		if(curfile->sx < i) draw();
		updatecursor();
		break;

	    case KEY_END:
		if(ctrlpressed) {
		    i = curfile->sx;
		    curfile->y = y2-y1-1;
		    curfile->sx = curfile->x = 0;
		    
		    if(CURLINE >= curfile->lines->count) {
			curfile->y = curfile->lines->count-curfile->sy-1;
		    }

		    if(i) draw();
		} else if(CURCOL != CSTRLEN) {
		    setpos(CSTRLEN, CURLINE);
		}

		updatecursor();
		break;
	    
	    case KEY_PPAGE:
		if(ctrlpressed) {
		    setpos(0, 0);
		    abscol = 0;
		} else if(CURLINE) {
		    if(curfile->y-y2-y1 < 0 && !curfile->sy) curfile->y = 0; else {
			curfile->sy = curfile->sy-y2+y1;
			if(curfile->sy < 0) curfile->sy = 0;
		    }

		    fix_x(options & EM_TAB);
		    draw();
		    updatecursor();
		}
		break;

	    case KEY_NPAGE:
		if(ctrlpressed) {
		    setpos(0, curfile->lines->count-1);
		    abscol = 0;
		} else if(CURLINE != curfile->lines->count) {
		    if(curfile->sy + y2-y1 == curfile->lines->count) curfile->y = y2-y1-1; else {
			curfile->sy += y2-y1;
			if(curfile->sy+y2-y1 > curfile->lines->count) curfile->sy = curfile->lines->count-y2+y1;
			if(curfile->sy < 0) {
			    curfile->sy = 0;
			    curfile->y = curfile->lines->count-1;
			}
		    }

		    fix_x(options & EM_TAB);
		    draw();
		}
		break;
	}
    }

    if(curfile->x >= x2-x1)
    switch(k) {

    case KEY_RIGHT:
    case KEY_LEFT:

	i = CURCOL;
	curfile->sx += (x2-x1)/3;
	curfile->x = i-curfile->sx;
	draw();
	updatecursor();
	break;
    }

    if(options & EM_MANUAL)
    switch(k) {
	case KEY_LEFT:
	case KEY_RIGHT:
	case KEY_HOME:
	case KEY_END: abscol = CURCOL; break;
	default: if(abscol <= 0) abscol = CURCOL; break;
    }

    if(options & EM_MANUAL) {
	if(curfile->markmode) marktext();
    }
}

void texteditor::inschar(int k) {
    if(k == '\t') {

	int rm = rtabmargin(true, CURCOL, CURSTRING)-CURCOL, i;
	if(rm < 0) rm = rtabmargin(true, CURCOL)-CURCOL;
	for(i = 0; i < rm; i++) inschar(' ');
	
    } else {
    
	char *p = CURSTRING;
	int len = strlen(p);
	char *n = (char *) malloc(len+5);
	char np[2];

	strcpy(n, p);
	sprintf(np, "%c", k);
	strinsert(n, CURCOL, np);

	if(wrap && strlen(n) > x2-x1-1) {
	    char *sub = strpbrk(n, WORD_DELIM), *osub = 0, *sep;

	    if(sub) {
		while((osub = strpbrk(sub+1, WORD_DELIM)) &&
		(strspn(osub, WORD_DELIM) < strlen(osub))) sub = osub;
	    } else {
		sub = n+strlen(n)-2;
	    }

	    char sins[2] = " ";
	    sins[0] = *sub;
	    strinsert(n, sub-n, sins);
	    sub++;
	    *sub = 0;
	    sep = sub+1;

	    curfile->lines->insert(CURLINE+2, strdup(sep));
	    curfile->lines->replace(CURLINE, n);

	    modification(uinschar, np);

	    if(CURCOL == x2-x1-1) setpos(strlen(sep), CURLINE+1); else
	    if(CURCOL > strlen(n)) setpos(CURCOL-strlen(n)+1, CURLINE+1);
	    else edmove(KEY_RIGHT);
    
	    draw();

	} else {
	    curfile->lines->replace(CURLINE, n);
	    modification(uinschar, np);

	    UPDATECURRENTLINE;
	    edmove(KEY_RIGHT, 0);
	}

	updatecursor();
	abscol = CURCOL;
    }
}

void texteditor::setpos(int col, int line) {
    bool drawneeded = false;

    CHECKLOADED;

    if(line >= curfile->lines->count)
	line = curfile->lines->count-1;

    if(line < 0)
	line = 0;

    if((line >= curfile->sy) && (line < curfile->sy+y2-y1)) {
	curfile->y = line-curfile->sy;
    } else {
	if((curfile->sy = line-(y2-y1)/2) < 0) curfile->sy = 0;
	if(curfile->sy+y2-y1-1 > curfile->lines->count) {
	    curfile->sy = curfile->lines->count-y2+y1;
	}

	if(curfile->sy < 0) curfile->sy = 0;
	curfile->y = line-curfile->sy;
	drawneeded = true;
    }

    if((col >= curfile->sx) && (col < curfile->sx+x2-x1)) {
	curfile->x = col-curfile->sx;
    } else {
	char *p = CURSTRING;
	if(col > strlen(p)) col = strlen(p);
	curfile->sx = 0;

	if((curfile->x = col) > x2-x1-1) {
	    curfile->sx = curfile->x-(x2-x1)/2;
	    curfile->x -= curfile->sx;
	}
	drawneeded = true;
    }

    if(drawneeded) draw();
    updatecursor();
}

void texteditor::getpos(int *col, int *line) {
    if(col) *col = CURCOL;
    if(line) *line = CURLINE;
}

int texteditor::open() {
    int k, l, go;

    if((x2-x1 < 2) || (y2-y1 < 2)) {
	return 0;
    }

    if(fn < 0 || fn > files->count) setfnum(0);

    active = true;

    if(curfile) if(curfile->lines) {
	if(!curfile->lines->count) {
	    char *p = strdup("");
	    curfile->lines->add(p);
	}

	if(CURCOL > CSTRLEN) fix_x(true);
	draw();
	updatecursor();
    }

    abscol = CURCOL;

    while(active && files->count) {
	refresh();
	if(idle) go = keypressed(); else go = 1;

	if(go) {
	    k = getkey();
	    if(emacs) k = emacsbind(k);

	    switch(k) {
		case KEY_UP:
		case KEY_DOWN:
		case KEY_LEFT:
		case KEY_RIGHT:
		case KEY_HOME:
		case KEY_END:
		case KEY_PPAGE:
		case KEY_NPAGE:
		    edmove(k, EM_TAB | EM_CTRL | EM_MANUAL);
		    updatecursor();
		    break;

		default:
		    if(prevshift) prevshift = curfile->markmode = false;
		switch(k) {
		    case '\r':
			if(otherkeys)
			if((*otherkeys)(*this, k) == -1) return -1;

			if(insertmode || (curfile->lines->count == CURLINE+1)) {
			    edenter(smarttab);
			} else {
			    edmove(KEY_DOWN);
			    edmove(KEY_HOME);
			}
			break;
		    case KEY_BACKSPACE:
		    case CTRL('h'):
		    case 127:
			edbackspace();
			break;
		    case CTRL('y'):
			eddelline();
			break;
		    case KEY_TAB:
			inschar('\t');
			break;
		    case CTRL('t'):
			eddelword();
			break;

		    default:
			if(k >= 32 && k <= 255) {
			    if(!insertmode && !endofline()) eddel();
			    inschar(k);
			} else if(otherkeys) {
			    if(!getctrlkeys() && ((k == KEY_IC) || (k == KEY_DC)))
			    switch(k) {
				case KEY_IC: insertmode = !insertmode; break;
				case KEY_DC: eddel(); break;
			    } else {
				l = fn;
				if((*otherkeys)(*this, k) == -1) return -1;
				if(l != fn) {
				    draw();
				    updatecursor();
				}
			    }
			}
		}
	    }
	} else {
	    if(idle) (*idle)(*this);
	}
    }

    active = false;
    return !files->count ? TEXTEDITOR_NOFILES :
			   0;
}

void texteditor::close() {
    files->remove(fn);
    curfile = 0;
    
    if(files->count) {
	int n = fn;
	fn = -1;
	if(n >= files->count) n = files->count-1;
	setfnum(n);
    } else {
	active = false;
	fn = -1;
    }
}

bool texteditor::find(const char *needle, const char *options, int *col, int *line) {
    const char *f;
    char *p;
    int i, plus;
    enum {fromcur, fromstart, backward} fdirection;
    bool casesens = (bool) strchr(options, 's');

    if(strchr(options, 't')) {
	fdirection = fromstart;
	i = 0;
    } else if(strchr(options, 'b')) {
	fdirection = backward;
	i = 0;
    } else if(strchr(options, 'c')) {
	fdirection = fromcur;
	i = CURLINE;
    }

    for(; i < curfile->lines->count; i++) {
	p = (char *) curfile->lines->at(i);

	if((fdirection == fromcur) && (i == CURLINE)) plus = CURCOL+1;
	else plus = 0;
	
	if(!casesens) f = strqcasestr(p+plus, needle, "");
	else f = strstr(p+plus, needle);
	
	if(f) {
	    *col = (int) (f-p);
	    *line = i;
	    return true;
	}
    }
    
    return false;
}

bool texteditor::ismark() {
    return curfile ? curfile->markmode : false;
}

char *texteditor::getline(int ln) {
    return (char *) (getfcount() ? curfile->lines->at(ln) : 0);
}

void texteditor::putline(int ln, const char *newline) {
    char *p = (char *) curfile->lines->at(ln);
    string deltext, instext;

    deltext = (string) p + "\n";
    instext = (string) newline + "\n";

    curfile->lines->replace(ln, (void *) newline);

    modification(udelchar, deltext, false, 0, ln);
    modification(uinschar, instext, true, 0, ln);
}

void texteditor::redraw() {
    active = (bool) curfile;
    draw();
}

void texteditor::highlight(int line, int color) {
    highlight(getfnum(), line, color);
}

void texteditor::highlight(int fn, int line, int color) {
    editfile *f = (editfile *) files->at(fn);
    int i;

    if(f) {
	if((i = f->highlines->findnum(&line, &findhighline)) != -1) {
	    f->highlines->remove(i);
	}
    
	highline *h = new highline;
	h->line = line;
	h->color = color;
	f->highlines->add(h);
    }
}

void texteditor::unlight(int line) {
    unlight(getfnum(), line);
}

void texteditor::unlight(int fn, int line) {
    int i;
    editfile *f = (editfile *) files->at(fn);

    if(f) {
	if((i = f->highlines->findnum(&line, &findhighline)) != -1) {
	    f->highlines->remove(i);
	}
    }
}

void texteditor::clearlight() {
    clearlight(getfnum());
}

void texteditor::clearlight(int fn) {
    editfile *f = (editfile *) files->at(fn);
    if(f) f->highlines->empty();
}

void texteditor::switchmark() {
    CHECKLOADED;

    if(ismark()) endmark();
    else startmark();
}

void texteditor::shiftident(int x1, int y1, int x2, int y2, int delta) {
    int starty = y1, endy = y2, i;
    char *p, *newp;
    string origtext, repltext;

    CHECKLOADED;
    if(!delta) return;
    if(x1) starty++;
    if(!x2) endy--;
    
    for(i = starty; i <= endy; i++) {
	p = (char *) curfile->lines->at(i);

	if(delta > 0) {
	    newp = new char[strlen(p)+delta+1];
	    sprintf(newp, "%*s%s", delta, "", p);
	} else {
	    if(strspn(p, " ") >= -delta) {
		newp = strdup(p-delta);
	    } else {
		newp = strdup(p);
		strimlead(newp);
	    }
	}

	strimtrail(newp);

	if(origtext.empty() && repltext.empty()) {
	    origtext = p;
	    repltext = newp;
	} else {
	    origtext += "\n";
	    repltext += "\n";
	    origtext += p;
	    repltext += newp;
	}

	curfile->lines->replace(i, newp);
    }
    
    modification(udelblock, origtext, true, curfile->markblock->x1, curfile->markblock->y1);
    modification(uinsblock, repltext, false, curfile->markblock->x1, curfile->markblock->y1);
}

void texteditor::shiftident(int delta) {
    CHECKLOADED;
    shiftident(curfile->markblock->x1, curfile->markblock->y1,
    curfile->markblock->x2, curfile->markblock->y2, delta);
}

void texteditor::undo() {
    int i, aline;
    undorecord *ur;
    tundoaction a;
    bool firstpass = true, finished = false;
    undolog = show = false;

    CHECKLOADED;
    while(curfile->undo->count && !finished) {
	ur = (undorecord *) curfile->undo->at(curfile->undo->count-1);

	if(!firstpass && ((a != ur->action) || (aline != ur->y))) {
	    if(!ur->prevconnected) break;
	    finished = true;
	} else {
	    a = ur->action;
	    aline = ur->y;
	}
	
	firstpass = false;
	setpos(ur->x, ur->y);

	switch(ur->action) {
	    case uinschar:
	    case uinsblock:
		clearmark();
		for(i = 0; i < ur->data.size(); i++) eddel(false);
		break;
	    case udelchar:
	    case udelblock:
		for(i = 0; i < ur->data.size(); i++)
		switch(ur->data[i]) {
		    case '\n':
			edenter(false);
			edmove(KEY_HOME);
			break;
		    default:
			inschar(ur->data[i]);
			break;
		}
		break;
	}

	curfile->undo->remove(curfile->undo->count-1);
    }
    
    abscol = CURCOL;
    scancomments(true);
    undolog = show = true;
    redraw();
}

// --------------------------------------------------------------------------

void texteditor::textblockfree(void *p) {
    textblock *tb = (textblock *) p;
    if(tb) delete tb;
}

void texteditor::highlinefree(void *p) {
    highline *hl = (highline *) p;
    if(hl) delete hl;
}

void texteditor::editfilefree(void *p) {
    editfile *ef = (editfile *) p;
    if(ef) {
	delete ef->lines;
	delete ef->blocks;
	delete ef->highlines;
	delete ef->undo;
	delete ef->markblock;
	delete ef->id;
	delete ef;
    }
}

void texteditor::undorecordfree(void *p) {
    undorecord *ur = (undorecord *) p;
    if(ur) delete ur;
}

int texteditor::findint(void *p1, void *p2) {
    return *(int *) p1 != (int) p2;
}

int texteditor::findhighline(void *p1, void *p2) {
    return *(int *) p1 != ((highline *) p2)->line;
}

void texteditor::shiftmarkedblock(int delta) {
    CHECKLOADED;
    if(CURLINE <= curfile->markblock->y1) {
	curfile->markblock->y1 += delta;
	curfile->markblock->y2 += delta;
    } else if((CURLINE > curfile->markblock->y1) && (CURLINE < curfile->markblock->y2)) {
	curfile->markblock->y2 += delta;
    }
}

// --------------------------------------------------------------------------

#ifdef __KTOOL_USE_NAMESPACES

using ktool::hlight;

#endif

bool hlight::operator == (const hl_kind &k) const {
    return kind == k;
}

bool hlight::operator != (const hl_kind &k) const {
    return kind != k;
}

bool hlight::operator < (const hlight &ah) const {
    return kind == h_eol;
}

