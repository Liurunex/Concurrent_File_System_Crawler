/* 
 * Author: Zhixin Liu
 * Duck ID: zhixinl
 * UO ID: 951452405
 * Authorship Statement:
 * This is my own work
 */
#ifndef _TSITERATOR_H_
#define _TSITERATOR_H_

#include <pthread.h>

typedef struct tsiterator TSIterator;

TSIterator *tsit_create(pthread_mutex_t *lock, long size, void **elements);

int tsit_hasNext(TSIterator *it);

int tsit_next(TSIterator *it, void **element);

void tsit_destroy(TSIterator *it);

#endif /* _TSTREESET_H_ */
