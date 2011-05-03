#include "projectdebug.h"
#include "motormisc.h"
#include "motorconfiguration.h"
#include "motordebugger.h"

projectdebug::projectdebug() {
}

projectdebug::projectdebug(const projectname aname) {
    load(aname);
}

projectdebug::~projectdebug() {
}

vector<string> projectdebug::getdebuginit() const {
    return cmdinit;
}

vector<string> projectdebug::getdebugonterminate() const {
    return cmdonterminate;
}

bool projectdebug::load(const projectname aname) {
    ifstream df;
    string sect, buf;
    breakpoint bp;
    vector<debuggermessage>::iterator idmsg;
    debuggercommand::commandkind dcmdkind;
    debuggermessage::messagekind dmsgkind;
    bool ret;

    fdm = dm_normal;
    bpoints.clear();
    watches.clear();

//    projectname::absorb(aname);
    pname = aname;
    df.open((conf.gettemplatedir(pname.gettemplatename()) + "/Debug").c_str());

    if(ret = df.is_open()) {
	while(getconf(sect, buf, df)) {
	    if(sect == "options") {
		if(buf == "nodebug") fdm = dm_nodebug; else
		if(buf == "noexec") fdm = dm_noexec;
	    } else if(sect == "init") {
		cmdinit.push_back(buf);
	    } else if(sect == "onterminate") {
		cmdonterminate.push_back(buf);
	    } else if(sect.substr(0, 3) == "cmd") {
		sect.erase(0, 3);
	        if(sect == "breakmain") dcmdkind = debuggercommand::breakmain; else
		if(sect == "breakpoint") dcmdkind = debuggercommand::breakpoint; else
		if(sect == "kill") dcmdkind = debuggercommand::kill; else
		if(sect == "next") dcmdkind = debuggercommand::next; else
		if(sect == "step") dcmdkind = debuggercommand::step; else
		if(sect == "run") dcmdkind = debuggercommand::run; else
		if(sect == "continue") dcmdkind = debuggercommand::cont; else
		if(sect == "loadcore") dcmdkind = debuggercommand::loadcore; else
		if(sect == "stack") dcmdkind = debuggercommand::stack; else
		if(sect == "breakpointdel") dcmdkind = debuggercommand::breakpointdel; else
		if(sect == "setvar") dcmdkind = debuggercommand::setvar; else
		if(sect == "getvar") dcmdkind = debuggercommand::getvar; else
	    	    continue;

		dcommands.push_back(debuggercommand(buf, dcmdkind));
	    } else if(sect.substr(0, 3) == "msg") {
		sect.erase(0, 3);
		if(sect == "terminated") dmsgkind = debuggermessage::terminated; else
		if(sect == "cantstart") dmsgkind = debuggermessage::cantstart; else
		if(sect == "interrupted") dmsgkind = debuggermessage::interrupted; else
		if(sect == "breakpoint") dmsgkind = debuggermessage::breakpoint; else
		if(sect == "setbreakpoint") dmsgkind = debuggermessage::setbreakpoint; else
		if(sect == "value") dmsgkind = debuggermessage::variablevalue; else 
		if(sect == "prompt") dmsgkind = debuggermessage::prompt; else
		if(sect == "stdheader") dmsgkind = debuggermessage::stdheader; else
		if(sect == "location") dmsgkind = debuggermessage::location; else
		if(sect == "stack") dmsgkind = debuggermessage::stack; else
	    	    continue;

		if((idmsg = find(dmessages.begin(), dmessages.end(), dmsgkind)) == dmessages.end()) {
	    	    dmessages.push_back(dmsgkind);
	    	    idmsg = dmessages.end()-1;
		}

		idmsg->definitionadd(buf);
	    }
        }

        df.close();
    }

    df.clear();
    df.open(pname.getregfname().c_str());

    if(df.is_open()) {
	while(getconf(sect, buf, df)) {
	    if(sect == "breakpoints") {

		try {
		    bp = breakpoint(buf);
		    bpoints.push_back(bp);
		} catch(...) {
		}

	    } else if(sect == "watch") {
		watches.push_back(buf);
	    }
	}

	df.close();
    }

    return ret;
}

void projectdebug::populateparselist(pparamslist *parselist) {
}

vector<debuggercommand>::const_iterator projectdebug::dcmdbegin() const {
    return dcommands.begin();
}

vector<debuggercommand>::const_iterator projectdebug::dcmdend() const {
    return dcommands.end();
}

vector<debuggermessage>::const_iterator projectdebug::dmsgbegin() const {
    return dmessages.begin();
}

vector<debuggermessage>::const_iterator projectdebug::dmsgend() const {
    return dmessages.end();
}

void projectdebug::debugload() {
    vector<breakpoint>::iterator ib;
    vector<string>::iterator iw;

    for(ib = bpoints.begin(); ib != bpoints.end(); ++ib) {
	debugger.addbreakpoint(*ib);
    }

    for(iw = watches.begin(); iw != watches.end(); ++iw) {
	debugger.addwatch(*iw);
    }
}

projectdebug::debuggermode projectdebug::getdebuggermode() const {
    return fdm;
}
