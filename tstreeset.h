/* 
 * Author: Zhixin Liu
 * Duck ID: zhixinl
 * UO ID: 951452405
 * Authorship Statement:
 * This is my own work,
 */
#ifndef _TSTREESET_H_
#define _TSTREESET_H_

#include "tsiterator.h"

typedef struct tstreeset TSTreeSet;	

TSTreeSet *tsts_create(int (*cmpFunction)(void *, void *));

void tsts_destroy(TSTreeSet *tsts, void (*userFunction)(void *element));

void tstreeset_lock(TSTreeSet *tsts);

void tstreeset_unlock(TSTreeSet *tsts);

int tsts_add(TSTreeSet *tsts, void *element);

int tsts_ceiling(TSTreeSet *tsts, void *element, void **ceiling);

void tsts_clear(TSTreeSet *tsts, void (*userFunction)(void *element));

int tsts_contains(TSTreeSet *tsts, void *element);

int tsts_first(TSTreeSet *tsts, void **element);

int tsts_floor(TSTreeSet *tsts, void *element, void **floor);

int tsts_higher(TSTreeSet *tsts, void *element, void **higher);

int tsts_isEmpty(TSTreeSet *tsts);

int tsts_last(TSTreeSet *tsts, void **element);

int tsts_lower(TSTreeSet *tsts, void *element, void **lower);

int tsts_pollFirst(TSTreeSet *tsts, void **element);

int tsts_pollLast(TSTreeSet *tsts, void **element);

int tsts_remove(TSTreeSet *tsts, void *element, void (*userFunction)(void *element));

long tsts_size(TSTreeSet *tsts);

void **tsts_toArray(TSTreeSet *tsts, long *len);

TSIterator *tsts_it_create(TSTreeSet *tsts);

#endif /* _TSTREESET_H_ */
