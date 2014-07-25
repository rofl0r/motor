#include "debuggermessage.h"

debuggermessage::debuggermessage(messagekind akind) {
    kind = akind;
}

debuggermessage::~debuggermessage() {
}

void debuggermessage::definitionadd(const string &text) {
    string porder, buf = text;

    porder = getrword(buf);
    if(porder.find_first_of("ILF") == -1) {
        porder = "";
	buf = text;
    }

    lines.push_back(pair<string, string>(buf, porder));
}

string debuggermessage::getvalue(const string &paramname) const {
    string r;
    INT npos = porder.find(paramname);
    if(npos >= 0 && npos < values.size()) r = values[npos];
    return r;
}

string debuggermessage::getvalue(INT n) const {
    string r;
    if(n >= 0 && n < values.size()) r = values[n];
    return r;
}

debuggermessage::messagekind debuggermessage::getkind() const {
    return kind;
}

bool debuggermessage::operator == (const messagekind akind) const {
    return kind == akind;
}

bool debuggermessage::operator != (const messagekind akind) const {
    return !(*this == akind);
}

bool debuggermessage::operator == (const string &amsg) const {
    regex_t r;
    regmatch_t rm[10];
    bool rc;
    INT k;
    vector< pair<string, string> >::const_iterator i;

    rc = false;
    values.clear();

    for(i = lines.begin(); i != lines.end() && !rc; i++) {
	if(!regcomp(&r, i->first.c_str(), REG_EXTENDED)) {
	    if(rc = !regexec(&r, amsg.c_str(), 10, rm, 0)) {
		porder = i->second;

		for(k = 1; (k < 10) && (rm[k].rm_so != -1); k++) {
		    values.push_back(amsg.substr(rm[k].rm_so,
			rm[k].rm_eo-rm[k].rm_so));
		}

		matchend = rm[0].rm_eo;
	    }

	    regfree(&r);
	}
    }

    return rc;
}

bool debuggermessage::operator != (const string &amsg) const {
    return !(*this == amsg);
}

INT debuggermessage::getmatchendpos() const {
    return matchend;
}
