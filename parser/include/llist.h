/*
* llist.h - manipulate linked lists
*
* Copyright (C) 1999-2000 by Roman Khnykin <romaroma@rr.org.ua>
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
* Created:	10.12.1999	romaroma	Initial revision.
* Changed:	06.03.2000	romaroma	new function:
*
*/

#ifndef _LLIST_H
#define _LLIST_H

#ifdef __cplusplus
extern "C" {
#endif 

/* Item structure */
typedef struct {
    void *prev;
    void *item;
    void *next;
} llist_item;

/* Add a new item */
llist_item *llist_add(llist_item *list, void *item);

/* Delete item specified by index */
llist_item *llist_delete(llist_item *list, int index);

/* Delete item(s) specified by pointer */
llist_item *llist_delete_item(llist_item *list, void *item);

/* Get list count */
int llist_count(llist_item *list);

/* Get item specified by index */
void *llist_get(llist_item *list, int index);

/* Get item index */
int llist_get_index(llist_item *list, void *item);

/* Set item specified by index */
llist_item *llist_set(llist_item *list, int index, void *item);

/* Get first item in list */
llist_item *llist_first(llist_item *list);

/* Get last item in list */
llist_item *llist_last(llist_item *list);

/* Get prevous item in list */
llist_item *llist_prev(llist_item *list);

/* Get next item in list */
llist_item *llist_next(llist_item *list);

#ifdef __cplusplus
}
#endif 

#endif /* _LLIST_H */
