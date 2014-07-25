#ifndef __MOTORCONFIGURATION_H__
#define __MOTORCONFIGURATION_H__

#include "motorcommon.h"

__MOTOR_BEGIN_NAMESPACE

/*
*
* This class is intended for holding motor configuration settings.
*
*/

class motorconfiguration {
    public:
	class extkey {
	    private:
		INT keycode;
		string command, keyname;

	    public:
		extkey();
		extkey(const string &akey, const string &acmd);
		~extkey();

		void exec() const;
		string description() const;

		bool operator == (INT akeycode) const;
		bool operator != (INT akeycode) const;
	};

    protected:
	string motordir, defprojectsdir, userfullname;
	bool dispvcs, dispmake, dispcomp, debugstd, checktty;

	vector<extkey> extkeys;

	void readuserdetails();

    public:
	motorconfiguration();
	~motorconfiguration();

	bool load();
	void save();

	string getmotordir();
	string getprojectdir();
	string gettemplatedir(const string &aname = "");
	string getconfigfname(const string &afname);

	string getdefaultprojectsdir() const;
	void setdefaultprojectsdir(const string &ad);

	vector<extkey>::const_iterator extkeybegin() const;
	vector<extkey>::const_iterator extkeyend() const;

	void getdisplay(bool &amake, bool &acomp, bool &avcs) const;
	void setdisplay(bool amake, bool acomp, bool avcs);

	bool getdebugstd() const;
	void setdebugstd(bool adebugstd);

	bool getchecktty() const;
	void setchecktty(bool achecktty);

	string getuserfullname() const;
};

__MOTOR_END_NAMESPACE

#ifdef __MOTOR_USE_NAMESPACES

using motor::motorconfiguration;

#endif

extern motorconfiguration conf;

#endif
