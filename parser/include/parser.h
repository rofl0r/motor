/*
* parser.h - Parser functions
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
* Changed:	06.03.2000	romaroma	Changed mechanism of parse
*						svalue tag.
* Changed:	30.03.2000	romaroma	Changed to version 1.2.0
* Changed:	19.07.2000	romaroma	Changed to version 1.3.1
* Changed:	02.08.2000	romaroma	Changed to version 1.3.2
* Changed:	09.08.2000	romaroma	Changed to version 1.3.3
* Changed:	17.08.2000	romaroma	Changed to version 1.4.0
* Changed:	23.08.2000	romaroma	Changed to version 1.4.1
* Changed:	04.09.2000	romaroma	Changed to version 1.4.2
*
*/

#ifndef _PARSER_H
#define _PARSER_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>
#include "parlist.h"

/*****************************************************************************/
/* External parser data */
/*****************************************************************************/

/*
* parser_file_base - string which will add to any included file name
*/
extern char *parser_file_base;

/*
* parser_extension_proc - Procedure for process external tags
* 
* Parameters:
*	char *tag - language command
*	char *params - command parameters
*	pparamslist *params - current parameters list
*	void out_proc - pointer to procedure for output text
*
* Return value:
* 	none
*/
extern void (*parser_extension_proc)(const char *tag, const char *tagparams,
    pparamslist *params, void out_proc(const char *txt, int len));

/***** value/svalue tag *****/
/*
* parser_svalue_proc - Type of procedure for process special value ("svalue")
*	tag.
* WARNING: This function must allocate new string for result returning.
* 
* Parameters:
*	char *value - value fetched from svalue command
*	char *key - key fetched from svalue command
*
* Return value:
*	char * - new string (or NULL if no match key found).
*/
typedef char * (parser_svalue_proc) (const char *value, const char *key);

/*
* parser_svalue_reg, parser_svalue_unreg - Add/remove procedure for process
*	special value ("svalue") tag.
* WARNING: Registered function must allocate new string for result returning.
* 
* Parameters:
*	parser_svalue_proc *proc - procedure address
*
* Return value:
* 	none
*/
void parser_svalue_reg(parser_svalue_proc *proc);
void parser_svalue_unreg(parser_svalue_proc *proc);

/***** exec tag *****/
/*
* parser_exec_proc - Type of procedure that will be
* 	executed by exec tag
* 
* Parameters:
*	int argc - count of parameters in exec tag
*	char *argv[] - parameters, specified in exec tag
*
* Return value:
*	none
*/
typedef void (parser_exec_proc) (int argc, const char *argv[]);

/*
* parser_exec_reg, parser_exec_unreg - Add/remove procedure that will be
* 	executed by exec tag
* 
* Parameters:
*	char *procname - procedure name for exec tag
*	parser_exec_proc *proc - procedure address
*
* Return value:
* 	none
*/
void parser_exec_reg(char *procname, parser_exec_proc *proc);
void parser_exec_unreg(char *procname);

/*****************************************************************************/
/* Parser functions */
/*****************************************************************************/

/*
* fstrparse - Parse source string and put parsed data to file.
* 
* Parameters:
*	char *plainText - source string
*	pparamslist *params - parameters list
*	FILE *f - file for output parsed data
*
* Return value:
* 	none
*/
void fstrparse(const char *plainText, pparamslist *params, FILE *f);

/*
* fstrparse - Parse source string and put parsed data to string.
* WARNING: This function can process memory allocation.
* 
* Parameters:
*	char *plainText - source string
*	pparamslist *params - parameters list
*
* Return value:
* 	char * - parsed string location
*/
char *sstrparse(const char *plainText, pparamslist *params);

/*
* strparse - Parse process function. It can get source text from string or load
* 	it from file. Also it has two methods of loading included files: read
* 	files into memory or use mmap to load theys.
* WARNING: This function can process memory allocation.
* 
* Parameters:
*	char *fileNameOrSrc - specify file name for parsing or prevousely
* 		loaded source text.
*	char *sizeNameOrSrc - if fileNameOrSrc specified text in memory this
* 		parameter is specify text length. For NULL-terminated string
* 		it can be -1.
*	pparamslist *params - parameters list
* 	FILE *outFile - output file or NULL for allocate new string with result
* 	int srcMode - what is in fileNameOrSrc parameter: filename or loaded
* 		text
* 	int loadMode - which mechanism will be used for files loading: load to
* 		memory or use mmap
*
* Return value:
* 	char * - parsed string location or NULL if parsed to file
*/
#define PARSER_SRC_STR		0
#define PARSER_SRC_FILE		1

#define PARSER_FLOAD_MEM	0
#define PARSER_FLOAD_MMAP	1

char *strparse(const char *fileNameOrSrc, int sizeNameOrSrc, pparamslist *params,
    FILE *outFile, int srcMode, int loadMode);

/*
* loadTextFromFile - load text from file using loading method (memory or mmap)
*
* WARNING: This function can process memory allocation.
* 
* Parameters:
*	char *filename - name of file
* 	int loadMode - which mechanism will be used for files loading: load to
* 		memory or use mmap
* 	int *size - pointer to int where loaded data size will be stored
*
* Return value:
* 	char * - loaded text location or NULL if load fails
*/
char *loadTextFromFile(const char *filename, int loadMode, int *size);

/*
* unloadTextFromFile - unload text from memory or mmap file
*
* WARNING: This function can process memory reallocation.
* 
* Parameters:
*	char *text - prevousely loaded text location
* 	int loadMode - which mechanism will be used for unloading: free memory
* 		or munmap
* 	int size - size of text
*
* Return value:
* 	none
*/
void unloadTextFromFile(char *text, int loadMode, int size);

#ifdef __cplusplus
}
#endif 

#endif /* _PARSER_H */
