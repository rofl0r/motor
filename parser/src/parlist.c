/*
* parlist.c - parameters list
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
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <strfn.h>
#include <parlist.h>

pparamslist *pparamslist_create() {
    pparamslist *list;

    list = malloc(sizeof(pparamslist));
    list->items = NULL;
    list->count = 0;
    
    return list;
}

void pparamslist_free(pparamslist *list) {
    if (list == NULL) return;
    pparamslist_clear(list);
    free(list);
}

void pparamslist_clear(pparamslist *list) {
    if (list == NULL) return;
    while(list->count) pparamslist_delete(0, list);
}

int pparamslist_add(const char *name, const char *value, pparamslist *list) {
    int res = -1;
    
    if ((name == NULL) || (list == NULL)) return -1;
    
    if (value)
	res = pparamslist_add_ptr(name, strdup(value), strlen(value) + 1, list);
    else
	res = pparamslist_add_ptr(name, 0, 0, list);

    return res;
}

int pparamslist_add_ptr(const char *name, const void *value, int vsize, pparamslist *list) {
    int i;

    if ((name == NULL) || (list == NULL)) return -1;
    i = list->count++;
    
    list->items = realloc(list->items, list->count * sizeof(pparam));
    list->items[i].name = strdup(name);
    list->items[i].value = (char *) value;
    list->items[i].vsize = vsize;
    list->items[i].vtype = LIST_VALUE;
    
    return i;
}

int pparamslist_add_format(const char *name, pparamslist *list, const char *format, ...) {
    char buf[8192];
    va_list arg;
    
    va_start(arg, format);
    vsnprintf(buf, sizeof(buf), format, arg);
    va_end(arg);
    buf[sizeof(buf) - 1] = '\0';
    
    return pparamslist_add(name, buf, list);
}

void pparamslist_add_array(const char *name, const char **values, int arrsize, pparamslist *list) {
    int i, i1;
    char **nval;
    
    if ((name == NULL) || (list == NULL)) return;
    i = list->count++;

    list->items = realloc(list->items, list->count * sizeof(pparam));
    list->items[i].name = strdup(name);

    if ((values != NULL) && (arrsize > 0)) {
	nval = malloc(arrsize * sizeof(char *));
	for (i1 = 0; i1 < arrsize; i1++) nval[i1] = strdup(values[i1]);
	list->items[i].value = nval;
	list->items[i].vsize = arrsize;
    }
    else {
	list->items[i].value = NULL;
	list->items[i].vsize = 0;
    }

    list->items[i].vtype = LIST_ARRAY;
    
    //return &(list->items[i]);
}

pparamslist *pparamslist_add_list(const char *name, pparamslist *list) {
    int i;
    
    if ((name == NULL) || (list == NULL)) return NULL;
    i = list->count++;

    list->items = realloc(list->items, list->count * sizeof(pparam));
    list->items[i].name = strdup(name);
    list->items[i].value = pparamslist_create();
    list->items[i].vsize = sizeof(pparamslist *);
    list->items[i].vtype = LIST_LIST;
    
    return list->items[i].value;
}

int pparamslist_add_array_value_(pparam *p, const char *value) {
    if (p == NULL) return -1;
    p->vsize++;
    p->value = realloc(p->value, p->vsize * sizeof(char *));
    if (value != NULL)
	((char **) p->value)[p->vsize - 1] = strdup(value);
    else
	((char **) p->value)[p->vsize - 1] = NULL;
    
    return p->vsize;
}

int pparamslist_add_array_value(const char *name, const char *value, pparamslist *list) {
    pparam *p;
    
    if ((name == NULL) || (list == NULL)) return -1;
    if ((p = pparamslist_get(name, list)) == NULL) return -1;
    return pparamslist_add_array_value_(p, value);
}

int pparamslist_add_array_valuef(const char *name, pparamslist *list, const char *format, ...) {
    char buf[8192];
    va_list arg;
    
    va_start(arg, format);
    vsnprintf(buf, sizeof(buf), format, arg);
    va_end(arg);
    buf[sizeof(buf) - 1] = '\0';

    return pparamslist_add_array_value (name, buf, list);
}

int pparamslist_add_array_valueI(int index, const char *value, pparamslist *list) {
    pparam *p;
    
    if (list == NULL) return -1;
    if ((p = pparamslist_getI(index, list)) == NULL) return -1;
    return pparamslist_add_array_value_(p, value);
}

void pparamslist_clear_array_(pparam *p) {
    int i;
    
    for (i = 0 ; i < p->vsize; i++) free(((char **) p->value)[i]);
    free(p->value);
    p->value = NULL;
    p->vsize = 0;
}

#define pp list->items[index]

int pparamslist_delete(int index, pparamslist *list) {
    int i;
    pparam *np = NULL, *p;

    if (list == NULL) return -1;
    if ((index < 0) || (index >= list->count)) return list->count;
    
    free(pp.name);
    switch (pp.vtype) {
	case LIST_VALUE:
	    if (pp.vsize) free(pp.value);
	    break;
	case LIST_ARRAY:
	    for (i = 0; i < pp.vsize; i++) free(((char **) pp.value)[i]);
	    free(pp.value);
	    break;
	case LIST_LIST:
	    pparamslist_free(pp.value);
	    break;
    }

    list->count--;
    if (list->count) {
	np = malloc(list->count * sizeof(pparam));
	p = np;
	for (i = 0; i <= list->count; i++) {
	    if (i != index) {
		memcpy(p, &(list->items[i]), sizeof(pparam));
		p++;
	    }
	}
    }

    free(list->items);
    list->items = np;

    return list->count;
}

pparam *pparamslist_get(const char *name, pparamslist *list) {
    int i;
    
    if ((name == NULL) || (list == NULL)) return NULL;

    for (i = 0; i < list->count; i++) {
	if (!strcasecmp(list->items[i].name, name))
	    return &(list->items[i]);
    }
    
    return NULL;
}

pparam *pparamslist_get_(const char *name, int namesize, pparamslist *list) {
    int i;
    
    if ((name == NULL) || (list == NULL)) return NULL;

    for (i = 0; i < list->count; i++) {
	if ((!strncasecmp(list->items[i].name, name, namesize))
	    && (!list->items[i].name[namesize]))
	    return &(list->items[i]);
    }
    
    return NULL;
}

pparam *pparamslist_getI(int index, pparamslist *list) {
    if (list == NULL) return NULL;
    if (index >= list->count) return NULL;
    return &(list->items[index]);
}

int pparamslist_index(const char *name, pparamslist *list) {
    int i;
    
    if ((name == NULL) || (list == NULL)) return -1;

    for (i = 0; i < list->count; i++)
	if (!strcasecmp(list->items[i].name, name)) return i;
    
    return -1;
}

int pparamslist_index_(const char *name, int vtype, pparamslist *list) {
    int i;
    
    if ((name == NULL) || (list == NULL)) return -1;

    for (i = 0; i < list->count; i++)
	if ((!strcasecmp(list->items[i].name, name))
	    && (list->items[i].vtype == vtype)) return i;
    
    return -1;
}

char *pparamslist_value(const char *name, pparamslist *list) {
    const char *s;
    char *se;
    pparam *p;
    int i;
    
    if (!name || !list) return NULL;
    
    if ((s = strchr(name, '.'))) {
	i = strtol(name, &se, 10);
	if (se == s) p = pparamslist_getI(i, list);
	else p = pparamslist_get_(name, s - name, list);
	
        if (!p) return NULL;

	s++;
	switch (p->vtype) {
	    case LIST_VALUE: return NULL;
	    case LIST_ARRAY:
		if ((i = atoi(s)) >= p->vsize) return NULL;
		else return ((char **) p->value)[i];
	    case LIST_LIST: return pparamslist_value(s, p->value);
	}
    }
    else {
	s = name;
	if (s[0] == '^') s++;
    
	if (strisdecnumber(s)) p = pparamslist_getI(atoi(s), list);
	else p = pparamslist_get(s, list);

	if (p == NULL) return NULL;
	
	if (name[0] == '^') return p->name;
	else 
	    switch (p->vtype) {
		case LIST_VALUE: return p->value;
		case LIST_ARRAY: return itoa(p->vsize);
		case LIST_LIST: return itoa(((pparamslist *) p->value)->count);
	    }
    }
    return NULL;
}

int pparamslist_exist(const char *name, pparamslist *list) {
    char *s;
    pparam *p;
    
    if ((name == NULL) || (list == NULL)) return 0;
    
    if ((s = strchr(name, '.')) != NULL) {
        if ((p = pparamslist_get_(name, s - name, list)) == NULL) return 0;
	s++;
	switch (p->vtype) {
	    case LIST_VALUE: return 0;
	    case LIST_ARRAY: return (atoi(s) < p->vsize);
	    case LIST_LIST: return pparamslist_exist(s, p->value);
	}
    }
    else {
	return (pparamslist_get(name, list) != NULL);
    }
    return 0;
}

void pparamslist_set(const char *name, const char *value, pparamslist *list) {
    if ((name == NULL) || (list == NULL)) return;
    
    if (value)
	pparamslist_set_ptr(name, strdup(value), strlen(value) + 1, list);
    else
	pparamslist_set_ptr(name, 0, 0, list);
}

void pparamslist_set_ptr(const char *name, const void *value, int vsize, pparamslist *list) {
    int i;
    
    if ((name == NULL) || (list == NULL)) return;

    if ((i = pparamslist_index_(name, LIST_VALUE, list)) < 0) {
	pparamslist_add_ptr(name, value, vsize, list);
    }
    else {
	if (list->items[i].vsize) free(list->items[i].value);
	list->items[i].value = (char *) value;
	list->items[i].vsize = vsize;
    }
}

void pparamslist_set_format(const char *name, pparamslist *list, const char *format, ...) {
    char buf[8192];
    va_list arg;
    
    va_start(arg, format);
    vsnprintf(buf, sizeof(buf), format, arg);
    va_end(arg);
    buf[sizeof(buf) - 1] = '\0';
    
    pparamslist_set(name, buf, list);
}

void pparamslist_unset(const char *name, pparamslist *list) {
    int i;
    
    if ((name == NULL) || (list == NULL)) return;

    if ((i = pparamslist_index_(name, LIST_VALUE, list)) < 0) return;
    pparamslist_delete(i, list);
}

pparam *pparamslist_array2list(const char *name, pparamslist *list) {
    int i, cnt;
    char **values;
    pparam *arr;

    if ((name == NULL) || (list == NULL)) return NULL;
    if ((i = pparamslist_index(name, list)) < 0) return NULL;
    if ((arr = &(list->items[i])) == NULL) return NULL;
    if (arr->vtype != LIST_ARRAY) return NULL;
    
    cnt = arr->vsize;
    arr->vsize = 0;
    values = arr->value;
    arr->value = pparamslist_create();
    arr->vtype = LIST_LIST;
    
    for (i = 0; i < cnt; i++) {
	pparamslist_add(itoa(i), values[i], arr->value);
	free(values[i]);
    }
    free(values);
    
    return arr;
}
