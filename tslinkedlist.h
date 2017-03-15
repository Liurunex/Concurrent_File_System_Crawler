/* 
 * Author: Zhixin Liu
 * Duck ID: zhixinl
 * UO ID: 951452405
 * Authorship Statement:
 * This is my own work,
 */
#ifndef _TSLINKEDLIST_H_
#define _TSLINKEDLIST_H_

#include "tsiterator.h"

typedef struct tslinkedlist TSLinkedList;		

TSLinkedList *tsll_create(void);

void tsll_destroy(TSLinkedList *tsll, void (*userFunction)(void *element));

void tslinkedlist_lock(TSLinkedList *tsll);

void tslinkedlist_unlock(TSLinkedList *tsll);

int tsll_add(TSLinkedList *tsll, void *element);

int tsll_insert(TSLinkedList *tsll, long index, void *element);

int tsll_addFirst(TSLinkedList *tsll, void *element);

int tsll_addLast(TSLinkedList *tsll, void *element);

void tsll_clear(TSLinkedList *tsll, void (*userFunction)(void *element));

int tsll_get(TSLinkedList *tsll, long index, void **element);

int tsll_getFirst(TSLinkedList *tsll, void **element);

int tsll_getLast(TSLinkedList *tsll, void **element);

int tsll_remove(TSLinkedList *tsll, long index, void **element);

int tsll_removeFirst(TSLinkedList *tsll, void **element);

int tsll_removeLast(TSLinkedList *tsll, void **element);

int tsll_set(TSLinkedList *tsll, long index, void *element, void **previous);

long tsll_size(TSLinkedList *tsll);

void **tsll_toArray(TSLinkedList *tsll, long *len);

TSIterator *tsll_it_create(TSLinkedList *tsll);

#endif /* _TSLINKEDLIST_H_ */
