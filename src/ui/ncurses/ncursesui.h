#ifndef __NCURSESUI_H__
#define __NCURSESUI_H__

#include "motorcommon.h"
#include "motorui.h"
#include "uidefs.h"
#include "uiconfiguration.h"
#include "textinputline.h"
#include "fileselector.h"
#include "dialogbox.h"
#include "texteditor.h"
#include "motorproject.h"
#include "motorexecutor.h"
#include "uitagbrowser.h"

__MOTOR_BEGIN_NAMESPACE

class ncursesui: public motorui {
    private:
	enum mgrmode { selectonly, selectorcreate };
	enum projeditaction { psettings, pfiles, pdirs };

	vector<INT> boldcolors;
	bool terminate, outopen, texteditok, watchesactive;

	textinputline inp;
	fileselector selector;
	textwindow outwindow, selwindow;

	vector<string> outlines;
        string distdir;
	map<string, string> distparams;

	static void sighandler(int signum);
        static INT texteditboxkeys(texteditor &ed, INT k);
	static bool horizontalmenukeys(horizontalmenu &hm, INT k);

	const projectname selectproject(mgrmode amode = selectonly, vector<string> templs = vector<string>());
	string selecttemplate(const string &def = "", const string &title = _(" Templates available ")) const;

	void projectedit(projeditaction pea, bool setcurrent = true);
	void populatesettingstree(treeview &tree);
	void populatecontentstree(treeview &tree, motorproject &mp, projeditaction pea, bool setcurrent);
	bool projectsettings();
	bool projectcontents(projeditaction pea, bool setcurrent = true);

	bool vcson();

    protected:
	horizontalmenu menubar;
	vector<motorexecutor::message>::const_iterator currentmsg;

	void commandline(INT argc, char **argv);
	void usage();

	void mainloop();

	void workareaupdate();

	void initmenubar();
	void initelements();
	void execmenubar();

	void help();

	bool createproject();
	bool regproject();
	bool changetemplate();

	string inputskel(const string &initval, const string &text, bool selmode);
	void pointmessage();

	void dist();
	void maketarget();

	void settings();
	void extkeylist();
	void vcsimport();
	void showbreakpoints();
	void generate();
	void arguments();
	void vcscheck();

	void onprojectload();
	void pload(const projectname pname = projectname());
	string selectprojectfile(motorproject mp);

	bool editbuildoptions(motorfile &f) const;

    public:
	ncursesui();
	~ncursesui();

	void execute(INT argc, char **argv);
	void redraw();

	askresult ask(const string &answersallowed, const string &text);
	void log(const string &text = "");
	askresult input(inputkind kind, string &result, const string &prompt);

	void reloadeditfiles();
	void loadeditfile(const editfile ef, INT options = 0);

	vector<editfile> geteditfiles();
	vector<string> geteditfile(const string &fname);

	bool editcloseall();
	void updatewatches();

	void setdesktop(const vector< pair<string, string> > &asettings);
	vector< pair<string, string > > getdesktop() const;

	bool editmodified() const;

	void executordone(const string &target, INT nerr, INT nwarn, INT ninfo);
	void externalexec(INT options, const string &cmd);
	void setdebugcurrentline(const editfile ef);

	void statusupdate();

	bool isterminated();
	bool hotkey(INT k);

	bool autosave();
	void writeoutput(const string &text);
	void doneoutput();

	void showmessages();
	void nextmessage();

	void showoutput();
	void evaluate(const string &e);
	void loadcore();
	bool showstack();

	void textboxf(const string &text, const char *fmt, ...);
	void textbox(const string &text, const string &title);
	bool texteditbox(const string &title, string &text);

	askresult notemplate(string &templname) const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::ncursesui;

#endif

extern ncursesui thisui;

#endif
