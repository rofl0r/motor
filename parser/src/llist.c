/*
* llist.c - manipulate linked lists
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

#include <stdlib.h>

#include <llist.h>

/*
------------------
Internal functions
------------------
*/

/* Returns item by index */
llist_item *item_by_index(llist_item *list, int index) {
    int cnt = 0;
    llist_item *it = llist_first(list);
    
    if (!it) return 0;
    while ((cnt < index) && (it)) {
	cnt++;
	it = it->next;
    }
    return it;
}

/* Delete item */
llist_item *item_delete(llist_item *item) {
    llist_item *it = 0;
    
    if (!item) return 0;
    
    if (item->prev) ((llist_item *)item->prev)->next = item->next;
    if (item->next) ((llist_item *)item->next)->prev = item->prev;
    
    if (item->prev) it = item->prev;
    else it = item->next;
    free(item);

    return it;
}



/*
-------------------
Interface functions
-------------------
*/

/* Add a new item */
llist_item *llist_add(llist_item *list, void *item) {
    llist_item	*ni = calloc(1, sizeof(llist_item)),
		*li = llist_last(list);

    ni->item = item;
    if (li) {
	li->next = ni;
	ni->prev = li;
    }
    return ni;
}

/* Delete item specified by index */
llist_item *llist_delete(llist_item *list, int index) {
    return item_delete(item_by_index(list, index));
}

/* Delete item(s) specified by pointer */
llist_item *llist_delete_item(llist_item *list, void *item) {
    llist_item	*it = llist_first(list);
    
    while (it) {
	if (it->item == item) return item_delete(it);
	it = it->next;
    }

    return list;
}

/* Get list count */
int llist_count(llist_item *list) {
    int i = 0;
    llist_item	*it = llist_first(list);
    
    while (it) {
	i++;
	it = it->next;
    }
    
    return i;
}

/* Get item specified by index */
void *llist_get(llist_item *list, int index) {
    llist_item *it = item_by_index(list, index);
    
    if (!it) return 0;
    return it->item;
}

/* Get item index */
int llist_get_index(llist_item *list, void *item) {
    llist_item	*it = llist_first(list);
    int num = -1;
    
    while (it) {
	num++;
	if (it->item == item) return num;
	it = it->next;
    }

    return -1;
}

/* Set item specified by index */
llist_item *llist_set(llist_item *list, int index, void *item) {
    llist_item *it = item_by_index(list, index);
    
    if (!it) return 0;
    
    it->item = item;
    
    return it;
}

/* Get first item in list */
llist_item *llist_first(llist_item *list) {
    llist_item *i = list;

    if (!i) return 0;
    while (i->prev) i = i->prev;
    return i;
}

/* Get last item in list */
llist_item *llist_last(llist_item *list) {
    llist_item *i = list;

    if (!i) return 0;
    while (i->next) i = i->next;
    return i;
}

/* Get prevous item in list */
llist_item *llist_prev(llist_item *list) {
    if (!list) return 0;
    return list->prev;
}

/* Get next item in list */
llist_item *llist_next(llist_item *list) {
    if (!list) return 0;
    return list->next;
}

