#ifndef __MOTORMISC_H__
#define __MOTORMISC_H__

#include "motorcommon.h"
#include "motorproject.h"

#define BOOL_TO_STR(a)		(a ? _("yes") : _("no"))
#define MAKEMODE_TO_STR(a)	(a == motorproject::automake ? _("automake") : _("manual"))

string findprogram(const string &progname);
string getparsed(motorproject &pf, const string &placefrom);

#endif
