/*
* parser_cgi.c - Parser extension for CGI programs
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

#include <string.h>

#include <strfn.h>
#include <parser_cgi.h>
#include <parser.h>

/*****************************************************************************/
/* External parser procedure */
/*****************************************************************************/

char *prs_cgi_svalue_proc(const char *value, const char *key) {
    if (!key || !key[0]) return 0;
    
    if (!strcasecmp(key, "mime")) {
	return get_mime_text(value);
    }
    else if (!strcasecmp(key, "js")) {
	return get_js_text(value);
    }
    else if (!strcasecmp(key, "nobr")) {
	return get_nobr_text(value);
    }
    else if (!strcasecmp(key, "noquot")) {
	return get_noquot_text(value);
    }
    else if (!strcasecmp(key, "nohtml")) {
	return get_nohtml_text(value);
    }
    else if (!strcasecmp(key, "nohtml_br")) {
	return get_nohtml_br_text(value);
    }
    else if (!strcasecmp(key, "html")) {
	return get_html_text(value);
    }
    
    return 0;
}


/*****************************************************************************/
/* External parser procedure registration */
/*****************************************************************************/

/*
* parser_cgi_init
*/
void parser_cgi_init() {
//parser_svalue_proc *proc
    parser_svalue_reg(&prs_cgi_svalue_proc);
}

/*
* parser_cgi_done
*/
void parser_cgi_done() {
    parser_svalue_unreg(&prs_cgi_svalue_proc);
}
