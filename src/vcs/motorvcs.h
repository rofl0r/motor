#ifndef __MOTORVCS_H__
#define __MOTORVCS_H__

#include "motorcommon.h"
#include "parserule.h"

__MOTOR_BEGIN_NAMESPACE

class motorvcs {
    public:
	enum changekind {
	    locally_added, locally_removed,
	    locally_modified, remotely_modified
	};

    private:
	bool fgood;
	string make;
	vector<parserule> parserrors;

	void load(const string &avcsname);
	void makeflist(const vector<string> &fnames);

    public:
	motorvcs();
	motorvcs(const string &avcsname);
	~motorvcs();

	void putmake(FILE *f, pparamslist *plist);
	bool iserror(parserule *apr, const string &msg);

	bool enabled();
	bool good();
	    // returns true if the last operation was successful

	void setcommitcomment(const string &amsg);
	void commit(const vector<string> &fnames);
	void update(const vector<string> &fnames);
	void localunmodify(const vector<string> &fnames);

	void addfile(const string &afname);
	void removefile(const string &afname);

	void forkbranch(const string &tag, const string &destdir);
	void tag(const string &arelname);
	void import();
	void checkout();

	string getdiff(const string &fname);

	vector< pair<string, changekind> > getchanges();
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::motorvcs;

#endif

extern motorvcs vcs;

#endif
