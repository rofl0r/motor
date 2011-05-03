/*
*
* kkiproc inter-process communications related routines
* $Id: kkiproc.cc,v 1.11 2003/04/02 09:54:47 konst Exp $
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

#include "kkiproc.h"

#include <sys/ioctl.h>
#include <signal.h>
#include <memory>

#ifdef __sun__
#include <sys/termio.h>
#include <sys/filio.h>
#endif

#ifdef __CYGWIN__
#include <sys/socket.h>
#endif

void detach(char *logfile) {
    if(logfile) freopen(logfile, "w", stdout);

    if(!fork()) {
	setsid();
	chdir("/");
    } else {
	_exit(0);
    }
}

time_t lastkeypress() {
    time_t t = 0;
    struct stat s;

#ifdef __linux__

    struct utmp *u;
    char tname[32];
    int tlen;

    if((tlen = readlink("/proc/self/fd/0", tname, 12)) != -1) {
	tname[tlen] = 0;

	if(!strncmp(tname, "/dev/tty", 8) && isdigit(tname[8])) {
	    setutent();

	    while((u = getutent())) {
		switch(u->ut_type) {
		    case USER_PROCESS:
		    case LOGIN_PROCESS:
			if(strlen(u->ut_line) > 3)
			if(!strncmp(u->ut_line, "tty", 3))
			if(isdigit(u->ut_line[3])) {
			    sprintf(tname, "/proc/%lu", u->ut_pid);

			    if(*u->ut_user && !access(tname, F_OK)) {
				sprintf(tname, "/dev/%s", u->ut_line);
				if(!stat(tname, &s))
				if(s.st_atime > t)
				    t = s.st_atime;
			    }
			}
			break;
		}
	    }

	    endutent();
	} else {
	    if(!stat(tname, &s)) {
		t = s.st_atime;
	    } else {
		time(&t);
	    }
	}
    } else {
	time(&t);
    }

#else

    char *p;

    if((p = ttyname(0)) != NULL) {
	if(!stat(p, &s) && s.st_atime > t) t = s.st_atime; else time(&t);
    } else {
	time(&t);
    }
     
#endif

    return t;
}

int dataready(int fd, int dowait) {
    struct timeval tv;
    fd_set fds;
    int rc;

    tv.tv_sec = tv.tv_usec = 0;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    if(select(fd+1, &fds, 0, 0, dowait ? 0 : &tv) != -1) {
	if(rc = FD_ISSET(fd, &fds)) {
	    ioctl(fd, FIONREAD, &rc);
	}
    } else {
	rc = 0;
    }

    return rc;
}

char *getprocentry(char *fname) {
    FILE *f = fopen(fname, "r");
    static char *p = 0;
    int fsize = kfilesize(fname);

    if(f) {
	p = (char *) realloc(p, fsize+1);
	fread(p, fsize, 1, f);
	fclose(f);
    } else {
	p = 0;
    }

    return p;
}

char *gethostname() {
    return getprocentry("/proc/kernel/hostname");
}

char *getdomainname() {
    return getprocentry("/proc/kernel/domainname");
}

const char *getcurtty() {
    static char buf[64];
    int n;

    sprintf(buf, "/proc/%lu/fd/0", getpid());
    if((n = readlink(buf, buf, 64)) != -1) {
	buf[n] = 0;
	return buf;
    }

    return 0;
}

#ifdef __sun__

void setenv(const string &name, const string &value, int replace) {
    if(getenv(name.c_str()) && replace) {
	auto_ptr<char> stuff(new char[name.size() + value.size() + 2]);
	sprintf(stuff.get(), "%s=%s", name.c_str(), value.c_str());
	putenv(stuff.get());
    }
}

#endif
