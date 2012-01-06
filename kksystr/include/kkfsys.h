#ifndef __KONST_FS_H_
#define __KONST_FS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <regex.h>

#include "conf.h"
#include "kkstrtext.h"

#ifndef HAVE_FTW_H

#define FTW_F           1
#define FTW_D           2
#define FTW_DNR         3
#define FTW_SL          4
#define FTW_NS          5

#define FTW_CHDIR       1

#define nftw(d, f, de, fl)      stubnftw(d, f, de, fl)
#define ftw(d, f, de)           stubnftw(d, f, de, 0)

#else

#include <ftw.h>

#endif

#define	FFIND_FILE	1
#define	FFIND_LINK	2
#define	FFIND_DIR	4

vector<string> filefind(const string &mask, const string &aroot, int mode = FFIND_FILE);

string readlink(const string &fname);
string pathfind(const string &name, const string &path, int amode = F_OK);
bool mksubdirs(string dir);
bool samefile(const string &fname1, const string &fname2);

__KTOOL_BEGIN_C

unsigned long kfilesize(char *fname);
void freads(FILE *f, char *s, int maxlen);
int fcopy(const char *source, const char *dest);
int fmove(const char *source, const char *dest);
int stubnftw(const char *dir, int (*fn)(const char *file, const struct stat *sb, int flag), int depth, int flags);

__KTOOL_END_C

#endif
