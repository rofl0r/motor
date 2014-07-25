#ifndef __TEXTINPUTLINE_H__
#define __TEXTINPUTLINE_H__

#include "fileselector.h"

__KTOOL_BEGIN_NAMESPACE

#define TEXTINPUT_HIST_LIMIT    100

class textinputline: public abstractuicontrol {
    public:
	INT (*otherkeys)(textinputline &caller, INT k);
	void (*idle)(textinputline &caller);

    private:
	vector<string> history;
	INT length, position, beginpos, lastkey;
	INT color, histcolor, histcurcolor;
	string value;
	char passwordchar;
	fileselector *selector;
	bool firstiter;

	bool keymove(INT key);
	void redraw();
	void exechistory();

    public:
	textinputline();
	~textinputline();

	void historyadd(const string &buf);

	void setvalue(const string &buf);
	string getvalue() const;

	void setcoords(INT x, INT y, INT len);
	void setcolor(INT acolor, INT ahistcolor = 0, INT ahistcurcolor = 0);
	void setpasswordchar(char npc);

	void connectselector(fileselector &fsel);
	void removeselector();

	INT getlastkey() const;

	void exec();
	void close();
};

__KTOOL_END_NAMESPACE

#ifdef __KTOOL_USE_NAMESPACES

using ktool::textinputline;

#endif

#endif
