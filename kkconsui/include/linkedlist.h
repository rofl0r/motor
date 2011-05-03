#ifndef KONST_LIST_H
#define KONST_LIST_H

#include <stdlib.h>

#include "conf.h"

typedef int (listcompare)(void *ptr1, void *ptr2);
typedef void (listfreeitem)(void *ptr1);
typedef void *(listforeachfunc)(void *elem, void *arg);

__KTOOL_BEGIN_NAMESPACE

class linkedlist {
    private:
	struct flinkedlist {
	    void *data;
	    flinkedlist *next;
	};

	flinkedlist *flist;
	int i;

    public:
	int count;
	listfreeitem *freeitem;
	    // user defined function to free a list item
    
	linkedlist();
	~linkedlist();
	
	void add(void *p);
	    // adds new element to the list

	void insert(int n, void *p);
	    // inserts an element to n-th position

	void sort(listcompare *compare);
	    // sorts the list

	void remove(int n);
	    // removes an element number n from the list

	void replace(int n, void *p);
	    // replaces n-th element value with new data

	void empty();
	    // clears the list

	void* at(int n);
	    // gets n-th element pointer from the list

	void* find(void *p, listcompare *compare);
	    // returns a pointer to the element found equal to p
	    // with compare function

	int findnum(void *p, listcompare *compare);
	    // the same as previous function, except for it
	    // returs a number, not pointer

	void *foreach(listforeachfunc *exec, void *arg);

	void* operator[] (int idx) {
	    return at(idx);
	}
};

__KTOOL_END_NAMESPACE

#ifdef __KTOOL_USE_NAMESPACES

using ktool::linkedlist;

#endif

#endif
