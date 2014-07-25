#include "parserule.h"

#include <iostream>

parserule::parserule(const string &atarget, const string &amask, messagekind akind) {
    string tmbuf;
    INT npos;

    target = atarget;
    kind = akind;
    tmbuf = mask = amask;
    rexcompiled = false;

    porder = getrword(mask);
    if(porder.find_first_not_of("INKLFDEW") != -1) {
	mask = tmbuf;
        porder = "";
    }

    while((npos = porder.find_first_of("EW")) != -1) {
	if(porder[npos] == 'E') kind = error; else
	if(porder[npos] == 'W') kind = warning;
	porder.erase(npos, 1);
    }
}

parserule::parserule(const parserule &p) {
    mask = p.mask;
    porder = p.porder;
    target = p.target;
    kind = p.kind;
    rexcompiled = false;
}

parserule::~parserule() {
    if(rexcompiled) {
	regfree(&rex);
    }
}

bool parserule::exec(const string &buf) const {
    bool ret = false;
    INT i;
    regmatch_t rexm[10];

    if(!rexcompiled)
	rexcompiled = !regcomp(&rex, mask.c_str(), REG_EXTENDED);

    if(ret = rexcompiled) {
	if(ret = !regexec(&rex, buf.c_str(), 10, rexm, 0)) {
            values.clear();
	    for(i = 1; (i < 10) && (rexm[i].rm_so != -1); i++)
	        values.push_back(buf.substr(rexm[i].rm_so,
                    rexm[i].rm_eo-rexm[i].rm_so));
        }
    }

    return ret;
}

string parserule::getvalue(const string &n) const {
    string r;
    INT npos = 0;

    if(!n.empty()) npos = porder.find(n);
    if(npos >= 0 && npos < values.size()) r = values[npos];

    return r;
}

parserule::messagekind parserule::getkind() const {
    return kind;
}

bool parserule::operator == (const pair<string, string> &ast) const {
    return ((target == ast.second) || target.empty()) && exec(ast.first);
}

bool parserule::operator != (const pair<string, string> &ast) const {
    return !(*this == ast);
}
