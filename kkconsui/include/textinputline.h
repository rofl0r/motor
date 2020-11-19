#ifndef __TEXTINPUTLINE_H__
#define __TEXTINPUTLINE_H__

#include "fileselector.h"

__KTOOL_BEGIN_NAMESPACE

#define TEXTINPUT_HIST_LIMIT    100

class textinputline: public abstractuicontrol {
    public:
	int (*otherkeys)(textinputline &caller, int k);
	void (*idle)(textinputline &caller);

    private:
	vector<string> history;
	int length, position, beginpos, lastkey;
	int color, histcolor, histcurcolor;
	string value;
	char passwordchar;
	fileselector *selector;
	bool firstiter;

	bool keymove(int key);
	void redraw();
	void exechistory();

    public:
	textinputline();
	~textinputline();

	void historyadd(const string &buf);

	void setvalue(const string &buf);
	string getvalue() const;

	void setcoords(int x, int y, int len);
	void setcolor(int acolor, int ahistcolor = 0, int ahistcurcolor = 0);
	void setpasswordchar(char npc);

	void connectselector(fileselector &fsel);
	void removeselector();

	int getlastkey() const;

	void exec();
	void close();
};

__KTOOL_END_NAMESPACE

#ifdef __KTOOL_USE_NAMESPACES

using ktool::textinputline;

#endif

#endif
