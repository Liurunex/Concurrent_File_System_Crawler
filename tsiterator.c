/* 
 * Author: Zhixin Liu
 * Duck ID: zhixinl
 * UO ID: 951452405
 * Authorship Statement:
 * This is my own work,
 * except the help functions used in iterator.c
 */
#include "tsiterator.h"
#include <stdlib.h>
#include <pthread.h>

struct tsiterator {
   long next;
   long size;
   void **elements;
   pthread_mutex_t *lock;
};

TSIterator *tsit_create(pthread_mutex_t *lock, long size, void **elements) {
   TSIterator *it = (TSIterator *)malloc(sizeof(TSIterator));

   if (it != NULL) {
      it->next = 0L;
      it->size = size;
      it->elements = elements;
      it->lock = lock;
   }
   return it;
}

int tsit_hasNext(TSIterator *it) {
   return (it->next < it->size) ? 1 : 0;
}

int tsit_next(TSIterator *it, void **element) {
   int status = 0;
   if (it->next < it->size) {
      *element = it->elements[it->next++];
      status = 1;
   }
   return status;
}

void tsit_destroy(TSIterator *it) {
   free(it->elements);
   pthread_mutex_unlock(it->lock);
   free(it);
}
