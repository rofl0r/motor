#ifndef __MOTORUI_H__
#define __MOTORUI_H__

#include "motorcommon.h"

#define	LF_CREATE_IF_NOTFOUND	2

#define	EE_PAUSE		2

__MOTOR_BEGIN_NAMESPACE

class motorui {
    public:
	enum askresult {
	    yes, no, cancel, all
	};

	enum inputkind {
	    file, directory, text,
	    filechroot, directorychroot
	};

	class editfile {
	    public:
		editfile();
		editfile(const string &afname, INT ax, INT ay);

		string fname;
		INT x, y;

		bool operator == (const string &afname);
		bool operator != (const string &afname);
	};

    protected:
	bool outputblocked;

    public:
	motorui();
	~motorui();

	virtual void execute(INT argc, char **argv);
	virtual void redraw();

	virtual askresult ask(const string &answersallowed, const string &text);
	virtual askresult askf(const string &answersallowed, const char *fmt, ...);

	virtual void log(const string &text = "");
	virtual void logf(const char *fmt, ...);

	virtual void loadeditfile(const editfile ef, INT options = 0);
	virtual void reloadeditfiles();
	virtual void setdebugcurrentline(const editfile ef);
	virtual vector<editfile> geteditfiles();
	    // the last item gotta be the current file name
	virtual vector<string> geteditfile(const string &fname);

	virtual bool editcloseall();
	virtual bool autosave();

	virtual askresult input(inputkind kind, string &result, const string &prompt);

	virtual void setoutputblockmode(bool fblock);
	virtual void writeoutput(const string &text);
	virtual void doneoutput();
	virtual void executordone(const string &target, INT nerr,
	    INT nwarn, INT ninfo);

	virtual void externalexec(INT options, const string &cmd);
	virtual void externalexecf(INT options, const char *fmt, ...);

	virtual void setdesktop(const vector< pair<string, string> > &asettings);
	virtual vector< pair<string, string > > getdesktop() const;
	    // This pair of functions deal with project
	    // specific UI settings

	virtual void updatewatches();
	virtual bool editmodified() const;

	virtual askresult notemplate(string &templname) const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::motorui;

#endif

extern motorui &ui;

#endif
