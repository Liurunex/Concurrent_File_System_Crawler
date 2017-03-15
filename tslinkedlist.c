/* 
 * Author: Zhixin Liu
 * Duck ID: zhixinl
 * UO ID: 951452405
 * Authorship Statement:
 * This is my own work,
 * except the help functions used in linkedlist.c
 */
#include "tslinkedlist.h"
#include "linkedlist.h"
#include <stdlib.h>
#include <pthread.h>

#define LOCK(tsll) &((tsll)->lock)

struct tslinkedlist {
   LinkedList *ll;
   pthread_mutex_t lock;
};

TSLinkedList *tsll_create(void) {

   TSLinkedList *tsll = (TSLinkedList *)malloc(sizeof(TSLinkedList));

   if (tsll != NULL) {
      LinkedList *ll = ll_create(); 
      
      if (ll == NULL) {
         free(tsll);
         tsll = NULL;
      }
      else {
         pthread_mutexattr_t ma;
         pthread_mutexattr_init(&ma);
         pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_RECURSIVE);
         tsll->ll = ll;
         pthread_mutex_init(LOCK(tsll), &ma);
         pthread_mutexattr_destroy(&ma);
      }
   }
   return tsll;
}

void tsll_destroy(TSLinkedList *tsll, void (*userFunction)(void *element)) {
   pthread_mutex_lock(LOCK(tsll));

   ll_destroy(tsll->ll, userFunction); 

   pthread_mutex_unlock(LOCK(tsll)); 
   pthread_mutex_destroy(LOCK(tsll));
   free(tsll);
}

void tslinkedlist_lock(TSLinkedList *tsll) {
   pthread_mutex_lock(LOCK(tsll));
} 

void tslinkedlist_unlock(TSLinkedList *tsll) {
   pthread_mutex_unlock(LOCK(tsll));
} 


int tsll_add(TSLinkedList *tsll, void *element) {
   return tsll_addLast(tsll, element);
}

int tsll_insert(TSLinkedList *tsll, long index, void *element) {
   int status = 0;
   
   pthread_mutex_lock(LOCK(tsll));
   status = ll_insert(tsll->ll, index, element);
   pthread_mutex_unlock(LOCK(tsll));

   return status;
}

int tsll_addFirst(TSLinkedList *tsll, void *element) {
   int status = 0;

   pthread_mutex_lock(LOCK(tsll));
   status = ll_addFirst(tsll->ll, element);
   pthread_mutex_unlock(LOCK(tsll));

   return status;
}

int tsll_addLast(TSLinkedList *tsll, void *element) {
   int status = 0;

   pthread_mutex_lock(LOCK(tsll));
   status = ll_addLast(tsll->ll, element);
   pthread_mutex_unlock(LOCK(tsll));

   return status;
}

void tsll_clear(TSLinkedList *tsll, void (*userFunction)(void *element)) {
   
   pthread_mutex_lock(LOCK(tsll));
   ll_clear(tsll->ll, userFunction);
   pthread_mutex_unlock(LOCK(tsll));
}

int tsll_get(TSLinkedList *tsll, long index, void **element) {
   int status = 0;

   pthread_mutex_lock(LOCK(tsll));
   status = ll_get(tsll->ll, index, element);
   pthread_mutex_unlock(LOCK(tsll));

   return status;
}

int tsll_getFirst(TSLinkedList *tsll, void **element) {
   int status = 0;
   
   pthread_mutex_lock(LOCK(tsll));
   status = ll_getFirst(tsll->ll, element);
   pthread_mutex_unlock(LOCK(tsll));

   return status;
}

int tsll_getLast(TSLinkedList *tsll, void **element) {
   int status = 0;
   
   pthread_mutex_lock(LOCK(tsll));
   status = ll_getLast(tsll->ll, element);
   pthread_mutex_unlock(LOCK(tsll));
   
   return status;
}

int tsll_remove(TSLinkedList *tsll, long index, void **element) {
   int status = 0;

   pthread_mutex_lock(LOCK(tsll));
   status = ll_remove(tsll->ll, index, element);
   pthread_mutex_unlock(LOCK(tsll));

   return status;
}

int tsll_removeFirst(TSLinkedList *tsll, void **element) {
   int status = 0;
   
   pthread_mutex_lock(LOCK(tsll));
   status = ll_removeFirst(tsll->ll, element);
   pthread_mutex_unlock(LOCK(tsll));

   return status;
}

int tsll_removeLast(TSLinkedList *tsll, void **element) {
   int status = 0;

   pthread_mutex_lock(LOCK(tsll));
   status = ll_removeLast(tsll->ll, element);
   pthread_mutex_unlock(LOCK(tsll));

   return status;
}

int tsll_set(TSLinkedList *tsll, long index, void *element, void **previous) {
   int status = 0;

   pthread_mutex_lock(LOCK(tsll));
   status = ll_set(tsll->ll, index, element, previous);
   pthread_mutex_unlock(LOCK(tsll));
   
   return status;
}

long tsll_size(TSLinkedList *tsll) {
   long size = 0;
   
   pthread_mutex_lock(LOCK(tsll));
   size = ll_size(tsll->ll);
   pthread_mutex_unlock(LOCK(tsll));

   return size;
}

void **tsll_toArray(TSLinkedList *tsll, long *len) {
   void **tmp;

   pthread_mutex_lock(LOCK(tsll));
   tmp = ll_toArray(tsll->ll, len);
   pthread_mutex_unlock(LOCK(tsll));

   return tmp;
}

TSIterator *tsll_it_create(TSLinkedList *tsll) {
   TSIterator *it = NULL;
   void **tmp;
   long len;

   pthread_mutex_lock(LOCK(tsll));
   tmp = ll_toArray(tsll->ll, &len);
   
   if (tmp != NULL) {
      it = tsit_create(LOCK(tsll), len, tmp);
      if (it == NULL)
         free(tmp);
   } 
   if (it == NULL)
      pthread_mutex_unlock(LOCK(tsll));

   return it;
}
