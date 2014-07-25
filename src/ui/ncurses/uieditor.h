#ifndef __UIEDITOR_H__
#define __UIEDITOR_H__

#include "uiconfiguration.h"
#include "texteditor.h"
#include "ncursesui.h"

__MOTOR_BEGIN_NAMESPACE

class uieditor: public texteditor {
    public:
	enum clipboardaction { cut, copy, paste };
	enum savemode { savecurrent, saveas, saveall, saveallask };

    protected:
	vector<uiconfiguration::syntaxitem> syntaxitems;

	static INT keyhandler(texteditor &e, INT k);
	static void idlefunc(texteditor &ref);

	void readsyntaxhighlight();
	void setsyntaxhighlight();

	motorui::askresult asksave();

    public:
	uieditor();
	~uieditor();

	void loadsettings();

	INT load(const string &buf, const string &id);
	INT load(ifstream &f, const string &id);

	void switchwindow(INT delta);
	void loadfile();
	bool closecurrentfile();
	bool fsave(savemode amode);

	void clipboard(clipboardaction a);
	void gotoline();
	void markbreakpoints();

	string atcursor();
	void windowlist();
	void resizebottom(INT lines);
	bool anymodified();
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::uieditor;

#endif

extern motor::uieditor ed;

#endif
