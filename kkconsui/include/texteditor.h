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
    int color;
    string text;

    bool operator == (const hl_kind &kind) const;
    bool operator != (const hl_kind &kind) const;
    bool operator < (const hlight &ah) const;
};

struct highline {
    int line, color;
};

struct textblock {
    int x1, y1, x2, y2, color;
};

struct editfile {
    char *id;

    linkedlist *lines, *blocks, *highlines, *undo;
    textblock *markblock;
    int sx, sy, x, y, ncolorscheme;
    bool modified, markmode, markreverse, showmarked;
};

struct colorscheme {
    vector<hlight> hl;
    vector<int> difcolors;

    string synt_quote, synt_qescape;

    int ncolor, qcolor, blockcolor;
    bool bold;
};

struct undorecord {
    tundoaction action;
    bool prevconnected;
    int x, y;
    string data;
};

class texteditor: public abstractuicontrol {
    protected:
	linkedlist *files;
	vector<colorscheme> colorschemes;
	
	int outx, abscol;
	int fn /* file number */, fcount /* files count */;
	bool undolog, show, prevshift;

	editfile *curfile;
	colorscheme colors;

	static void textblockfree(void *p);
	static void editfilefree(void *p);
	static void undorecordfree(void *p);
	static void highlinefree(void *p);

	static int findint(void *p1, void *p2);
	static int findhl(void *p1, void *p2);
	static int findhighline(void *p1, void *p2);
	static int sorthl(void *p1, void *p2);

	bool fix_x(bool tab);
	void sethlcolor(int n);
	void draw(int fromline);
	void draw_print(char *buf, int bcolor, int distance);

	int addwindow(char *id);

	void scancomments(bool visible);

	void modification(tundoaction action, const string &data,
	    bool connected = false, int curx = -1, int cury = -1);

	bool endofline();
	const char currentchar();

	int count_clrcodes(char *cp, int pos);
	int hl_comment(char *cp, char *txt, int color);
	int hl_comment(char *cp, int st, int pend, int color);
	void showline(int ln, int startx, int distance, int extrax = 0);

	void marktext();
	void edmove(int k, int options = 2);
	void eddel(bool usetabs = true);
	void edenter(bool countspaces = true);
	void edbackspace();
	void eddelline();
	void eddelword();

	void inschar(int k);

	void shiftmarkedblock(int delta);
	
    public:
	bool active, wrap, insertmode, modified, smarttab;

	int (*otherkeys)(texteditor &caller, int k);
	void (*idle)(texteditor &caller);

	texteditor();
	virtual ~texteditor();

	int load(const string buf, const string id);
	int load(FILE *f, const string id);
	int load(ifstream &f, const string id);
	    // loads a file from char* or FILE opening
	    // a new editor window

	char *save(const char *linebreak);
	int save(FILE *f, const char *linebreak);
	int save(ofstream &f, const string linebreak);
	    // saves a file to char* or FILE

	void close();
	    // closes current file

	int getfcount();
	    // returns the amount of windows opened

	int getfnum();
	    // returns current editor window number

	char *getfid();
	char *getfid(int fnn);
	    // returns id of a file

	void setfid(char *id);
	void setfid(int fnn, char *id);
	    // sets an id of file

	void setfnum(int n);
	    // sets active file

	void setcolorscheme(int nscheme);
	void setcolorscheme(int fn, int nscheme);
	    // set color scheme for current file

	void startmark();
	void endmark();
	void switchmark();
	bool ismark();

	void clearmark();
	void copymark(FILE *f);
	void copymark(char *p, int maxlen);

	void insert(FILE *f);
	void insert(const string buf);

	void delmark();
	void undo();
	
	void addblock(int x1, int y1, int x2, int y2, int color);
	    // adds a block

	void setcoords(int nx1, int ny1, int nx2, int ny2);
	int addscheme(int nc, int bc, int fbold, ...);
	    // nc    = normal text color
	    // bc    = block color
	    // fbold = use bold colors
	    // ...   = numbers of pairs in different attribute,
	    //         the last one MUST BE 0 !
	void addhighlight(int nscheme, string text, int color, hl_kind kind);
	void addcolordif(int nscheme, int pairno);
	
	int open();
	virtual void redraw();
	void draw();
	void updatecursor();

	bool find(const char *needle, const char *options, int *col, int *line);

	void setpos(int col, int line);
	void getpos(int *col, int *line);
	
	char *getline(int ln);
	void putline(int ln, const char *newline);

	void highlight(int line, int color);
	void highlight(int fn, int line, int color);

	void unlight(int line);
	void unlight(int fn, int line);

	void clearlight();
	void clearlight(int fn);
	    // lines highlighting

	void shiftident(int x1, int y1, int x2, int y2, int delta);
	void shiftident(int delta);
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
