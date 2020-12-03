#ifndef __MOTORCOMMON_H__
#define __MOTORCOMMON_H__

#include <vector>
#include <list>
#include <algorithm>
#include <fstream>
#include <map>

#include <unistd.h>
#include <stdio.h>
#include <regex.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "parser.h"

#ifdef HAVE_CONFIG_H

#include "config.h"

#else

// #define LOCALE_DIR  "/usr/local/share/locale/"
// #define SHARE_DIR   "/usr/local/share/motor/"
#define VERSION     "3.5.2"
#define GNUMAKE     "gmake"

#endif

#include "kkstrtext.h"
#include "conf.h"

#ifdef __KTOOL_USE_NAMESPACES
#define __MOTOR_USE_NAMESPACES
#endif

#ifdef __MOTOR_USE_NAMESPACES

#define __MOTOR_BEGIN_NAMESPACE		namespace motor {
#define __MOTOR_END_NAMESPACE		}

using namespace std;

#else

#define __MOTOR_BEGIN_NAMESPACE
#define __MOTOR_END_NAMESPACE

#endif


#ifdef ENABLE_NLS

#include <libintl.h>
#define _(s) gettext(s)

#else

#define _(s) (s)

#endif

#endif
