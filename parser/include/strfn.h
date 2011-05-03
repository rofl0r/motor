/*
* strfn.h - String functions
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
* Created:	22.06.1999	romaroma	Initial revision.
* Changed:	05.03.2000	romaroma	Documentation added.
* Changed:	30.03.2000	romaroma	Changed to version 1.2.0.
* Changed:	20.06.2000	romaroma	Some new functions added.
* 						strchrs function was removed
* Changed:	07.07.2000	romaroma	Some new functions was added.
* Changed:	10.07.2000	romaroma	Some new functions was added.
* Changed:	08.09.2000	romaroma	Some new functions was added.
*
*/

#ifndef _STRFN_H
#define _STRFN_H

/*****************************************************************************/
/* Functions for manipulate strings */
/*****************************************************************************/

/*
* strndup - Duplicate string with maximum length specified.
*
* WARNING: This function can process memory allocation.
* 
* Parameters:
*	char *src - source string
*	int size - new string maximum size
*
* Return value:
* 	char * - new string
*/
char *strndup(const char *src, int size);

/*
* strappend - Append source string to destination.
*
* WARNING: This function can process memory allocation.
* 
* Parameters:
*	char *dst - destination string
*	char *src - source string
*
* Return value:
* 	char * - new destination location
*/
char *strappend(char *dst, const char *src);

/*
* strnappend - Append source string to destination with maximum length
*	specified.
*
* WARNING: This function can process memory allocation.
* 
* Parameters:
*	char *dst - destination string
*	char *src - source string
*	int size - maximum append string size
*
* Return value:
* 	char * - new destination location
*/
char *strnappend(char *dst, const char *src, int size);

/*
* strdel - Delete part of string
* 
* Parameters:
*	char *src - source string
*	int start - start position to delete
*	int len - length to delete
*
* Return value:
* 	char * - new string location
*/
char *strdel(char *src, int start, int len);

/*
* strrmstr - Remove substring from string
* 
* Parameters:
*	char *src - source string
*	char *sub - substring to remove
*
* Return value:
* 	char * - new string location
*/
char *strrmstr(char *src, char *sub);

/*
* strreplacechars - Replace charecters in string
* 
* Parameters:
*	char *src - source string
*	char chr - character that will be replaced
* 	char nchr - new character
*
* Return value:
* 	char * - string location
*/
char *strreplacechars(char *src, char chr, char nchr);

/*
* strend - returns pointer to the end of string
*
* Parameters:
*	char *src - source string
*
* Return value:
*	char * - pointer to the end of source string
*/
char *strend(const char* src);

/*
* strcasestr - search substring in string, ignore case
*
* Parameters:
*	char *haystack - source string
*	char *needle - substring
*
* Return value:
*	char * - pointer to the found substring or zero if not found
*/
char *strcasestr(const char *haystack, const char *needle);

/*
* loadFromFile - load text from file to string
*
* WARNING: This function can process memory allocation.
* 
* Parameters:
*	char *fname - file name
*
* Return value:
* 	char * - text location
*/
char *loadFromFile(const char *fname);

/*
* strtriml - Trim left.
*	This function don't change source string.
* 
* Parameters:
*	char *st - string
*
* Return value:
* 	char * - new string location
*/
char *strtriml(char *st);

/*
* strtrimr - Trim right.
*
* WARNING: This function can change source string.
* 
* Parameters:
*	char *st - string
*
* Return value:
* 	char * - new string location
*/
char *strtrimr(char *st);

/*
* strtrimr - Trim left and right.
*
* WARNING: This function can change source string.
* 
* Parameters:
*	char *st - string
*
* Return value:
* 	char * - new string location
*/
char *strtrim(char *st);

/*
* itoa - Convert integer to string.
* 
* Parameters:
*	int i - integer to convert
*
* Return value:
* 	char * - string location
*/
char *itoa(int i);

/*
* isdecnum - Check for string contains decimal number.
* 
* Parameters:
*	char *str - string to check
*
* Return value:
*	int - non zero value if string contains decimal number
*/
int strisdecnumber(const char *str);

/*
* ishexnum - Check for string contains hexadecimal number.
* 
* Parameters:
*	char *str - string to check
*
* Return value:
*	int - non zero value if string contains hexadecimal number
*/
int strishexnumber(const char *str);

/*
* strsepword - Separate first word and return pointer to second word in string
* 
* Parameters:
*	char *str - string to separate
*
* Return value:
*	char * - pointer to second word in string
*/
char *strsepword(char *str);

/*****************************************************************************/
/* MIME/HTML string conversion function */
/*****************************************************************************/

/*
* get_real_text - MIME decode source string.
* get_mime_text - MIME encode source string.
* get_js_text - Decode source string for JavaScript use.
* get_nobr_text - Change all spaces in source string to "&nobr;" string.
* get_noquot_text - Change all "\"" and "'" in source string to spaces
* get_nohtml_text - Change all special html characters to its html alternatives.
* 	Skip all '\n' and '\r' characters.
* get_nohtml_br_text - Same to get_nohtml, but don't removed '\n' characters
* get_html_text - Same to get_nohtml, but don't replaced '\'' char and replaced
* 	'\n' char to "<br>"
*
* WARNING: This functions can process memory allocation.
*
* Parameters:
*	char *src - string to convert
*
* Return value:
*	char * - converted string location
*/
char *get_real_text(const char *src);
char *get_mime_text(const char *src);
char *get_js_text(const char *src);
char *get_nobr_text(const char *src);
char *get_noquot_text(const char *src);
char *get_nohtml_text(const char *src);
char *get_nohtml_br_text(const char *src);
char *get_html_text(const char *src);

#endif
