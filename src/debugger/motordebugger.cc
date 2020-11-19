#include "motordebugger.h"
#include "motorexecutor.h"
#include "motorproject.h"
#include "motormisc.h"
#include "motorui.h"
#include "motorconfiguration.h"

#include "kkiproc.h"

#include <strstream>
#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <signal.h>
#include <memory>

#ifdef __FreeBSD__
#include <sys/consio.h>
#elif __OpenBSD__
#include <dev/wscons/wsdisplay_usl_io.h>
#else
#include <sys/vt.h>
#endif

#ifdef DEBUG
ofstream motordebugger::dlog;
#endif

motordebugger::motordebugger(): factive(false), frunning(false),
alwaysjustrun(false), fpread(0), fpwrite(0), lastdmsg(0) {
#ifdef DEBUG
    if(!dlog.is_open()) {
	dlog.open(((string) getenv("HOME") + "/.motor/debug-log").c_str(), ios::app);

	time_t t;
	dlog << "Debugger log started on " << ctime(&(t = time(0))) << endl;
    }
#endif
}

motordebugger::~motordebugger() {
}

#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)

#define TTY_PREFIX "/dev/ttyv"
#define TTY_NUMBER hex << vt-1

#else

#define TTY_PREFIX "/dev/tty"
#define TTY_NUMBER i2str(vt)

#endif

bool motordebugger::getfreeterm() {
    int fd, vt;
    strstream tstr;
    ofstream f;
    bool r;

    r = true;
    termname = forcedtty;

    if(termname.empty()) {
	if(r = ((fd = open("/dev/tty", O_WRONLY, 0)) != -1)) {
	    if(ioctl(fd, VT_OPENQRY, &vt) >= 0)
	    if(r = (vt != -1)) {
		tstr << TTY_PREFIX << TTY_NUMBER;
		termname = tstr.str();
		termname.resize(tstr.pcount());
		termnumber = vt;
	    }

	    close(fd);
	}
    }

    if(r) {
	struct stat st;

	if(!stat(termname.c_str(), &st))
	if(st.st_mode & S_IFLNK) {
	    auto_ptr<char> rlbuf(new char[64]);
	    if(!readlink(termname.c_str(), rlbuf.get(), 512))
		termname = rlbuf.get();
	}

	if(r = !access(termname.c_str(), W_OK)) {
	    f.open((project.getrootdir() + "/.debugtty").c_str());
	    if(f.is_open()) {
		f << termname << endl;
		f.close();
	    }
	}
    }

    return r;
}

void motordebugger::init() {
    int inpipe[2], outpipe[2], fd, st;
    struct timeval tv;
    fd_set fds;

    if(project.getdebuggermode() != projectdebug::dm_normal) {
	ui.log(_("Debugging is not supported by the project template"));
	return;
    }

    if(factive) return;

    ui.setdebugcurrentline(motorui::editfile());

    if(getfreeterm()) {
        if(!project.build()) {
	    done();
	    return;
	}

	if(!pipe(inpipe) && !pipe(outpipe)) {
	    if(!(cpid = fork())) {
		dup2(inpipe[1], STDOUT_FILENO);
		dup2(inpipe[1], STDERR_FILENO);
		dup2(outpipe[0], STDIN_FILENO);
            
        	close(inpipe[0]);
        	close(inpipe[1]);
        	close(outpipe[0]);
        	close(outpipe[1]);

		execl(findprogram(GNUMAKE).c_str(), GNUMAKE,
		    "-f", "Makefile.func", "debug", 0);

        	_exit(0);
    	    } else {
        	lfname = "";
        	factive = true;
        	fpread = fdopen(inpipe[0], "r");
        	fpwrite = fdopen(outpipe[1], "w");
		plist = pparamslist_create();
		project.populateparselist(plist);
        	readoutput();
		exec(project.getdebuginit());

        	sleep(1);
        	waitpid(cpid, &st, WNOHANG);

        	if(::kill(cpid, 0)) {	// The PID doesn't exist anymore
		    done();
		    ui.logf(_("Cannot debug %s"), project.getname().c_str());
		}
	    }
	}
    } else {
	if(!alwaysjustrun) {
	    alwaysjustrun = ui.ask("YN", _("No free tty to start a debug session. Run without debugging?")) == motorui::yes;
	}

	if(alwaysjustrun) {
	    run();
	}
    }
}

void motordebugger::run() {
    launch(debuggercommand::run);
}

void motordebugger::launch(debuggercommand::commandkind akind) {
    string cmd;

    rebuildasked = false;

    if(project.getdebuggermode() == projectdebug::dm_noexec) {
	ui.log(_("It's impossible to run this kind of projects"));
	return;
    }

    if((project.getdebuggermode() == projectdebug::dm_nodebug)
    || alwaysjustrun || (!factive && !conf.getchecktty())) {
	if(project.build()) {
	    project.chdir();
	    cmd = findprogram(GNUMAKE);
	    if(!cmd.empty()) {
		cmd += " -f Makefile.func run";
		ui.externalexec(EE_PAUSE, cmd);
	    }
	} else {
	    done();
	}
    } else {
	if(!factive) {
	    init();
	} else {
	    exec(project.getdebugonterminate());
	}

	if(factive) {
    	    frunning = true;
	    ui.logf(_("Running %s.. Ctrl-C to break"), project.getname().c_str());
	    setbreakpoints();
	    setparser("args", project.getarguments());
	    exec(akind);
	}
    }
}

void motordebugger::kill() {
    exec(debuggercommand::kill);
    ui.setdebugcurrentline(motorui::editfile());
    frunning = false;
    ui.logf(_("Killed %s"), project.getname().c_str());
}

void motordebugger::checkrebuildneed() {
    if(ui.editmodified() && !rebuildasked) {
	if(frunning) {
	    rebuildasked = true;

	    if(ui.ask("YN", _("The source was modified. Do you want to rebuild the program?")) == motorui::yes)
	    if(ui.autosave()) {
		done();
	    }
	} else {
	    done();
	}
    }
}

void motordebugger::cont() {
    checkrebuildneed();
    if(!factive) init();

    if(factive) {
        if(!frunning) {
            run();
        } else {
	    ui.logf(_("Continuing running %s.. Ctrl-C to break"), project.getname().c_str());
	    exec(debuggercommand::cont);
        }
    }
}

void motordebugger::gonext(debuggercommand::commandkind akind) {
    motorui::editfile oef, ef;

    checkrebuildneed();
    if(!factive) init();

    if(factive) {
        if(!frunning) {
            breakpoints.push_back(breakpoint());
//	    exec(debuggercommand::breakmain);
//	    launch(debuggercommand::run);
	    launch(debuggercommand::breakmain);
        } else {
	    ui.logf(_("Continuing running %s.. Ctrl-C to break"), project.getname().c_str());

            do {

                ef = motorui::editfile(lfname, 0, lline);
                exec(akind);

            } while((nodebug(lfname) || ((ef.fname == lfname) && (ef.y == lline))) && frunning && factive && fcommand);

                // loop while we're either in a no-debug file or
                // stuck on the same line

	    if(frunning) {
                ef = motorui::editfile(lfname, 0, lline);
                ui.setdebugcurrentline(ef);
                ui.logf(_("Debugger stopped"));
	        ui.updatewatches();
            }
        }
    }
}

void motordebugger::next() {
    gonext(debuggercommand::next);
}

void motordebugger::step() {
    gonext(debuggercommand::step);
}

void motordebugger::tocursor(const string &fname, int line) {
    checkrebuildneed();
    if(!factive) init();

    if(factive) {
	addbreakpoint(breakpoint("", fname, line, false));

        if(!frunning) {
            run();
        } else {
	    cont();
        }
    }
}

vector<stackitem> motordebugger::getstack() {
    string buf;
    vector<stackitem> r;

    ui.log(_("Reading the stack info.."));

    if(factive) {
	exec(debuggercommand::stack);

        if(lastdmsg)
	if(lastdmsg->getkind() == debuggermessage::stack) {
	    buf = outbuf;

	    while(*lastdmsg == buf) {
		r.push_back(stackitem(lastdmsg->getvalue("N"), lastdmsg->getvalue("A"),
		    motorui::editfile(lastdmsg->getvalue("F"), 0,
			atol(lastdmsg->getvalue("L").c_str()))));

		buf.erase(0, lastdmsg->getmatchendpos());
	    }
	}
    }

    if(!r.empty()) {
	ui.log(_("Stack info has been fetched"));
    } else {
	ui.log(_("Cannot read the stack"));
    }

    return r;
}

string motordebugger::loadcore(const string &corefname) {
    string r;

    if(frunning) kill();
    if(!factive) init();

    if(factive) {
	ui.log(_("Loading core dump.."));
	setparser("filename", corefname);
	exec(debuggercommand::loadcore);
	r = outbuf;
	ui.log(_("Core dump load done"));
    }

    return r;
}

void motordebugger::setvar(const string &vname, const string &vvalue) {
    if(factive && frunning) {
	setparser("var_name", vname);
	setparser("var_value", vvalue);
	exec(debuggercommand::setvar);
    }
}

string motordebugger::getvar(const string &vname) {
    string r;

    if(factive && frunning) {
	result = "";
	setparser("var_name", vname);
	exec(debuggercommand::getvar);
	r = result;
    }

    return r;
}

void motordebugger::readoutput() {
    char rbuf[514];
    int br, fd, psize;
    fd_set fds;
    struct timeval tv;
    vector<debuggermessage>::const_iterator im;

    fd = fileno(fpread);
    outbuf = "";

    if((im = find(project.dmsgbegin(), project.dmsgend(),
    debuggermessage::prompt)) != project.dmsgend()) {
	do {

	    FD_ZERO(&fds);
	    FD_SET(fd, &fds);
	    tv.tv_sec = 1;
	    tv.tv_usec = 0;

	    select(fd+1, &fds, 0, 0, &tv);

	    if(FD_ISSET(fd, &fds)) {
		if((br = dataready(fileno(fpread), 0)) > 0) {
		    while(br) {
			if((psize = br) > 512) psize = 512;
			psize = fread(rbuf, 1, psize, fpread);
			rbuf[psize] = 0;
			outbuf += rbuf;
			br -= psize;
		    }
		}
	    } else {
		switchterm(there);
	    }

	} while(*im != outbuf);

        outbuf.erase(outbuf.size()-im->getvalue(0).size());
    }

#ifdef DEBUG
    dlog << "read: " << outbuf << endl;
#endif

    switchterm(here);
}

void motordebugger::addbreakpoint(const breakpoint &bp) {
    vector<breakpoint>::iterator ib;

    if((ib = find(breakpoints.begin(), breakpoints.end(),
    pair<string, int>(bp.getfname(), bp.getline())))
    == breakpoints.end()) {
	breakpoints.push_back(bp);

	if(running()) {
	    setparser("filename", bp.getfname());
	    setparser("line", i2str(bp.getline()));
	    exec(debuggercommand::breakpoint);
	}
    }
}

void motordebugger::removebreakpoint(const breakpoint &bp) {
    vector<breakpoint>::iterator ib;

    if((ib = find(breakpoints.begin(), breakpoints.end(), pair<string, int>(bp.getfname(), bp.getline()))) != breakpoints.end()) {
	breakpoints.erase(ib);

	if(running()) {
	    setparser("filename", bp.getfname());
	    setparser("line", i2str(bp.getline()));
	    exec(debuggercommand::breakpointdel);
	}
    }
}

bool motordebugger::isbreakpoint(const breakpoint &bp) const {
    return find(breakpoints.begin(), breakpoints.end(), pair<string, int>(bp.getfname(), bp.getline())) != breakpoints.end();
}

vector<breakpoint> motordebugger::getbreakpoints() const {
    vector<breakpoint> r;
    vector<breakpoint>::const_iterator ib;

    for(ib = breakpoints.begin(); ib != breakpoints.end(); ib++)
	if(ib->permanent())
	    r.push_back(*ib);

    return r;
}

void motordebugger::setbreakpoints() {
    vector<breakpoint> bps;
    vector<breakpoint>::iterator i;

    bps = breakpoints;
    breakpoints.clear();

    for(i = bps.begin(); i != bps.end(); i++) {
	addbreakpoint(*i);
    }
}

void motordebugger::exec(const string &cmd) {
    if(factive) {
	fprintf(fpwrite, "%s\n", cmd.c_str());
	fflush(fpwrite);

#ifdef DEBUG
	dlog << "wrote: " << cmd << endl;
#endif

	readoutput();
	getlocation();
	react();
    }
}

void motordebugger::exec(debuggercommand::commandkind akind) {
    vector<debuggercommand>::const_iterator i;

    fcommand = true;
    i = project.dcmdbegin();

    while(((i = find(i, project.dcmdend(), akind)) != project.dcmdend())
    && fcommand) {
	exec(i->get(plist));
	i++;
    }
}

void motordebugger::exec(const vector<string> cmds) {
    vector<string>::const_iterator i;
    char *pbuf;

    for(i = cmds.begin(); i != cmds.end(); i++) {
	pbuf = sstrparse(i->c_str(), plist);
	exec(pbuf);
	delete pbuf;
    }
}

void motordebugger::done() {
    if(factive) {
	exec(project.getdebugonterminate());

	if(fpread) fclose(fpread);
	if(fpwrite) fclose(fpwrite);

        ui.setdebugcurrentline(motorui::editfile());
	pparamslist_free(plist);
	factive = frunning = false;
        ui.updatewatches();
	::kill(cpid, SIGTERM);
	waitpid(cpid, 0, 0);
    }
}

void motordebugger::getlocation() {
    vector<debuggermessage>::const_iterator im;

    if((im = find(project.dmsgbegin(), project.dmsgend(), debuggermessage::location)) != project.dmsgend()) {
	if(*im == outbuf) {
	    lfname = im->getvalue("F");
	    lline = atol(im->getvalue("L").c_str());
	}
    }

    im = project.dmsgbegin();
    lastdmsg = 0;

    while((im = find(im, project.dmsgend(), outbuf)) != project.dmsgend())
	switch(im->getkind()) {
	    case debuggermessage::prompt:
	    case debuggermessage::location:
	    case debuggermessage::stdheader:
		im++;
		break;
	    default:
		lastdmsg = &(*im);
		im = project.dmsgend();
		break;
	}
}

void motordebugger::react() {
    breakpoint bp;
    string fname, id;
    int line;
    vector<breakpoint>::iterator ib;

    if(lastdmsg) {
        fname = lastdmsg->getvalue("F");
        line = atol(lastdmsg->getvalue("L").c_str());
        id = lastdmsg->getvalue("I");

        switch(lastdmsg->getkind()) {
            case debuggermessage::setbreakpoint:
	        if(breakpoints.size()) {
                    bp = *(breakpoints.end()-1);
                    breakpoints.erase(breakpoints.end()-1);

                    if(fname.empty()) fname = bp.getfname();
                    if(!line) line = bp.getline();

                    breakpoints.push_back(breakpoint(id, fname, line, bp.permanent()));
	        }
                break;

            case debuggermessage::breakpoint:
                if((ib = find(breakpoints.begin(), breakpoints.end(), id)) != breakpoints.end()) {
                    bp = *ib;

                    if(bp.permanent()) {
                        if(fname.empty()) fname = bp.getfname();
                        if(fname.empty()) fname = lfname;

                        if(!line) line = bp.getline();
                        if(!line) line = lline;

                	breakpoints.erase(ib);
                        breakpoints.push_back(breakpoint(id, fname, line, bp.permanent()));
                    } else {
			removebreakpoint(bp);
		    }
                }

                ui.setdebugcurrentline(motorui::editfile(lfname, 0, lline));
		ui.logf(_("Stopped at breakpoint %s"), id.c_str());
	        ui.updatewatches();
                break;

            case debuggermessage::terminated:
		ui.logf(_("Terminated %s"), project.getname().c_str());
                done();
                frunning = false;
                break;

            case debuggermessage::cantstart:
		ui.logf(_("Cannot start %s"), project.getname().c_str());
                done();
                break;

            case debuggermessage::interrupted:
		result = lastdmsg->getvalue(0);
		ui.logf(_("Interrupted: %s"), result.c_str());
	        ui.updatewatches();
		fcommand = false;
                break;

            case debuggermessage::variablevalue:
		result = lastdmsg->getvalue(0);
                break;
        }
    }
/*
    if(factive) {
        ui.setdebugcurrentline(motorui::editfile(lfname, 0, lline));
    }
*/
}

bool motordebugger::nodebug(const string &fname) {
    vector<debuggermessage>::const_iterator im;
    bool r = false;

    if(!conf.getdebugstd())
    if((im = find(project.dmsgbegin(), project.dmsgend(), debuggermessage::stdheader)) != project.dmsgend()) {
        r = *im == fname;
    }

    return r;
}

void motordebugger::setparser(const string &name, const string &value) {
    pparamslist_set(name.c_str(), value.c_str(), plist);
}

void motordebugger::switchterm(termswitchdirection d) {
    static int vtfd = -1;
    static int curvt;

    switch(d) {
	case there:
            if(vtfd == -1)
            if((vtfd = open("/dev/tty", O_WRONLY, 0)) != -1) {
                curvt = atol(getcurtty()+strlen(TTY_PREFIX));
                ioctl(vtfd, VT_ACTIVATE, termnumber);
                ioctl(vtfd, VT_WAITACTIVE, termnumber);
            }
	    break;

	case here:
            if(vtfd != -1) {
                ioctl(vtfd, VT_ACTIVATE, curvt);
	        ioctl(vtfd, VT_WAITACTIVE, curvt);
	        close(vtfd);
                vtfd = -1;
            }
	    break;
    }
}

bool motordebugger::running() const {
    return frunning && factive;
}

string motordebugger::getfname() const {
    return lfname;
}

int motordebugger::getline() const {
    return lline;
}

void motordebugger::addwatch(const string &wexpr) {
    watches.push_back(wexpr);
}

vector<string> motordebugger::getwatches() const {
    return watches;
}

void motordebugger::removewatch(const string &wexpr) {
    vector<string>::iterator i;

    if((i = find(watches.begin(), watches.end(), wexpr)) != watches.end()) {
	watches.erase(i);
    }
}

bool motordebugger::iswatch(const string &wexpr) const {
    return find(watches.begin(), watches.end(), wexpr) != watches.end();
}

void motordebugger::forceusetty(const string &tname) {
    forcedtty = tname;
}
