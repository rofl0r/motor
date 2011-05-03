/*
*
* kkfsys file system related routines
* $Id: kkfsys.cc,v 1.7 2002/07/03 14:38:58 konst Exp $
*
* Copyright (C) 1999-2001 by Konstantin Klyagin <konst@konst.org.ua>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at
* your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
* USA
*
*/

#include "kkfsys.h"

unsigned long kfilesize(char *fname) {
    struct stat buf;
    if(!stat(fname, &buf)) return buf.st_size; else return 0;
}

void freads(FILE *f, char *s, int maxlen) {
    s[0] = 0;
    fgets(s, maxlen, f);

    for(int i = strlen(s)-1; ; i--)
    if(i >= 0 && strchr("\r\n", s[i])) s[i] = 0; else break;
}

int fcopy(const char *source, const char *dest) {
    FILE *inpf, *outf;
    int ret = -1, bc;
    char buf[8192];

    if(inpf = fopen(source, "r")) {
	if(outf = fopen(dest, "w")) {
	    ret = bc = 0;
	    
	    while((bc = fread(buf, 1, 8192, inpf))) {
		fwrite(buf, bc, 1, outf);
		ret += bc;
		if(bc < 8192) break;
	    }
	    
	    fclose(inpf);
	}
	
	fclose(outf);
    }

    return ret;
}

int fmove(const char *source, const char *dest) {
    int ret = fcopy(source, dest);
    if(ret != -1) unlink(source);
    return ret;
}

void stepftw(const char *fname, int *stopwalk, int (*fn)(const char *file, const struct stat *sb, int flag)) {
    struct stat st;
    int flag;

    flag = FTW_F;

    if(lstat(fname, &st)) {
#ifdef FTW_NS
	flag = FTW_NS;
#endif
    } else {
	if(S_ISLNK(st.st_mode)) {
#ifdef FTW_SL
	    flag = FTW_SL;
#endif
	} else
	if(S_ISREG(st.st_mode)) flag = FTW_F; else
	if(S_ISDIR(st.st_mode)) {
	    flag = FTW_D;
#ifdef FTW_DNR
	    if(access(fname, R_OK)) flag = FTW_DNR;
#endif
	}
    }

    if(fn) {
	if((*fn)(fname, &st, flag)) {
	    *stopwalk = 1;
	} else if(flag == FTW_D) {
	    if(stubnftw(fname, fn, 0, 1)) *stopwalk = 1;
	}
    }
}

int stubnftw(const char *dir, int (*fn)(const char *file, const struct stat *sb, int flag), int depth, int flags) {
    DIR *dr;
    struct dirent *ent;
    char *fname;
    int stopwalk = 0;

    if(!flags) {
	stepftw(dir, &stopwalk, fn);
    } else if(dr = opendir(dir)) {
	while((ent = readdir(dr)) && !stopwalk) {
	    if( !strcmp(ent->d_name, ".") ||
		!strcmp(ent->d_name, "..")) continue;

	    fname = (char *) malloc(strlen(dir) + strlen(ent->d_name) + 2);
	    strcpy(fname, dir);
	    if(fname[strlen(fname)] != '/') strcat(fname, "/");
	    strcat(fname, ent->d_name);

	    stepftw(fname, &stopwalk, fn);
	    free(fname);
	}

	closedir(dr);
    }

    return stopwalk;
}

string pathfind(const string &name, const string &path, int amode) {
    string token, current, buf = path;

    while(!(token = getword(buf, ":")).empty()) {
	current = token + "/" + name;
	if(!access(current.c_str(), amode)) return current;
    }

    return "";
}

bool mksubdirs(string dir) {
    string subname, created;
    bool errhappen = false;

    if(!dir.empty())
    if(dir[0] == '/') created = "/";

    while(!dir.empty() && !errhappen) {
	subname = getword(dir, "/");

	if(!created.empty())
	if(*(created.end()-1) != '/') created += "/";

	created += subname;

	if(access(created.c_str(), F_OK))
	    errhappen = mkdir(created.c_str(), S_IRWXU);
    }

    return !errhappen;
}

string readlink(const string &fname) {
    char rfname[1024];
    int n;

    if((n = ::readlink(fname.c_str(), rfname, 1024)) != -1) {
	rfname[n] = 0;
	return rfname;
    } else {
	return "";
    }
}

bool samefile(const string &fname1, const string &fname2) {
    bool r;
    struct stat st1, st2;

    if(r = !stat(fname1.c_str(), &st1) && !stat(fname2.c_str(), &st2)) {
	r = st1.st_ino == st2.st_ino;
    } else {
	r = fname1 == fname2;
    }

    return r;
}

#define FINDFILE_PUSH(m) \
    if((mode & m) && !regexec(&r, fname.c_str(), 0, 0, 0)) \
	lst.push_back(fname);

vector<string> filefind(const string &mask, const string &aroot, int mode) {
    vector<string> lst, rlst;
    vector<string>::iterator is;
    regex_t r;
    DIR *d;
    string fname, root;
    struct dirent *de;
    struct stat st;

    root = aroot;
    if(root.substr(root.size()-1) != "/") root += "/";

    if(!regcomp(&r, mask.c_str(), REG_EXTENDED)) {
	if(d = opendir(root.c_str())) {
	    while(de = readdir(d)) {
		fname = de->d_name;
		if((fname != ".") && (fname != "..")) {
		    fname = root + fname;

		    if(!stat(fname.c_str(), &st)) {
			if(S_ISREG(st.st_mode)) {
			    FINDFILE_PUSH(FFIND_FILE);
			} else if(S_ISDIR(st.st_mode)) {
			    FINDFILE_PUSH(FFIND_DIR);
			    rlst = filefind(mask, fname, mode);
			    for(is = rlst.begin(); is != rlst.end(); is++) {
				lst.push_back(*is);
			    }
			} else if(S_ISLNK(st.st_mode)) {
			    FINDFILE_PUSH(FFIND_LINK);
			}
		    }
		}
	    }

	    closedir(d);
	}

	regfree(&r);
    }

    return lst;
}
