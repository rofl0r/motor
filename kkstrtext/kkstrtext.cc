/*
*
* kkstrtext string related and text processing routines
* $Id: kkstrtext.cc,v 1.45 2005/02/01 00:13:24 konst Exp $
*
* Copyright (C) 1999-2004 by Konstantin Klyagin <konst@konst.org.ua>
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

#include "kkstrtext.h"
#include <errno.h>

char *strcut(char *strin, int frompos, int count) {
    if(count > 0) {
	if(count > strlen(strin)-frompos) count = strlen(strin)-frompos;
	char *buf = (char *) malloc(strlen(strin) - frompos - count + 1);
	memcpy(buf, strin + frompos + count, strlen(strin) - frompos - count);
	memcpy(strin + frompos, buf, strlen(strin) - frompos - count);
	strin[strlen(strin) - count] = 0;
	free(buf);
    }
    return strin;
}

char *strimlead(char *str)  { return trimlead(str, " \t");  }
char *strimtrail(char *str) { return trimtrail(str, " \t"); }
char *strim(char *str)      { return trim(str, " \t");      }

char *trimlead(char *str, char *chr) {
    while(strchr(chr, str[0]) && strlen(str)) strcpy(str, str + 1);
    return str;
}

char *trimtrail(char *str, char *chr) {
    while(strchr(chr, str[strlen(str)-1]) && strlen(str)) str[strlen(str)-1] = 0;
    return str;
}

char *trim(char *str, char *chr) {
    return trimlead(trimtrail(str, chr), chr);
}

char *time2str(const time_t *t, char *mask, char *sout) {
    struct tm *s;
    char ch, b[10], b1[20];
    int len, i, j;

    sout[0] = 0;
    s = localtime(t);

    for(i = 0; i < strlen(mask); i++) {
	len = 0;
    
	if(strchr("DMYhms", ch = mask[i])) {
	    j = i; len = 1;
	    while(mask[++j] == ch) len++;
	    sprintf(b, "%%0%dd", len);
	    i += len-1;
      
	    switch(ch) {
		case 'D': sprintf(b1, b, s->tm_mday); break;
		case 'M': sprintf(b1, b, s->tm_mon+1); break;
		case 'Y':
		    j = s->tm_year + 1900;
		    sprintf(b1, b, j);
		    if(len <= 3) strcut(b1, 0, 2);
		    break;
		case 'h': sprintf(b1, b, s->tm_hour); break;
		case 'm': sprintf(b1, b, s->tm_min); break;
		case 's': sprintf(b1, b, s->tm_sec); break;
	    }
	    strcat(sout, b1);
	} else {
	    len = strlen(sout);
	    sout[len+1] = 0;
	    sout[len] = mask[i];
	}
    }
    return sout;
}

time_t str2time(char *sdate, char *mask, time_t *t) {
    struct tm *s;
    int i, len, j, k;
    char ch, b[10];

    s = (struct tm*) malloc(sizeof(struct tm));
  
    for(i = 0; i < strlen(mask); i++) {
	len = 0;
    
	if(strchr("DMYhms", ch = mask[i])) {
	    j = i; len = 1;
	    while(mask[++j] == ch) len++;
	    i += len-1;

	    b[0] = 0;
	    for(j = i-len+1; j < i+1; j++) {
		k = strlen(b);
		b[k+1] = 0;
		b[k] = sdate[j];
	    }
      
	    switch(ch) {
		case 'D': s->tm_mday=atoi(b); break;
		case 'M': s->tm_mon=atoi(b); s->tm_mon--; break;
		case 'Y': s->tm_year=atoi(b); s->tm_year-=1900; break;
		case 'h': s->tm_hour=atoi(b); s->tm_hour--; break;
		case 'm': s->tm_min=atoi(b); break;
		case 's': s->tm_sec=atoi(b); break;
	    }
	}
    }

    s->tm_isdst = -1;
    *t = mktime(s);
    free(s);
    return *t;
}

string unmime(const string &text) {
    string r;
    char *buf = new char[text.size()+1];
    strcpy(buf, text.c_str());
    r = unmime(buf);
    delete[] buf;
    return r;
}

string mime(const string &text) {
    string r;
    char *buf = new char[text.size()*3+1];
    r = mime(buf, text.c_str());
    delete[] buf;
    return r;
}

string fromutf8(const string &text) {
    string r;
    char *buf = (char *) utf8_to_str(text.c_str());
    r = buf;
    delete buf;
    return r;
}

string toutf8(const string &text) {
    string r;
    char *buf = str_to_utf8(text.c_str());
    r = buf;
    delete buf;
    return r;
}

char *unmime(char *text) {
    register int s, d;
    int htm;

    for(s = 0, d = 0; text[s] != 0; s++) {
	if(text[s] == '+') text[d++] = ' '; else
	if(text[s] == '%') {
	    sscanf(text + s + 1, "%2x", &htm);
	    text[d++] = htm;
	    s += 2;
	} else
	    text[d++] = text[s];
    }

    text[d] = 0;
    return(text);
}

char *mime(char *dst, const char *src) {
    register int s, d;
    char c;

    for(s = 0, d = 0; src[s]; s++) {
	if((src[s] >= 'a' && src[s] <= 'z') || 
	   (src[s] >= 'A' && src[s] <= 'Z') ||
	   (src[s] >= '0' && src[s] <= '9')) dst[d++] = src[s]; else {
	    if(src[s] != ' ') {
		dst[d++] = '%';
		c = (src[s] >> 4 & 0x0F);
		dst[d++] = (c > 9) ? 'A'+c-10 : '0'+c;
		c = (src[s] & 0x0F);
		dst[d++] = (c > 9) ? 'A'+c-10 : '0'+c;
	    } else
		dst[d++] = '+';
	}
    }
  
    dst[d] = '\0';
    return(dst);
}

char *strccat(char *dest, char c) {
    int k = strlen(dest);
    dest[k] = c;
    dest[k+1] = 0;
    return dest;
}

vector<int> getquotelayout(const string &haystack, const string &qs, const string &aescs) {
    vector<int> r;
    string needle, escs;
    int pos, prevpos, curpos;
    char cchar, qchar, prevchar;

    qchar = 0;
    curpos = prevpos = 0;
    escs = (qs == aescs) ? "" : aescs;
    needle = qs + escs;

    while((pos = haystack.substr(curpos).find_first_of(needle)) != -1) {
	curpos += pos;
	cchar = *(haystack.begin()+curpos);

	if(escs.find(cchar) != -1) {
	    if(qchar)
	    if(prevpos == curpos-1)
	    if(escs.find(prevchar) != -1) {
		/* Neutralize previous esc char */
		cchar = 0;
	    }
	} else if(qs.find(cchar) != -1) {
	    if(!((escs.find(prevchar) != -1) && (prevpos == curpos-1))) {
		/* Wasn't an escape (right before this quote char) */

		if(!qchar || (qchar == cchar)) {
		    qchar = qchar ? 0 : cchar;
		    r.push_back(curpos);
		}
	    }
	}

	prevpos = curpos++;
	prevchar = cchar;
    }

    return r;
}

vector<int> getsymbolpositions(const string &haystack, const string &needles, const string &qoutes, const string &esc) {
    vector<int> r, qp, nr;
    vector<int>::iterator iq, ir;
    int pos, st, ed, cpos;

    for(cpos = 0; (pos = haystack.substr(cpos).find_first_of(needles)) != -1; ) {
	r.push_back(cpos+pos);
	cpos += pos+1;
    }

    qp = getquotelayout(haystack, qoutes, esc);
    for(iq = qp.begin(); iq != qp.end(); iq++) {
	if(!((iq-qp.begin()) % 2)) {
	    st = *iq;
	    ed = iq+1 != qp.end() ? *(iq+1) : haystack.size();
	    nr.clear();

	    for(ir = r.begin(); ir != r.end(); ir++) {
		if(!(*ir > st && *ir < ed)) {
		    nr.push_back(*ir);
		}
	    }

	    r = nr;
	}
    }

    return r;
}

#define CHECKESC(curpos, startpos, esc) \
    if(curpos > startpos+1) \
    if(strchr(esc, *(curpos-1))) \
    if(!strchr(esc, *(curpos-2))) { \
	curpos++; \
	continue; \
    }

const char *strqpbrk(const char *s, int offset, const char *accept, const char *q, const char *esc) {
    if(!s) return 0;
    if(!s[0]) return 0;

    char qchar = 0;
    const char *ret = 0, *p = s;
    char *cset = (char *) malloc(strlen(accept)+strlen(q)+1);
    
    strcpy(cset, accept);
    strcat(cset, q);

    while(p = strpbrk(p, cset)) {
	if(strchr(q, *p)) {
	    if(strcmp(esc, q))
		CHECKESC(p, s, esc);

	    if(!qchar) {
		qchar = *p;
	    } else {
		if(*p == qchar) qchar = 0;
	    }
	} else if((p-s >= offset) && !qchar) {
	    ret = p;
	    break;
	}
	p++;
    }

    free(cset);
    return ret;
}

const char *strqcasestr(const char *s, const char *str, const char *q, const char *esc) {
    char quote = 0;
    int i;

    for(i = 0; i < strlen(s); i++) {
	if(strchr(q, s[i])) {
	    if(strcmp(esc, q))
		CHECKESC(s+i, s, esc);
	    quote = !quote;
	}

	if(!quote)
	if(!strncasecmp(s + i, str, strlen(str))) return s + i;
    }

    return 0;
}

const char *strqstr(const char *s, const char *str, const char *q, const char *esc) {
    char quote;
    const char *ret = 0, *p, *ss, *r;
    p = ss = s;

    while(p = strstr(ss, str)) {
	quote = 0;
	r = s;

	while(r = strpbrk(r, q)) {
	    if(r > p) break;
	    if(strcmp(esc, q))
		CHECKESC(r, s, esc);
	    quote = !quote;
	    r++;
	}
	
	if(!quote) {
	    ret = p;
	    break;
	} else {
	    ss = p+strlen(str);
	}
    }

    return ret;
}

char *strinsert(char *buf, int pos, char *ins) {
    char *p = strdup(buf+pos);
    memcpy(buf+pos+strlen(ins), p, strlen(p)+1);
    memcpy(buf+pos, ins, strlen(ins));
    free(p);
    return buf;
}

char *strcinsert(char *buf, int pos, char ins) {
    char *p = strdup(buf+pos);
    memcpy(buf+pos+1, p, strlen(p)+1);
    buf[pos] = ins;
    free(p);
    return buf;
}

int strchcount(char *s, char *accept) {
    char *p = s-1;
    int ret = 0;
    while(p = strpbrk(p+1, accept)) ret++;
    return ret;
}

int stralone(char *buf, char *startword, int wordlen, char *delim) {
    int leftdelim = 0, rightdelim = 0;
    leftdelim = (buf != startword && strchr(delim, *(startword-1))) || buf == startword;
    rightdelim = !*(startword+wordlen) || strchr(delim, *(startword+wordlen));
    return leftdelim && rightdelim;
}

string justfname(const string &fname) {
    return fname.substr(fname.rfind("/")+1);
}

string justpathname(const string &fname) {
    int pos;
    
    if((pos = fname.rfind("/")) != -1) {
	return fname.substr(0, pos);
    } else {
	return "";
    }
}

void charpointerfree(void *p) {
    char *cp = (char *) p;
    if(cp) delete cp;
}

void nothingfree(void *p) {
}

int stringcompare(void *s1, void *s2) {
    if(!s1 || !s2) {
	return s1 != s2;
    } else {
	return strcmp((char *) s1, (char *) s2);
    }
}

int intcompare(void *s1, void *s2) {
    return (int) s1 != (int) s2;
}

string i2str(int i) {
    char buf[64];
    sprintf(buf, "%d", i);
    return (string) buf;
}

string ui2str(int i) {
    char buf[64];
    sprintf(buf, "%du", i);
    return (string) buf;
}

string textscreen(const string &text) {
    string r = text;

    for(int i = 0; i < r.size(); i++) {
	if(!isalnum(r[i])) r.insert(i++, "\\");
    }   

    return r;
}

string leadcut(const string &base, const string &delim) {
    int pos = base.find_first_not_of(delim);
    return (pos != -1) ? base.substr(pos) : "";
}

string trailcut(const string &base, const string &delim) {
    int pos = base.find_last_not_of(delim);
    return (pos != -1) ? base.substr(0, pos+1) : "";
}

string getword(string &base, const string &delim) {
    string sub;
    int i;
    bool found = false;

    base = leadcut(base, delim);
    
    for(i = 0, sub = base; i < sub.size(); i++)
    if(strchr(delim.c_str(), sub[i])) {
	sub.resize(i);
	base.replace(0, i, "");
	base = leadcut(base, delim);
	found = true;
	break;
    }

    if(!found) base = "";
    return sub;
}

const string getwordquote(string &base, string quote, string delim) {
    string sub;
    bool inquote = false;
    int i;

    base = leadcut(base, delim);

    for(i = 0, sub = base; i < sub.size(); i++) {
	if(strchr(quote.c_str(), sub[i])) {
	    inquote = !inquote;
	} else if(!inquote && strchr(delim.c_str(), sub[i])) {
	    sub.resize(i);
	    base.replace(0, i, "");
	    base = leadcut(base, delim);
	    break;
	}
    }

    if(sub == base) base = "";
    return sub;
}

string getrword(string &base, const string &delim) {
    string sub;
    int i;

    base = trailcut(base, delim);
    
    for(i = base.size()-1, sub = base; i >= 0; i--)
    if(strchr(delim.c_str(), base[i])) {
	sub = base.substr(i+1);
	base.resize(i);
	base = trailcut(base, delim);
	break;
    }

    if(sub == base) base = "";
    return sub;
}

string getrwordquote(string &base, const string &quote, const string &delim) {
    string sub;
    bool inquote = false;
    int i;

    base = trailcut(base, delim);
    
    for(i = base.size()-1, sub = base; i >= 0; i--)
    if(strchr(quote.c_str(), base[i])) {
	inquote = !inquote;
    } else if(!inquote && strchr(delim.c_str(), base[i])) {
	sub = base.substr(i+1);
	base.resize(i);
	base = trailcut(base, delim);
	break;
    }

    if(sub == base) base = "";
    return sub;
}

int rtabmargin(bool fake, int curpos, const char *p) {
    int ret = -1, n, near;

    if(p && (curpos != strlen(p))) {
	n = strspn(p+curpos, " ");

	if(fake) {
	    near = ((curpos/(TAB_SIZE/2))+1)*(TAB_SIZE/2);
	    if(n >= near-curpos) ret = near;
	}

	near = ((curpos/TAB_SIZE)+1)*TAB_SIZE;
	if(n >= near-curpos) ret = near;
    } else {
	if(p && fake) fake = (strspn(p, " ") == strlen(p));
	if(fake) ret = ((curpos/(TAB_SIZE/2))+1)*(TAB_SIZE/2);
	else ret = ((curpos/TAB_SIZE)+1)*TAB_SIZE;
    }

    return ret;
}

int ltabmargin(bool fake, int curpos, const char *p) {
    int ret = -1, near, n = 0;
    const char *cp;
    
    if(p) {
	cp = p+curpos;

	if(curpos) {
	    if(*(--cp) == ' ') n++;
	    for(; (*cp == ' ') && (cp != p); cp--) n++;
	}

	if(fake) {
	    near = (curpos/(TAB_SIZE/2))*(TAB_SIZE/2);
	    if(near <= curpos-n)
	    if((ret = curpos-n) != 0) ret++;
	}

	near = (curpos/TAB_SIZE)*TAB_SIZE;
	if(near <= curpos-n) {
	    if((ret = curpos-n) != 0) ret++;
	} else ret = near;
	
    } else {
	if(fake) ret = (curpos/(TAB_SIZE/2))*(TAB_SIZE/2);
	else ret = (curpos/TAB_SIZE)*TAB_SIZE;
    }
    
    return ret;
}

void breakintolines(string text, vector<string> &lst, int linelen) {
    int dpos, nlen;
    string sub;
    vector<string>::iterator i;

    breakintolines(text, lst);

    if(linelen > 0) {
	for(i = lst.begin(); i != lst.end(); i++) {
	    if(i->size() > linelen) {
		sub = i->substr(0, nlen = linelen);

		if((dpos = sub.rfind(" ")) != -1) {
		    if(dpos) nlen = dpos; else nlen = 1;
		}

		if(dpos != -1)
		    nlen++;

		sub = i->substr(nlen);
		i->erase(nlen);
		lst.insert(i+1, sub);
		i = lst.begin();
	    }
	}
    }
}

void breakintolines(const string &text, vector<string> &lst) {
    int npos, dpos, tpos;
    string sub;

    tpos = 0;
    lst.clear();

    while(tpos < text.size()) {
	if((npos = text.find("\n", tpos)) != -1) {
	    sub = text.substr(tpos, npos-tpos);
	} else {
	    sub = text.substr(tpos);
	    npos = text.size();
	}

	tpos += npos-tpos+1;

	for(dpos = 0; (dpos = sub.find("\r", dpos)) != -1; ) {
	    sub.erase(dpos, 1);
	}

	for(dpos = 0; (dpos = sub.find("\t", dpos)) != -1; ) {
	    sub.erase(dpos, 1);
	    sub.insert(dpos, string(rtabmargin(false, dpos)-dpos, ' '));
	}

	lst.push_back(sub);
    }
}

void find_gather_quoted(vector<quotedblock> &lst, const string &str,
const string &quote, const string &escape) {
    bool inquote = false;
    int npos = 0, qch;
    quotedblock qb;

    while((npos = str.find_first_of(quote, npos)) != -1) {
	if(npos)
	if(escape.find(str[npos-1]) == -1) {
	    inquote = !inquote;

	    if(inquote) {
		qb.begin = npos;
		qch = str[npos];
	    } else {
		if(str[npos] == qch) {
		    qb.end = npos;
		    lst.push_back(qb);
		} else {
		    inquote = true;
		}
	    }
	}
	npos++;
    }
}

int find_quoted(const string &str, const string &needle, int offs,
const string &quote, const string &escape) {
    vector<quotedblock> positions;
    vector<quotedblock>::iterator qi;
    int npos = offs;
    bool found;

    find_gather_quoted(positions, str, quote, escape);

    while((npos = str.find(needle, npos)) != -1) {
	for(found = false, qi = positions.begin(); qi != positions.end() && !found; qi++)
	    if((npos > qi->begin) && (npos < qi->end)) found = true;

	if(!found) break;
	npos++;
    }

    return !found ? npos : -1;
}

int find_quoted_first_of(const string &str, const string &needle, int offs,
const string &quote, const string &escape) {
    vector<quotedblock> positions;
    vector<quotedblock>::iterator qi;
    int npos = offs;
    bool found;

    find_gather_quoted(positions, str, quote, escape);

    while((npos = str.find_first_of(needle, npos)) != -1) {
	for(found = false, qi = positions.begin(); qi != positions.end() && !found; qi++)
	    if((npos > qi->begin) && (npos < qi->end)) found = true;

	if(!found) break;
	npos++;
    }

    return !found ? npos : -1;
}

void splitlongtext(string text, vector<string> &lst, int size, const string cont) {
    string sub;
    int npos;

    lst.clear();

    while(!text.empty()) {
	if(text.size() <= size-cont.size()) {
	    npos = text.size();
	} else if((npos = text.substr(0, size-cont.size()).find_last_of(" \t")) == -1) {
	    npos = size-cont.size();
	}

	sub = text.substr(0, npos);
	text.erase(0, npos);

	if(text.size() > cont.size()) sub += cont; else {
	    sub += text;
	    text = "";
	}

	if((npos = text.find_first_not_of(" \t")) != -1)
	    text.erase(0, npos);

	lst.push_back(sub);
    }
}

string strdateandtime(time_t stamp, const string &fmt) {
    return strdateandtime(localtime(&stamp), fmt);
}

string strdateandtime(struct tm *tms, const string &fmt) {
    char buf[512];
    time_t current_time = time(0);
    time_t when = mktime(tms);
    string afmt = fmt;

    if(afmt.empty()) {
	afmt = "%b %e %Y %H:%M";
/*
	if(current_time > when + 6L * 30L * 24L * 60L * 60L // Old.
	|| current_time < when - 60L * 60L) {               // Future.
	    afmt = "%b %e  %Y";
	} else {
	    afmt = "%b %e %H:%M";
	}
*/
    }

    strftime(buf, 512, afmt.c_str(), tms);
    return buf;
}

bool iswholeword(const string &s, int so, int eo) {
    bool rm, lm;
    const string wdelims = "[](),.; <>-+{}=|&%~*/:?@";

    lm = !so || (wdelims.find(s.substr(so-1, 1)) != -1);
    rm = (eo == s.size()-1) || (wdelims.find(s.substr(eo, 1)) != -1);

    return rm && lm;
}

int hex2int(const string &ahex) {
    int r, i;

    r = 0;

    if(ahex.size() <= 2) {
	for(i = 0; i < ahex.size(); i++) {
	    r += isdigit(ahex[i]) ? ahex[i]-48 : toupper(ahex[i])-55;
	    if(!i) r *= 16;
	}
    }

    return r;
}

bool getconf(string &st, string &buf, ifstream &f, bool passemptylines) {
    bool ret = false;
    static string sect;

    while(!f.eof() && !ret) {
	getstring(f, buf);

	if(buf.size()) {
	    switch(buf[0]) {
		case '%':
		    sect = buf.substr(1);
		    break;
		case '#':
		    if(buf[1] != '!') break;
		default:
		    ret = buf.size();
		    break;
	    }
	} else if(passemptylines) {
	    ret = 1;
	}
    }

    st = sect;
    return ret;
}

bool getstring(istream &f, string &sbuf) {
    static char buf[2048];
    bool r;

    if(r = !f.eof()) {
	sbuf = "";

	do {
	    f.clear();
	    f.getline(buf, 2048);
	    sbuf += buf;
	} while(!f.good() && !f.eof());
    }

    return r;
}

string ruscase(const string &s, const string &mode) {
    static const string lower = "ÁÂ×ÇÄÅÖÚÉÊËÌÍÎÏÐÒÓÔÕÆÈÃÞÛÝØßÙÜÀÑ";
    static const string upper = "áâ÷çäåöúéêëìíîïðòóôõæèãþûýøÿùüàñ";
    string r, tfrom, tto;
    int pos, tpos;

    if(mode == "tolower") {
	tfrom = upper;
	tto = lower;
    } else if(mode == "toupper") {
	tfrom = lower;
	tto = upper;
    } else {
	return s;
    }

    for(r = s; (pos = r.find_first_of(tfrom)) != -1; ) {
	char c = r[pos];
	tpos = tfrom.find(c);
	r[pos] = tto[tpos];
    }

    pos = 0;

    while((pos = r.find_first_not_of(tfrom, pos)) != -1) {
	if(mode == "tolower") r[pos] = tolower(r[pos]); else
	if(mode == "toupper") r[pos] = toupper(r[pos]);
	pos++;
    }

    return r;
}

string siconv(const string &atext, const string &fromcs, const string &tocs) {
#ifdef HAVE_ICONV
    iconv_t cd = iconv_open(tocs.c_str(), fromcs.c_str());

    if(((int) cd) != -1) {
	string r, text(atext);
	size_t inleft, outleft, soutleft;
	char *inbuf, *outbuf, *sinbuf, *soutbuf;

	//from iconv.c (libiconv)
	iconv(cd,NULL,NULL,NULL,NULL);

	while(!text.empty()) {
	    sinbuf = inbuf = strdup(text.c_str());
	    inleft = strlen(inbuf);

	    soutleft = outleft = inleft*4;
	    soutbuf = outbuf = new char[outleft];

	    size_t res = iconv(cd, (ICONV_CONST char **) &inbuf, &inleft,
		&outbuf, &outleft);

	    soutbuf[soutleft-outleft] = 0;
	    r += soutbuf;
	    text.erase(0, text.size()-inleft);

	    delete soutbuf;
	    delete sinbuf;

	    if(res == -1 && errno != EILSEQ)
		break;

	    if(!text.empty()) {
		text.erase(0, 1);
		r += " ";
	    }
	}

	iconv_close(cd);
	return r;
    }
#endif

    return atext;
}

string cuthtml(const string &html, int flags) {
    string r, tag, buf, token;
    int npos, pos, tpos;

    for(pos = 0; (npos = html.find("<", pos)) != -1; pos = npos) {
	tpos = npos;
	r += html.substr(pos, npos-pos);

	if((npos = html.find(">", ++npos)) != -1) {
	    npos++;

	    tag = html.substr(tpos+1, npos-tpos-2);
	    if(tag.substr(0, 1) == "/") tag.erase(0, 1);
	    tag = leadcut(trailcut(tag, "/ \n\r"), "/ \n\r");

	    buf = ruscase(tag, "toupper");
	    token = getword(buf);

	    if(token == "BR") r += (flags & chCutBR) ? "\n" : "<br>";
		else if((flags & chCutBR) && token == "P") r += "\n\n";

	    if(flags & chLeaveLinks) {
		getword(tag);

		if(token == "A") {
		    if((tpos = buf.find("HREF")) != -1)
		    if((tpos = buf.substr(tpos).find("\"")) != -1) {
			tag.erase(0, tpos+1);
			r += "[ href: " + getword(tag, "\"") + " ] ";
		    }

		} else if(token == "IMG") {
		    if((tpos = buf.find("SRC")) != -1)
		    if((tpos = buf.substr(tpos).find("\"")) != -1) {
			tag.erase(0, tpos+1);
			r += " [ img: " + getword(tag, "\"") + " ]";
		    }

		}
	    }

	} else {
	    r += html.substr(tpos);
	    npos = html.size();
	}
    }

    if(pos < html.size())
	r += html.substr(pos);

    return r;
}

char *utf8_to_str(const char *pin) {
    int n = 0, i = 0, inlen;
    unsigned char *result;
    const unsigned char *in = (unsigned char *) pin;

    if(!in) return NULL;

    inlen = strlen(pin);
    result = new unsigned char[inlen + 1];

    while (n <= inlen - 1) {
	long c = (long)in[n];
	if(c < 0x80) result[i++] = (char)c; else {
	    if((c & 0xC0) == 0xC0) result[i++] = (char)(((c & 0x03) << 6) | (((unsigned char)in[++n]) & 0x3F));
	    else if ((c & 0xE0) == 0xE0) {
		if (n + 2 <= inlen) {
		    result[i] = (char)(((c & 0xF) << 4) | (((unsigned char)in[++n]) & 0x3F));
		    result[i] = (char)(((unsigned char)result[i]) | (((unsigned char)in[++n]) & 0x3F));
		    i++;
		} else n += 2;
	    } else if ((c & 0xF0) == 0xF0) n += 3;
	    else if ((c & 0xF8) == 0xF8)
		n += 4;
	    else if ((c & 0xFC) == 0xFC)
		n += 5;
	}
	n++;
    }

    result[i] = '\0';
    return (char *) result;
}

char *str_to_utf8(const char *pin) {
    int n = 0, i = 0;
    int inlen;
    char *result = NULL;
    const unsigned char *in = (unsigned char *) pin;

    if(!in)
	return NULL;

    inlen = strlen(pin);
    result = new char[inlen * 2 + 1];

    while (n < inlen) {
	long c = (long)in[n];
	if (c == 27) {
	    n += 2;
	    if (in[n] == 'x')
		n++;
	    if (in[n] == '3')
		n++;
	    n += 2;
	    continue;
	}

	if (c < 128)
	    result[i++] = (char)c;
	else {
	    result[i++] = (char)((c >> 6) | 192);
	    result[i++] = (char)((c & 63) | 128);
	}
	n++;
    }

    result[i] = '\0';
    return result;
}

string striprtf(const string &s, const string &charset) {
    string r, spec, unichar, tmp;
    char pre = 0;
    bool bprint, bspec, bunicode;
    int bparen = -1;

    bprint = true;
    bspec = bunicode = false;

    for(string::const_iterator i = s.begin(); i != s.end(); ++i) {
	if(!isalpha(*i) && !isdigit(*i)) bprint = true;

	if(bspec) {
	    spec += *i;

	    if(spec.size() == 2) {
		r += (char) hex2int(spec);
		bspec = false;
		bprint = true;
	    }

	} else switch(*i) {
	    case '{':
		if(pre != '\\') {
		    bparen++;
		    bprint = false;
		} else {
		    bprint = true;
		    r += *i;
		}
		break;

	    case '}':
		if(pre != '\\') {
		    bprint = false;
		    bparen--;
		} else {
		    bprint = true;
		    r += *i;
		}

		break;

	    case '\\':
		if(pre != '\\') {
		    bprint = false;
		} else {
		    bprint = true;
		    r += *i;
		    pre = 0;
		    continue;
		}
		break;

	    case '\'':
		if(!bparen && bprint && pre == '\\') {
		    spec = "";
		    bspec = true;
		} else {
		    r += *i;
		}
		break;

	    case 'u':
		if(!bparen) {
		    if(pre == '\\' && isdigit(*(i+1))) {
			unichar = "";
			bunicode = true;
		    } else if(bprint) {
			r += *i;
		    }
		}
		break;
	    default:
		if(!bparen) {
		    if(bunicode) {
			unichar += *i;

			if(unichar.size() == 5) {
			    bunicode = false;
			    if(unichar.substr(0, 4).find_first_not_of("0123456789") == -1) {
				long l = strtol(unichar.substr(0, 4).c_str(), 0, 0);
				char ubuf[sizeof(long)+4];
			    #ifdef HAVE_ICONV
				memcpy(ubuf, "\xff\xfe", 2);
				memcpy(ubuf+2, &l, sizeof(long));
				memcpy(ubuf+sizeof(long)*2, "\x0a\x00", 2);
			    #else
				strcpy(ubuf, unichar.substr(4).c_str());
			    #endif
				r += siconv(ubuf , "utf-16", charset);
			    }
			}

		    } else if(bprint) {
			r += *i;
		    }
		}
	}

	pre = *i;
    }

    return leadcut(trailcut(r));
}
