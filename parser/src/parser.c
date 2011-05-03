/*
* parser.c - Parser functions
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

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <strfn.h>
#include <llist.h>
#include <parlist.h>
#include <parser.h>

char *parser_file_base = NULL;

#define MaxInt 0x7fffffff;

const char PARSE_BEG = '[';
const char PARSE_END = ']';

#define PARSE_EVAL_BEG '{'
#define PARSE_EVAL_END '}'
#define PARSE_EQUAL '='
#define PARSE_NEQUAL '!'
#define PARSE_GREAT '>'
#define PARSE_LESS '<'
#define PARSE_OR '|'
#define PARSE_AND '&'
#define PARSE_CTRL '^'

#define PARSE_SUM '+'
#define PARSE_SUB '-'
#define PARSE_MUL '*'
#define PARSE_DIV '/'
#define PARSE_RES '%'
#define PARSE_EVAL_CHARS "+-*/%"

#define PARSE_PAR_SEP '.'
#define PARSE_ARR_BEG '('
#define PARSE_ARR_END ')'

#define PARSE_L "l"
#define PARSE_R "r"

#define PARSE_MOVE "move"
#define PARSE_SET "set"
#define PARSE_UNSET "unset"
#define PARSE_EVAL "eval"

#define PARSE_DEFLIST "deflist"
#define PARSE_DEFARR "defarr"

#define PARSE_LOOP "loop"
#define PARSE_LOOPEND "loopend"
#define PARSE_LOOPNEXT "loopnext"
#define PARSE_LOOPPREV "loopprev"
#define PARSE_LOOPINC "loopinc"
#define PARSE_LOOPDEC "loopdec"
#define PARSE_LOOPEXIT "loopexit"
#define PARSE_LOOP_SEP ';'

#define PARSE_PARAM_SEP ','

#define VPARSE_LOOPVALUE "loopvalue"
#define VPARSE_PRE "pre"
#define VPARSE_OUTPUT "output"

#define V_ON "on"
#define V_OFF "off"

#define PARSE_INCLUDE "include"
#define PARSE_INCLUDE_TEMPLATE "template"
#define PARSE_INCLUDE_PLAIN "plain"

#define PARSE_EXEC "exec"

#define PARSE_OUTPUT "output"
#define PARSE_PRE "pre"

#define PARSE_BR "br"

#define PARSE_VALUE "value"
#define PARSE_SVALUE "svalue"

#define PARSE_IF "if"
#define PARSE_ELSE "else"
#define PARSE_ELSIF "elsif"
#define PARSE_ENDIF "endif"


/*****************************************************************************/
/* Extensions */
/*****************************************************************************/

void (*parser_extension_proc)(const char *tag, const char *tagparams,
    pparamslist *params, void out_proc(const char *txt, int len)) = NULL;

static void (*parser_out_proc)(const char *txt, int len) = NULL;


/*
* Special value ("svalue") process
*/

// List of procedures
llist_item *prs_svalue_proc_list = 0;

/*
* parser_svalue_reg
*/
void parser_svalue_reg(parser_svalue_proc *proc) {
    if (llist_get_index(prs_svalue_proc_list, proc) < 0)
	prs_svalue_proc_list = llist_add(prs_svalue_proc_list, proc);
}

/*
* parser_svalue_unreg
*/
void parser_svalue_unreg(parser_svalue_proc *proc) {
    prs_svalue_proc_list = llist_delete_item(prs_svalue_proc_list, proc);
}

/*
* Call all svalue procedures in list
*/
char *prs_call_svalue(const char *value, const char *params) {
    llist_item *l = llist_first(prs_svalue_proc_list);
    char *res;
    
    while (l) {
	res = ((parser_svalue_proc *) l->item)(value, params);
	if (res) return res;
	l = llist_next(l);
    }
    
    return 0;
}


/*
* Execute ("exec") process
*/

// List of procedures
pparamslist *prs_exec_proc_list = 0;

/*
* parser_exec_reg
*/
void parser_exec_reg(char *procname, parser_exec_proc *proc) {
    if (!prs_exec_proc_list) prs_exec_proc_list = pparamslist_create();
    pparamslist_set_ptr(procname, proc, 0, prs_exec_proc_list);
}

/*
* parser_exec_unreg
*/
void parser_exec_unreg(char *procname) {
    if (!prs_exec_proc_list) return;
    pparamslist_unset(procname, prs_exec_proc_list);
    if (!(prs_exec_proc_list->count)) pparamslist_free(prs_exec_proc_list);
}

/*
* Call all svalue procedures in list
*/
void prs_call_exec(const char *name, char *params) {
    pparam *p;
    char **argv = 0;
    char *s = params, *s1;
    int cnt = 0;
    
    if (!prs_exec_proc_list) return;
    if (!(p = pparamslist_get(name, prs_exec_proc_list))) return;
    
    while (s && s[0]) {
	if ((s1 = strchr(s, PARSE_PARAM_SEP))) {
	    *s1 = '\0';
	    s1++;
	}
	cnt++;
	argv = realloc(argv, cnt * sizeof(char *));
	argv[cnt - 1] = strtrim(s);

	s = s1;
    }

    ((parser_exec_proc *) (void *)p->value)(cnt, (const char**)argv);
    
    free(argv);
}

/*****************************************************************************/
/* Interface functions */
/*****************************************************************************/

void text_parse_(const char *srcText, int textSize, pparamslist *params, int loadMode);
void init_parser_data();

static char *sstrparse_res;
static FILE *fstrparse_f;

void sstrparse_out_text(const char *txt, int len) {
    sstrparse_res = strnappend(sstrparse_res, txt, len);
}

void fstrparse_out_text(const char *txt, int len) {
    fwrite(txt, 1, len, fstrparse_f);
    //fflush(fstrparse_f);
}
    
char *strparse(const char *fileNameOrSrc, int sizeNameOrSrc, pparamslist *params, FILE *outFile, int srcMode, int loadMode) {
    int tsize;
    char *src, *res = 0;

    if (fileNameOrSrc == NULL) return NULL;

    switch (srcMode) {
	case PARSER_SRC_STR:
	    src = (char *)fileNameOrSrc;
	    if (sizeNameOrSrc == -1) tsize = strlen(fileNameOrSrc);
	    else tsize = sizeNameOrSrc;
	    break;
	case PARSER_SRC_FILE:
	    src = loadTextFromFile(fileNameOrSrc, loadMode, &tsize);
	    break;
	default:
	    return 0;
    }
    if (src == NULL) return NULL;

    init_parser_data();

    if (outFile != NULL) {
	// parse to file
	fstrparse_f = outFile;
	parser_out_proc = fstrparse_out_text;
	text_parse_(src, tsize, params, loadMode);
    }
    else {
	// parse to string
	sstrparse_res = calloc(1, 1);
	parser_out_proc = sstrparse_out_text;
	text_parse_(src, tsize, params, loadMode);
	res = sstrparse_res;
    }

    switch (srcMode) {
	case PARSER_SRC_STR:
	    break;
	case PARSER_SRC_FILE:
	    unloadTextFromFile(src, loadMode, tsize);
	    break;
    }

    return res;
}

char *sstrparse(const char *plainText, pparamslist *params) {
    return strparse(plainText, -1, params, NULL, PARSER_SRC_STR, PARSER_FLOAD_MEM);
}

void fstrparse(const char *plainText, pparamslist *params, FILE *f) {
    strparse(plainText, -1, params, f, PARSER_SRC_STR, PARSER_FLOAD_MEM);
}

/* Real functions */

char *loadTextFromFile(const char *filename, int loadMode, int *size) {
    int f;
    char *buf = NULL;
    struct stat stbuf;

    if ((f = open(filename, O_RDONLY)) < 0) return NULL;
    if (!fstat(f, &stbuf)) {
	*size = stbuf.st_size;
	
	switch (loadMode) {
	    case PARSER_FLOAD_MEM:
		// load into memory
		buf = malloc(*size);
		if (read(f, buf, *size) != *size) {
		    free(buf);
		    buf = NULL;
		}
		break;
	    case PARSER_FLOAD_MMAP:
		// load using mmap
		buf = mmap(0, *size, PROT_READ, MAP_SHARED, f, 0);
		if (buf == MAP_FAILED) buf = NULL;
		break;
	}
    }
    close(f);

    return buf;
}

void unloadTextFromFile(char *text, int loadMode, int size) {
    if (text == NULL) return;

    switch (loadMode) {
        case PARSER_FLOAD_MEM:
    	    // unload from memory
	    free(text);
	    break;
	case PARSER_FLOAD_MMAP:
	    // munmap
	    munmap(text, size);
	    break;
    }
}

typedef struct loop_struct_type {
    char *bpos;
    int start, end, step, curr;
} loop_struct;

int compare(const char *s1, const char *s2) {
    char buf[32] = {'\0'}, *s = buf;
    int i1, i2;
    
    i1 = strtol(s1, &s, 0);
    if (s1[0] && !s[0]) {
	i2 = strtol(s2, &s, 0);
	if (s2[0] && !s[0]) {
	    if (i1 < i2) return -1;
	    if (i1 > i2) return 1;
	    return 0;
	}
    }

    return strcasecmp(s1, s2);
}

char *new_line = "\n";

#define cloop loops[loopcnt - 1]
#define loop_step loops[loopcnt - 1].curr += loops[loopcnt - 1].step
#define loop_unstep loops[loopcnt - 1].curr -= loops[loopcnt - 1].step

#define DISABLE_IF 0
#define DISABLE_LOOP 1

static char out_off = 0, out_pre = 1, disable = 0, disablet = 0;

static loop_struct *loops = NULL;
static int loopcnt = 0;

static pparamslist *parser_params = NULL;
    
/* !!! Parser internal functions !!! */

void freestr(char **str) {
    if (*str != 0) {
	free(*str);
	*str = 0;
    }
}

/* Out text */
void out_text(const char *txt, int size) {
    int i, sz = size;
    /* char tabbuf[8] = {' ',' ',' ',' ',' ',' ',' ',' '}; */

    if (parser_out_proc == NULL) return;
    if (disable || out_off) return;

    if (size < 0) sz = strlen(txt);
    if (out_pre) parser_out_proc(txt, sz);
    else
        for (i = 0; i < sz; i++) {
	    switch (txt[i]) {
		case '\r': case '\n':
		    break;
		/* case '\t':
		    parser_out_proc(tabbuf, sizeof(tabbuf));
		    break; */
		default:
		    parser_out_proc(&txt[i], 1);
	    }
	}
}
    
void out_text_(const char *txt, int size) {
    int sz = size;

    if (parser_out_proc == NULL) return;
    if (disable || out_off) return;

    if (size < 0) sz = strlen(txt);
    parser_out_proc(txt, sz);
}

/* Loops */
    
void loop_add(const char *bpos, int start, int end, int step) {
    int ind = loopcnt++;
    loops = realloc(loops, loopcnt * sizeof(loop_struct));
    loops[ind].bpos = (char *) bpos;
    loops[ind].start = start;
    loops[ind].end = end;
    loops[ind].step = step;
    loops[ind].curr = start;
}

void loop_del() {
    if (loopcnt <= 0) return;
    loopcnt--;
    loops = realloc(loops, loopcnt * sizeof(loop_struct));
    if (!loopcnt) loops = NULL;
}
    
int loop_check() {
    loop_struct *l = &(loops[loopcnt - 1]);
    if (l->step >= 0) return l->curr < l->end;
    else return l->curr > l->end;
}
    
/* Get value */

char *get_value(char *name) {
    char *z = 0;
    int i;

    if (name == NULL) return NULL;
    if (name[0] == PARSE_CTRL) {
        z = (char *) name + 1;
        if (!strncasecmp(z, VPARSE_LOOPVALUE, sizeof(VPARSE_LOOPVALUE) - 1)) {
    	    if (loopcnt) {
		z += sizeof(VPARSE_LOOPVALUE) - 1;
		i = loopcnt - 1;
		while ((z[0] == PARSE_CTRL) && i) i--;
	    	z = strdup(itoa(loops[i].curr));
	    }
	    else z = strdup(itoa(0));
        }
        else if (!strncasecmp(z, VPARSE_PRE, sizeof(VPARSE_PRE) - 1)) {
	    z = strdup(itoa(out_pre));
        }
        else if (!strncasecmp(z, VPARSE_OUTPUT, sizeof(VPARSE_OUTPUT) - 1)) {
	    if (out_off) z = strdup(itoa(0));
	    else z = strdup(itoa(1));
        }
        else z = NULL;
    }
    else {
	char *s, *s1, *ptype;
	
	ptype = strtrim(strsepword(name));
	if ((z = pparamslist_value(name, parser_params))) {
	    z = strdup(z);
	    while (ptype && ptype[0]) {
	        if ((s = strchr(ptype, PARSE_PARAM_SEP))) {
		    s[0] = '\0';
		    s++;
		}
		if ((s1 = prs_call_svalue(z, strtrim(ptype)))) {
		    free(z);
		    z = s1;
		}

		ptype = s;
	    }
	}
    }

    return z;
}

/* Evaluate value */

char *evaluate(const char *value) {
    const char *src = value;
    char *val, *sb, *se, *z, *buf = calloc(1, 1);

    while (src[0]) {
	if ((sb = strchr(src, PARSE_EVAL_BEG)) == NULL) {
	    buf = strappend(buf, src);
	    break;
	}
	if ((se = strchr(sb, PARSE_EVAL_END)) == NULL) se = sb + strlen(sb);
	
	buf = strnappend(buf, src, sb - src);
	sb++;
	z = strndup(sb, se - sb);
	buf = strappend(buf, val = get_value(z));
	freestr(&val);
	free(z);

	if (se[0]) se++;
	src = se;
    }

    return buf;
}

char *evaluate_(const char *value, char **evbuf) {
    if ((*evbuf) != NULL) free((*evbuf));
    (*evbuf) = evaluate(value);
    return (*evbuf);
}
	
/* If condition */

int ifcond(char *str) {
    char *s, *sb, *se, c;
    int res = 0, i;

    if ((s = strchr(str, PARSE_EQUAL)) != NULL) c = PARSE_EQUAL;
    else if ((s = strchr(str, PARSE_NEQUAL)) != NULL) c = PARSE_NEQUAL;
    else if ((s = strchr(str, PARSE_GREAT)) != NULL) c = PARSE_GREAT;
    else if ((s = strchr(str, PARSE_LESS)) != NULL) c = PARSE_LESS;
    else c = PARSE_EQUAL;

    if (s != NULL) { s[0] = '\0'; s++; s = strtrim(s); }

    sb = evaluate(strtrim(str));
    if ((s != NULL) && s[0]) {
        se = evaluate(s);
	i = compare(strtrim(sb), strtrim(se));
	free(se);
	switch (c) {
	    case PARSE_EQUAL: if (!i) res = 1; break;
	    case PARSE_NEQUAL: if (i) res = 1; break;
	    case PARSE_GREAT: if (i > 0) res = 1; break;
	    case PARSE_LESS: if (i < 0) res = 1; break;
	}
    }
    else {
	i = pparamslist_exist(strtrim(sb), parser_params);
	if (c == PARSE_EQUAL) { if (i) res = 1; }
	else { if (!i) res = 1; }
    }
    free(sb);

    return res;
}
    
int ifexpr(char *str) {
    char nc = 0;
    char *s = str, *se;
    int res = -1, i, c = '\0';

    while (s != NULL) {
        if ((se = strchr(s, PARSE_OR)) != NULL) nc = PARSE_OR;
        else if ((se = strchr(s, PARSE_AND)) != NULL) nc = PARSE_AND;
        else nc = 0;
        if (se != NULL) { se[0] = '\0'; se++; }

        i = ifcond(s);
        if (res < 0) res = i;
        else switch (c) {
    	    case PARSE_OR: res = res || i; break;
	    case PARSE_AND: res = res && i; break;
    	}
	if (se == NULL) break;
	s = se;
	c = nc;
    }

    return res;
}
    
/* Evaluate expression */

int evalexpr(char *str) {
    char nc = 0, c = 0;
    char *s = str, *se, *val;
    int res = 0, i;

    if (!str) return 0;
    
    while (s) {
	se = strpbrk(s, PARSE_EVAL_CHARS);
        if (se) {
	    nc = se[0];
	    se[0] = '\0';
	    se++;
	}
        else nc = 0;

	val = evaluate(strtrim(s));
	if (val) {
	    i = atoi(val);
	    free(val);
	}
	else i = 0;
        switch (c) {
	    case 0: res = i; break;
    	    case PARSE_SUM: res += i; break;
    	    case PARSE_SUB: res -= i; break;
    	    case PARSE_MUL: res *= i; break;
    	    case PARSE_DIV: if (i) res /= i; else res = MaxInt; break;
    	    case PARSE_RES: if (i) res %= i; else res = i; break;
    	}

	if (!se || !nc) break;
	s = se;
	c = nc;
    }

    return res;
}


/* Other */

void try_another_tag(const char *kwd, const char *par, pparamslist *params) {
    if (parser_extension_proc != NULL)
	parser_extension_proc(kwd, par, params, parser_out_proc);
}

void init_parser_data() {
    out_off = 0;
    out_pre = 1;
    disable = 0;
    disablet = 0;
    loops = NULL;
    loopcnt = 0;
}

/* !!! Parser internal functions end !!! */

#define __srcsize (srcText + textSize - src)
void text_parse_(const char *srcText, int textSize, pparamslist *params, int loadMode) {
    const char *src = srcText;
    char *s, *sb, *se, *mc, *kw, *pr;
    char *evbuf = NULL;
    
    parser_params = params;
    
    if (src == NULL) return;

    while (__srcsize > 0) {
	/* Looking for search function
	if ((sb = strstr(src, PARSE_BEG)) == NULL) {
	*/
	if ((sb = memchr(src, PARSE_BEG, __srcsize)) == NULL) {
	    out_text(src, __srcsize);
	    break;
	}
	
	if (sb != src) out_text(src, sb - src);
	
	/* Looking for search function
	sb += (sizeof(PARSE_BEG) - 1);
	if ((se = strstr(sb, PARSE_END)) == NULL) break;
	src = se + sizeof(PARSE_END) - 1;
	*/
	sb++;
	if ((se = memchr(sb, PARSE_END, srcText + textSize - sb)) == NULL) break;
	src = se + 1;

	mc = calloc(1, se - sb + 1);
	strncpy(mc, sb, se - sb);
	
	/* Find comments */
	sb = mc;
	while ((se = strstr(sb, "//"))) {
	    if (strncmp(se, "///", 3)) {
		se[0] = '\0';
		break;
	    }
	    
	    memmove(se + 2, se + 3, strlen(se + 2));
	    sb = se + 2;
	}
	
	/* Separate command and parameters */
	for (sb = strtriml(mc); sb[0] && !isspace(sb[0]); sb++);
	if (sb[0]) { sb[0] = '\0'; sb++; }

	kw = strdup(strtrim(mc));
	pr = strdup(strtrim(sb));

	free(mc);
	
	/* Parse control */
	if (!strcasecmp(kw, PARSE_IF)) {
	    /* If block */
	    if (disable) {
		if (disablet == DISABLE_IF) disable++;
	    }
	    else if (! ifexpr(pr)) {
		disable++;
		disablet = DISABLE_IF;
	    }
	}
	else if (!strcasecmp(kw, PARSE_ELSE)) {
	    /* Else block */
	    if (disable) {
		if ((disablet == DISABLE_IF) && (disable == 1)) disable--;
	    }
	    else {
		disable++;
		disablet = DISABLE_IF;
	    }
	}
	else if (!strcasecmp(kw, PARSE_ENDIF)) {
	    /* End if block */
	    if (disable && (disablet == DISABLE_IF)) disable--;
	}
	/* Loops */
    	else if (!strcasecmp(kw, PARSE_LOOP)) {
	    /* Begin loop block */
	    if (disable) {
		if (disablet == DISABLE_LOOP) disable++;
	    }
	    else {
		if (pr[0]) {
		    int i1, i2, i3;

		    if ((sb = strchr(pr, PARSE_LOOP_SEP)) == NULL) sb = pr + strlen(pr);
		    if (sb[0]) { sb[0] = '\0'; sb++; }
		    if ((se = strchr(sb, PARSE_LOOP_SEP)) == NULL) se = sb + strlen(sb);
		    if (se[0]) { se[0] = '\0'; se++; }
		
		    i1 = atoi(evaluate_(strtrim(pr), &evbuf));
		    i2 = atoi(evaluate_(strtrim(sb), &evbuf));
		    i3 = atoi(evaluate_(strtrim(se), &evbuf));

		    loop_add(src, i1, i2, i3);
		}
		else
		    loop_add(src, 0, 1, 0);
		
		if (!loop_check()) {
		    loop_del();
		    disable++;
		    disablet = DISABLE_LOOP;
		}
	    }
	}
    	else if (!strcasecmp(kw, PARSE_LOOPEND)) {
	    /* End loop block */
	    if (disable) {
		if (disablet == DISABLE_LOOP) disable--;
	    }
	    else {
		if (pr[0]) {
		    if (ifexpr(pr)) loop_del(&loops, &loopcnt);
		    else src = cloop.bpos;
		}
		else {
		    loop_step;
		    if (loop_check(&loops, &loopcnt)) {
			src = cloop.bpos;
		    }
		    else {
			loop_del(&loops, &loopcnt);
		    }
		}
	    }
	}
	/* Other controls */
	else if (!disable) {
	    if (!strcasecmp(kw, PARSE_L)) {
		/* Out left bracket */
		out_text((char *) &PARSE_BEG, 1);
	    }
	    else if (!strcasecmp(kw, PARSE_R)) {
		/* Out right bracket */
		out_text((char *) &PARSE_END, 1);
	    }
	    else if (!strcasecmp(kw, PARSE_EXEC)) {
		/* Call external procedure */
		se = strsepword(pr);
		sb = evaluate(strtrim(pr));
		prs_call_exec(sb, evaluate_(strtrim(se), &evbuf));
		free(sb);
	    }
	    else if (!strcasecmp(kw, PARSE_INCLUDE)) {
		/* Include file */
		int incsize = 0;

		se = strsepword(pr);

		sb = calloc(1, 1);
		if (parser_file_base) {
		    sb = strappend(sb, parser_file_base);
		    sb = strappend(sb, "/");
		}
		sb = strappend(sb, evaluate_(pr, &evbuf));
		strrmstr(sb, "../");
		s = loadTextFromFile(sb, loadMode, &incsize);
		free(sb);

		if (s != NULL) {
		    if (!strcasecmp(se, PARSE_INCLUDE_PLAIN))
			out_text(s, incsize);
		    if (!strcasecmp(se, PARSE_INCLUDE_TEMPLATE))
			text_parse_(s, incsize, params, loadMode);
		    else if (!se || !se[0])
			text_parse_(s, incsize, params, loadMode);
		    unloadTextFromFile(s, loadMode, incsize);
		}
	    }
	    /* Output control */
	    else if (!strcasecmp(kw, PARSE_OUTPUT)) {
		/* Output on/off */
		if (!strcasecmp(pr, V_ON)) out_off = 0;
		else if (!strcasecmp(pr, V_OFF)) out_off = 1;
	    }
    	    else if (!strcasecmp(kw, PARSE_PRE)) {
	        /* Pre text control */
	        if (!strcasecmp(pr, V_ON)) out_pre = 1;
	        else if (!strcasecmp(pr, V_OFF)) out_pre = 0;
	    }
	    /* Loops */
	    else if (!strcasecmp(kw, PARSE_LOOPEXIT)) {
		/* Exit from loop block */
		if (pr[0]) {
		    if (ifexpr(pr)) { disable++; disablet = DISABLE_LOOP; }
		}
		else { disable++; disablet = DISABLE_LOOP; }
	    }
    	    else if (!strcasecmp(kw, PARSE_LOOPNEXT)) {
	        /* Loop next */
		loop_step;
	    }
    	    else if (!strcasecmp(kw, PARSE_LOOPPREV)) {
	        /* Loop prevous */
		loop_unstep;
	    }
    	    else if (!strcasecmp(kw, PARSE_LOOPINC)) {
	        /* Loop increment */
		cloop.curr++;
	    }
    	    else if (!strcasecmp(kw, PARSE_LOOPDEC)) {
	        /* Loop decrement */
		cloop.curr--;
	    }
	    /* Text control */
	    else if (!strcasecmp(kw, PARSE_BR)) {
		/* New line */
	        out_text_(new_line, -1);
	    }
	    /* Values control */
	    else if (!strcasecmp(kw, PARSE_VALUE) || !strcasecmp(kw, PARSE_SVALUE)) {
		/* Out parameter [special] value */
		if ((s = get_value(evaluate_(pr, &evbuf))) != NULL) {
		    out_text(s, -1);
		    freestr(&s);
		}
	    }
	    else if (!strcasecmp(kw, PARSE_MOVE)) {
		/* Set parameter value to value of another parameter */
		if ((s = strchr(pr, PARSE_EQUAL)) != NULL) { s[0] = '\0'; s++; }
		sb = evaluate(strtrim(pr));
		if (s != NULL) se = get_value(evaluate_(strtrim(s), &evbuf));
		else se = NULL;
		pparamslist_set(sb, se, params);
		free(sb);
		freestr(&se);
	    }
	    else if (!strcasecmp(kw, PARSE_SET)) {
		/* Set parameter to specified value */
		if ((s = strchr(pr, PARSE_EQUAL)) != NULL) { s[0] = '\0'; s++; }
		sb = evaluate(strtrim(pr));
		if (s != NULL) s = evaluate_(strtrim(s), &evbuf);
		pparamslist_set(sb, s, params);
		free(sb);
	    }
	    else if (!strcasecmp(kw, PARSE_UNSET)) {
		/* Delete parameter */
		pparamslist_unset(evaluate_(strtrim(pr), &evbuf), params);
	    }
	    else if (!strcasecmp(kw, PARSE_EVAL)) {
		/* Evaluate expression and copy result to parameter */
		if ((s = strchr(pr, PARSE_EQUAL)) != NULL) { s[0] = '\0'; s++; }
		sb = evaluate(strtrim(pr));
		if (s != NULL) s = strtrim(s);
		pparamslist_set(sb, itoa(evalexpr(s)), params);
		free(sb);
	    }
	    else if (!strcasecmp(kw, PARSE_DEFLIST)) {
		/* Add params list */
		/* Not realised */
	    }
	    else if (!strcasecmp(kw, PARSE_DEFARR)) {
		/* Add params array */
		/* Not realised */
	    }
	    else try_another_tag(kw, pr, params);
	}
	else try_another_tag(kw, pr, params);
	
	free(kw);
	free(pr);
    }
}
