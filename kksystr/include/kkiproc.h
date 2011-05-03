#ifndef __KONST_PROCESS_H_
#define __KONST_PROCESS_H_

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utmp.h>
#include <string.h>
#include <ctype.h>

#include "conf.h"
#include "kkfsys.h"
#include "kkstrtext.h"

__KTOOL_BEGIN_C

int dataready(int fd, int dowait);

time_t lastkeypress();
void detach(char *logfile);

char *getprocentry(char *fname);
const char *getcurtty();

__KTOOL_END_C

#ifdef __sun__

void setenv(const string &name, const string &value, int replace);

#endif

#endif
