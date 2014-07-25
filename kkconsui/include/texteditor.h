#ifndef __KOST_TEXTEDITOR_H
#define __KOST_TEXTEDITOR_H

#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "linkedlist.h"
#include "conf.h"

#include "kkstrtext.h"
#include "conscommon.h"
#include "abstractui.h"

#define TEXTEDITOR_NOFILES -2
#define TEXTEDITOR_ABORTED -1
#define TEXTEDITOR_OK       0

__KTOOL_BEGIN_NAMESPACE

typedef enum {
    h_alone,
    h_eol,
    h_symbol,
    h_block,
    h_quotes,
    h_comment
} hl_kind;      // kinds of text to highlight

typedef enum {
    udelchar,
    udelblock,
    uinschar,
    uinsblock
} tundoaction;

// text highlight entry

struct hlight {
    hl_kind kind;
    INT color;
    string text;

    bool operator == (const hl_kind &kind) const;
    bool operator != (const hl_kind &kind) const;
    bool operator < (const hlight &ah) const;
};

struct highline {
    INT line, color;
};

struct textblock {
    INT x1, y1, x2, y2, color;
};

struct editfile {
    char *id;

    linkedlist *lines, *blocks, *highlines, *undo;
    textblock *markblock;
    INT sx, sy, x, y, ncolorscheme;
    bool modified, markmode, markreverse, showmarked;
};

struct colorscheme {
    vector<hlight> hl;
    vector<INT> difcolors;

    string synt_quote, synt_qescape;

    INT ncolor, qcolor, blockcolor;
    bool bold;
};

struct undorecord {
    tundoaction action;
    bool prevconnected;
    INT x, y;
    string data;
};

class texteditor: public abstractuicontrol {
    protected:
	linkedlist *files;
	vector<colorscheme> colorschemes;
	
	INT outx, abscol;
	INT fn /* file number */, fcount /* files count */;
	bool undolog, show, prevshift;

	editfile *curfile;
	colorscheme colors;

	static void textblockfree(void *p);
	static void editfilefree(void *p);
	static void undorecordfree(void *p);
	static void highlinefree(void *p);

	static INT findint(void *p1, void *p2);
	static INT findhl(void *p1, void *p2);
	static INT findhighline(void *p1, void *p2);
	static INT sorthl(void *p1, void *p2);

	bool fix_x(bool tab);
	void sethlcolor(INT n);
	void draw(INT fromline);
	void draw_print(char *buf, INT bcolor, INT distance);

	INT addwindow(char *id);

	void scancomments(bool visible);

	void modification(tundoaction action, const string &data,
	    bool connected = false, INT curx = -1, INT cury = -1);

	bool endofline();
	const char currentchar();

	INT count_clrcodes(char *cp, INT pos);
	INT hl_comment(char *cp, char *txt, INT color);
	INT hl_comment(char *cp, INT st, INT pend, INT color);
	void showline(INT ln, INT startx, INT distance, INT extrax = 0);

	void marktext();
	void edmove(INT k, INT options = 2);
	void eddel(bool usetabs = true);
	void edenter(bool countspaces = true);
	void edbackspace();
	void eddelline();
	void eddelword();

	void inschar(INT k);

	void shiftmarkedblock(INT delta);
	
    public:
	bool active, wrap, insertmode, modified, smarttab;

	INT (*otherkeys)(texteditor &caller, INT k);
	void (*idle)(texteditor &caller);

	texteditor();
	virtual ~texteditor();

	INT load(const string buf, const string id);
	INT load(FILE *f, const string id);
	INT load(ifstream &f, const string id);
	    // loads a file from char* or FILE opening
	    // a new editor window

	char *save(const char *linebreak);
	INT save(FILE *f, const char *linebreak);
	INT save(ofstream &f, const string linebreak);
	    // saves a file to char* or FILE

	void close();
	    // closes current file

	INT getfcount();
	    // returns the amount of windows opened

	INT getfnum();
	    // returns current editor window number

	char *getfid();
	char *getfid(INT fnn);
	    // returns id of a file

	void setfid(char *id);
	void setfid(INT fnn, char *id);
	    // sets an id of file

	void setfnum(INT n);
	    // sets active file

	void setcolorscheme(INT nscheme);
	void setcolorscheme(INT fn, INT nscheme);
	    // set color scheme for current file

	void startmark();
	void endmark();
	void switchmark();
	bool ismark();

	void clearmark();
	void copymark(FILE *f);
	void copymark(char *p, INT maxlen);

	void insert(FILE *f);
	void insert(const string buf);

	void delmark();
	void undo();
	
	void addblock(INT x1, INT y1, INT x2, INT y2, INT color);
	    // adds a block

	void setcoords(INT nx1, INT ny1, INT nx2, INT ny2);
	INT addscheme(INT nc, INT bc, INT fbold, ...);
	    // nc    = normal text color
	    // bc    = block color
	    // fbold = use bold colors
	    // ...   = numbers of pairs in different attribute,
	    //         the last one MUST BE 0 !
	void addhighlight(INT nscheme, string text, INT color, hl_kind kind);
	void addcolordif(INT nscheme, INT pairno);
	
	INT open();
	virtual void redraw();
	void draw();
	void updatecursor();

	bool find(const char *needle, const char *options, INT *col, INT *line);

	void setpos(INT col, INT line);
	void getpos(INT *col, INT *line);
	
	char *getline(INT ln);
	void putline(INT ln, const char *newline);

	void highlight(INT line, INT color);
	void highlight(INT fn, INT line, INT color);

	void unlight(INT line);
	void unlight(INT fn, INT line);

	void clearlight();
	void clearlight(INT fn);
	    // lines highlighting

	void shiftident(INT x1, INT y1, INT x2, INT y2, INT delta);
	void shiftident(INT delta);
};

__KTOOL_END_NAMESPACE

#ifdef __KTOOL_USE_NAMESPACES

using ktool::editfile;
using ktool::texteditor;

using ktool::hl_kind;
using ktool::h_alone;
using ktool::h_eol;
using ktool::h_symbol;
using ktool::h_block;
using ktool::h_quotes;
using ktool::h_comment;

#endif

#endif
