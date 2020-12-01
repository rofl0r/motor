#include "motorexecutor.h"
#include "motorproject.h"
#include "motormisc.h"
#include "motorui.h"
#include "motorconfiguration.h"

#include <sys/wait.h>

motorexecutor::motorexecutor() {
}

motorexecutor::~motorexecutor() {
}

void motorexecutor::piperead() {
    char cbuf[512];
    string buf;
    message msg;
    vector<parserule>::const_iterator i;

    ui.writeoutput(_("Executing ") + target);

    lastbuf = "";
    lastmessages.clear();

    while(!feof(fpipe)) {
        cbuf[0] = 0;
	fgets(cbuf, 512, fpipe);
	lastbuf += cbuf;

	buf = trailcut(buf = cbuf, "\n");
	ui.writeoutput(buf);

	if((i = find(project.parsebegin(), project.parseend(),
	pair<string, string>(buf, target))) != project.parseend()) {
	    msg.fname = i->getvalue("F");
	    msg.line = atol(i->getvalue("L").c_str());
	    msg.description = i->getvalue("D");

	    switch(msg.kind = i->getkind()) {
		case parserule::error: nerr++; break;
		case parserule::warning: nwarn++; break;
		case parserule::info: ninfo++; break;
	    }

            lastmessages.push_back(msg);
	}
    }
}

bool motorexecutor::runmake(const string &atarget) {
    int cpid, cp[2];
    bool rc;

    setdefaultvars();
    project.chdir();
    target = atarget;
    nerr = nwarn = ninfo = 0;

    if(rc = !pipe(cp)) {
	if(!(cpid = fork())) {
            dup2(cp[1], STDOUT_FILENO);
            dup2(cp[1], STDERR_FILENO);
            close(cp[1]);
	    close(cp[0]);

	    pop();

	    execlp("make", "make",
		"-f", "Makefile.func",
		target.c_str(), 0);

            _exit(0);
        } else {
            close(cp[1]);

            if(rc = (fpipe = fdopen(cp[0], "r"))) {
		piperead();
		fclose(fpipe);
        	close(cp[0]);
	    }

            waitpid(cpid, 0, 0);

	    ui.doneoutput();
	    ui.executordone(target, nerr, nwarn, ninfo);

    	    rc = !nerr;
	}
    }

    return rc;
}

void motorexecutor::pop() {
    vector< pair<string, string> >::iterator i;

    for(i = variables.begin(); i != variables.end(); i++) {
	setenv(i->first.c_str(), i->second.c_str(), 1);
    }
}

void motorexecutor::setvar(const string &aname, const string &avalue) {
    variables.push_back(pair<string, string>(aname, avalue));
}

void motorexecutor::clearvars() {
    variables.clear();
}

vector<motorexecutor::message>::const_iterator motorexecutor::begin() const {
    return lastmessages.begin();
}

vector<motorexecutor::message>::const_iterator motorexecutor::end() const {
    return lastmessages.end();
}

string motorexecutor::getlastbuf() const {
    return lastbuf;
}

void motorexecutor::setdefaultvars() {
    vector<motorui::editfile> flst = ui.geteditfiles();
    if(!flst.empty()) {
	setvar("MOTOR_CURRENTFILE", flst.back().fname);
    }
}
