/*
* strfn.c - String functions
*
* Copyright (C) 1999-2001 by Roman Khnykin <romaroma@rr.org.ua>
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

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <strfn.h>

/* string conversion function */
char char2byte(char c) {
    switch (c) {
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': return(c - '0');
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': return(c - 'A' + 10);
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': return(c - 'A' + 10);
    }
    return 0;
}

char byte2char(char c) {
    char z = c;
    if (c < 10) { z += '0'; }
    else if (c < 16) { z += ('A' - 10); }
    return z;
}

char *strappend(char *dst, const char *src) {
    int dlen;
    
    if (dst == NULL) return NULL;
    if (src == NULL) return dst;

    dlen = strlen(dst);

    dst = realloc(dst, dlen + strlen(src) + 1);
    strcpy(&dst[dlen], src);
    
    return dst;
}

char *strnappend(char *dst, const char *src, int size) {
    int dlen, slen;
    
    if (dst == NULL) return NULL;
    if (src == NULL) return dst;

    dlen = strlen(dst);
    slen = strlen(src);
    if (size < slen) slen = size;
    
    dst = realloc(dst, dlen + slen + 1);
    strncpy(&dst[dlen], src, slen);
    dst[dlen + slen] = '\0';
    
    return dst;
}

char *strreplacechars(char *src, char chr, char nchr) {
    char *s;
    
    if (src == NULL) return NULL;
    for (s = src; s[0]; s++) {
	if (*s == chr) *s = nchr;
    }
    
    return src;
}

char *loadFromFile(const char *fname) {
    FILE *f;
    int len;
    char *buf;

    if ((f = fopen(fname, "rb")) == NULL) return NULL;
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    fseek(f, 0, SEEK_SET);
    buf = calloc(len + 1, 1);
    fread(buf, 1, len, f);
    fclose(f);
    
    return buf;
}

char *strtriml(char *st) {
    char *res = st;
    if (st == NULL) return NULL;
    while (res[0] && isspace(res[0])) res++;
    return res;
}

char *strtrimr(char *st) {
    char *res;
    if (st == NULL) return NULL;
    if (st[0]) {
	res = st + strlen(st) - 1;
	while ((res != st) && isspace(res[0])) res--;
	res[1] = '\0';
    }
    return st;
}

char *strtrim(char *st) {
    char *res = strtriml(st);
    strtrimr(res);
    return res;
}

char *itoa(int i) {
    static char buf[16];
    sprintf(buf, "%d", i);
    return buf;
}

char *get_real_text(const char *src) {
    char *dst;
    int i, last = 0;
    
    if (src == NULL) return NULL;
    dst = strdup(src);
    for (i = 0; src[i]; i++) {
        switch (src[i]) {
            case '%': {
                dst[last] = (char2byte(src[i + 1]) << 4) + char2byte(src[i + 2]);
                i += 2;
                break;
            }
            case '+': {
                dst[last] = ' ';
                break;
            }
            default: {
                dst[last] = src[i];
                break;
            }
        }
        last++;
    }
    dst[last] = '\0';

    return dst;
}

#define get_mime_text_addEncChar { buf[1] = byte2char(((unsigned char)src[i]) >> 4); \
buf[2] = byte2char(src[i] & 0xf); \
dst = strappend(dst, (char *)&buf); }
    
char *get_mime_text(const char *src) {
    char *dst;
    char buf[4];
    //char plus[2] = {'+', '\0'};
    int i;
    
    buf[0] = '%'; buf[3] = '\0';
    if (src == NULL) return NULL;
    dst = calloc(1, 1);
    for (i = 0; src[i]; i++) {
	/*if (src[i] == ' ')
	    dst = strappend(dst, (char *) &plus);
	else*/ if (isdigit(src[i]) || (isascii(src[i]) && isalpha(src[i])))
    	    dst = strnappend(dst, &src[i], 1);
	else
	    get_mime_text_addEncChar;
    }

    return dst;
}

char *get_js_text(const char *src) {
    char *dst;
    char buf[4];
    int i;
    
    buf[0] = '%'; buf[3] = '\0';
    if (src == NULL) return NULL;
    dst = calloc(1, 1);
    for (i = 0; src[i]; i++) {
	if ((src[i] == '\'') || (src[i] == '"') || (src[i] == '%'))
	    get_mime_text_addEncChar
	else
    	    dst = strnappend(dst, &src[i], 1);
    }

    return dst;
}

char *__nbsp_str = "&nbsp;";
char *__lt_str = "&lt;";
char *__gt_str = "&gt;";
char *__amp_str = "&amp;";
char *__quot_str = "&quot;";
char *__br_str = "<br>";

char *get_nobr_text(const char *src) {
    char *dst;
    int i;
    
    if (src == NULL) return NULL;
    
    dst = calloc(1, 1);
    for (i = 0; src[i]; i++) {
	switch (src[i]) {
	    case ' ': case '\r': case '\n': case '\t':
		dst = strappend(dst, __nbsp_str);
	        break;
	    default:
		dst = strnappend(dst, &src[i], 1);
	}
    }
    
    return dst;
}


/*
* mode values:
* 	0x1 - replace '\'' to '`'
* 	0x2 - remove '\n'
* 	0x4 - convert '\n' to '<br>' (only if not removed)
*/
char *__get_html_text(const char *src, int mode) {
    char *dst;
    int i;
    
    if (src == NULL) return NULL;
    
    dst = calloc(1, 1);
    for (i = 0; src[i]; i++) {
	switch (src[i]) {
	    case '\r':
		break;
	    case '\n':
		if (!(mode & 0x2)) {
		    if (mode & 0x4) dst = strappend(dst, __br_str);
		    else dst = strnappend(dst, &src[i], 1);
		}
	        break;
	    case '<':
		dst = strappend(dst, __lt_str);
	        break;
	    case '>':
		dst = strappend(dst, __gt_str);
	        break;
	    case '&':
		dst = strappend(dst, __amp_str);
	        break;
	    case '"':
		dst = strappend(dst, __quot_str);
	        break;
	    default:
		dst = strnappend(dst, &src[i], 1);
	}
    }
    if (mode & 0x1) strreplacechars(dst, '\'', '`');
    
    return dst;
}

char *get_nohtml_text(const char *src) {
    return __get_html_text(src, 0x1 + 0x2);
}

char *get_nohtml_br_text(const char *src) {
    return __get_html_text(src, /*0x1*/0);
}

char *get_html_text(const char *src) {
    return __get_html_text(src, 0x4);
}

char *get_noquot_text(const char *src) {
    char *dst;
    
    if (src == NULL) return NULL;
    
    dst = strdup(src);
    strreplacechars(dst, '"', ' ');
    strreplacechars(dst, '\'', ' ');
    
    return dst;
}

char *strdel(char *src, int start, int len) {
    char *b, *e;
    int size = 0;

    if (src == NULL) return NULL;
    if (strlen(src) <= start) return src;
    
    b = src + start;
    size = strlen(b);
    if (size < len) e = b + size;
    else e = b + len;
    memmove(b, e, strlen(e) + 1);

    return src;
}

char *strrmstr(char *src, char *sub) {
    char *s = src;
    int sublen = 0;

    if (src == NULL) return NULL;
    if ((sub == NULL) || (!sub[0])) return src;
    
    sublen = strlen(sub);

    while ((s = strstr(s, sub)) != NULL)
	memmove(s, s + sublen, strlen(s) - sublen + 1);
	
    return src;
}

int strisdecnumber(const char *str) {
    int i;
    
    if (str == NULL) return 0;
    for (i = 0; str[i]; i++)
	if (!isdigit(str[i])) return 0;
    return 1;
}

int strishexnumber(const char *str) {
    int i;
    
    if (str == NULL) return 0;
    for (i = 0; str[i]; i++)
	if (!isxdigit(str[i])) return 0;
    return 1;
}

char *strsepword(char *str) {
    char *s;
    
    if (!str) return 0;

    for (s = str; s[0] && !isspace(s[0]); s++);
    if (s[0]) {
	s[0] = '\0';
	s++;
    }
    return s;
}

char *strend(const char* src) {
    const char *res = 0;
    if (src) for (res = src; *res; res++);
    return (char *) res;
}

char *strcasestr(const char *haystack, const char *needle) {
    const char *s = haystack;
    int size = strlen(needle);
    
    while (s && s[0]) {
	if (!strncasecmp(s, needle, size)) return (char *) s;
	s++;
    }
    
    return 0;
}
