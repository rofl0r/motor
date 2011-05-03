/*
* parser_cgi.h - Parser extension for CGI programs
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
* Created:	06.03.2000	romaroma	Initial revision.
* Changed:	20.06.2000	romaroma	New svalue tags
*
*/

/*
Process this keys in "svalue" tag:
    mime - encode string to MIME format
    js - encode string for JavaScript usage
    nobr - replace all spaces to "&nobr;"
    noquot - replace all quotes to spaces
    nohtml - replace all special HTML characters to it's HTML equivalents
*/

#ifndef _PARSER_CGI_H
#define _PARSER_CGI_H

#ifdef __cplusplus
extern "C" {
#endif 

/*****************************************************************************/
/* External parser procedure registration */
/*****************************************************************************/

/*
* parser_cgi_init - Initialize module
* parser_cgi_done - Deinitialize module
* 
* Parameters:
*	none
*
* Return value:
* 	none
*/
void parser_cgi_init();
void parser_cgi_done();

#ifdef __cplusplus
}
#endif 

#endif /* _PARSER_CGI_H */
