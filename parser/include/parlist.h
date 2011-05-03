/*
* parlist.h - parameters list
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
* Changed:	19.07.2000	romaroma	Bugfix.
* Changed:	17.08.2000	romaroma	New functions.
* Changed:	20.08.2000	romaroma	New functions.
*
*/

#ifndef _PARLIST_H
#define _PARLIST_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>

/*****************************************************************************/
/* Internal list data */
/*****************************************************************************/

#define LIST_VALUE	0
#define LIST_ARRAY	1
#define LIST_LIST	2

typedef struct pparam_struct {
    char *name;
    void *value;
    int vsize;
    int vtype;
} pparam;

typedef struct pparamslist_struct {
    pparam *items;
    int count;
} pparamslist;


/*****************************************************************************/
/* Functions for create/delete lists */
/*****************************************************************************/

/*
* pparamslist_create - Create a new list.
* 
* Parameters:
* 	none
*
* Return value:
* 	pparamslist * - new list address
*/
pparamslist *pparamslist_create();

/*
* pparamslist_free - Delete list.
* 
* Parameters:
* 	pparamslist *list - list address
*
* Return value:
* 	none
*/
void pparamslist_free(pparamslist *list);


/*****************************************************************************/
/* Functions for manipulate list items */
/*****************************************************************************/

/*
* pparamslist_add - Add new text value to list.
* 
* Parameters:
* 	char *name - variable name
* 	char *value - variable value
* 	pparamslist *list - list address
*
* Return value:
* 	int - index of new variable
*/
int pparamslist_add(const char *name, const char *value, pparamslist *list);

/*
* pparamslist_add_ptr - Add new pointer to list (not create copy of value).
* 
* Parameters:
* 	char *name - variable name
* 	void *value - variable value
* 	int vsize - variable size. If it's zero - value never be freed.
* 	pparamslist *list - list address
*
* Return value:
* 	int - index of new variable
*/
int pparamslist_add_ptr(const char *name, const void *value, int vsize, pparamslist *list);

/*
* pparamslist_add_format - Add new text value to list using printf format.
* 
* Parameters:
* 	char *name - variable name
* 	pparamslist *list - list address
* 	char *format - value format string
* 	... - values
*
* Return value:
* 	int - index of new variable
*/
int pparamslist_add_format(const char *name, pparamslist *list, const char *format, ...);

/*
* pparamslist_add_list - Create new sub-list and add it to exist list as item.
* 
* Parameters:
* 	char *name - new list name
* 	pparamslist *list - list address
*
* Return value:
* 	pparamslist * - new sub-list address
*/
pparamslist *pparamslist_add_list(const char *name, pparamslist *list);

/*
* pparamslist_add_array - Create new array and add it to exist list as item.
*	This function can copy data from any existing array.
* 
* Parameters:
* 	char *name - array name
*	char **values - existing array (or NULL if doesn't need copy)
*	int *arsize - existing array length (or 0 if doesn't need copy)
* 	pparamslist *list - list address
*
* Return value:
* 	none
*/
void pparamslist_add_array(const char *name, const char **values, int arrsize, pparamslist *list);

/*
* pparamslist_set - Set new variable value.
*	If variable not found it will be created.
* 
* Parameters:
* 	char *name - variable name
* 	char *value - variable value
* 	pparamslist *list - list address
*
* Return value:
* 	none
*/
void pparamslist_set(const char *name, const char *value, pparamslist *list);

/*
* pparamslist_set_ptr - Set new variable pointer (not create copy of value).
*	If variable not found it will be created.
* 
* Parameters:
* 	char *name - variable name
* 	void *value - variable pointer
* 	int vsize - variable size. If it's zero - value never be freed.
* 	pparamslist *list - list address
*
* Return value:
* 	none
*/
void pparamslist_set_ptr(const char *name, const void *value, int vsize, pparamslist *list);

/*
* pparamslist_set_format - Set new text value to list using prinf format.
*	If variable not found it will be created.
* 
* Parameters:
* 	char *name - variable name
* 	pparamslist *list - list address
* 	char *format - value format string
* 	... - values
*
* Return value:
* 	none
*/
void pparamslist_set_format(const char *name, pparamslist *list, const char *format, ...);

/*
* pparamslist_unset - Delete variable from list.
*	WARNING: This function can't remove lists or arrays!
* 
* Parameters:
* 	char *name - variable name
* 	pparamslist *list - list address
*
* Return value:
* 	none
*/
void pparamslist_unset(const char *name, pparamslist *list);

/*
* pparamslist_delete - Delete item from list.
* 
* Parameters:
* 	int index - item index
* 	pparamslist *list - list address
*
* Return value:
* 	int - count of items in list
*/
int pparamslist_delete(int index, pparamslist *list);

/*
* pparamslist_clear - Delete all items from list.
* 
* Parameters:
* 	pparamslist *list - list address
*
* Return value:
* 	none
*/
void pparamslist_clear(pparamslist *list);

/*
* pparamslist_get - Get item by name.
* 
* Parameters:
*	char *name - item name
* 	pparamslist *list - list address
*
* Return value:
* 	pparam * - item address (or NULL if no items found)
*/
pparam *pparamslist_get(const char *name, pparamslist *list);

/*
* pparamslist_getI - Get item by index.
* 
* Parameters:
*	int index - item index
* 	pparamslist *list - list address
*
* Return value:
* 	pparam * - item address (or NULL if no items found)
*/
pparam *pparamslist_getI(int index, pparamslist *list);

/*
* pparamslist_value - Get value from list.
*	This function try to get value from list using nested lists and arrays.
*	If value name is a name of list or array function will return their
*	sizes.
* 
* Parameters:
*	char *name - value name
*		for nested lists/arrays it's possible to use use name with dots
*		(ex. "aaa.bbb.ccc.0")
* 	pparamslist *list - list address
*
* Return value:
* 	char * - value address (or NULL if no items found)
*/
char *pparamslist_value(const char *name, pparamslist *list);

/*
* pparamslist_index - Get item index by name.
* 
* Parameters:
*	char *name - item name
* 	pparamslist *list - list address
*
* Return value:
* 	int - item index (or -1 if no items founs)
*/
int pparamslist_index(const char *name, pparamslist *list);

/*
* pparamslist_exist - Check for item exists.
*	This function tries to get item from list using nested lists and arrays
*	and returns state of item.
* 
* Parameters:
*	char *name - value name
*		for nested lists/arrays it's possible to use use name with dots
*		(ex. "aaa.bbb.ccc.0")
* 	pparamslist *list - list address
*
* Return value:
* 	int - not zero if item exist
*/
int pparamslist_exist(const char *name, pparamslist *list);


/*****************************************************************************/
/* Functions for manipulate arrays */
/*****************************************************************************/

/*
* pparamslist_add_array_value - Add new value to array specified by name.
*	This function finds array in list and add a new value to it.
* 
* Parameters:
*	char *name - array name
*	char *value - new value
* 	pparamslist *list - list address
*
* Return value:
* 	int - new array size
*/
int pparamslist_add_array_value(const char *name, const char *value, pparamslist *list);

/*
* pparamslist_add_array_valuef - Add new value to array specified by name using
* 	printf format.
*	This function finds array in list and add a new value to it.
* 
* Parameters:
*	char *name - array name
* 	pparamslist *list - list address
* 	const char *format - value format string
* 	... - values
*
* Return value:
* 	int - new array size
*/
int pparamslist_add_array_valuef(const char *name, pparamslist *list, const char *format, ...);

/*
* pparamslist_add_array_valueI - Add new value to array specified by index.
*	This function finds array in list and add a new value to it.
* 
* Parameters:
*	int index - array index
*	char *value - new value
* 	pparamslist *list - list address
*
* Return value:
* 	int - new array size
*/
int pparamslist_add_array_valueI(int index, const char *value, pparamslist *list);

/*
* pparamslist_add_array_value_ - Add new value to array.
*	This function adds a new value to array specified as list item.
* 
* Parameters:
*	pparam *p - array item
*	char *value - new value
*
* Return value:
* 	int - new array size
*/
int pparamslist_add_array_value_(pparam *p, const char *value);

/*
* pparamslist_clear_array_ - Clear array.
*	This function clears a array specified as list item.
* 
* Parameters:
*	pparam *p - array item
*
* Return value:
* 	none
*/
void pparamslist_clear_array_(pparam *p);

/*
* pparamslist_array2list - Convert array, specified by name, to sub-list.
*	New sub-list items will be named by index ("0", "1", etc.).
* 
* Parameters:
*	char *name - array name
* 	pparamslist *list - list address
*
* Return value:
* 	pparam * - new item address
*/
pparam *pparamslist_array2list(const char *name, pparamslist *list);

#ifdef __cplusplus
}
#endif 

#endif /* _PARLIST_H */
