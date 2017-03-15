/* 
 * Author: Zhixin Liu
 * Duck ID: zhixinl
 * UO ID: 951452405
 * Authorship Statement:
 * This is my own work,
 * except the help functions used in treeset.c
 */
#include "treeset.h"
#include "tstreeset.h"
#include <stdlib.h>
#include <pthread.h>

#define LOCK(tsts) &((tsts)->lock)

struct tstreeset {
  TreeSet *ts;
  pthread_mutex_t lock;
};

TSTreeSet *tsts_create(int (*cmpFunction)(void *, void *)) {
   TSTreeSet *tsts = (TSTreeSet *)malloc(sizeof(TSTreeSet));
   
   if (tsts != NULL) {
      TreeSet *ts = ts_create(cmpFunction);

      if (ts == NULL) {
         free(tsts);
         tsts = NULL;
      }
      else {
         pthread_mutexattr_t ma;
         pthread_mutexattr_init(&ma);
         pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_RECURSIVE);
         tsts->ts = ts;
         pthread_mutex_init(LOCK(tsts), &ma);
         pthread_mutexattr_destroy(&ma);
      }
   }
   return tsts;
}

void tsts_destroy(TSTreeSet *tsts, void (*userFunction)(void *element)) {
   pthread_mutex_lock(LOCK(tsts));

   ts_destroy(tsts->ts, userFunction); 

   pthread_mutex_unlock(LOCK(tsts)); 
   pthread_mutex_destroy(LOCK(tsts));

   free(tsts);
}

void tstreeset_lock(TSTreeSet *tsts) {
   pthread_mutex_lock(LOCK(tsts));
} 

void tstreeset_unlock(TSTreeSet *tsts) {
   pthread_mutex_unlock(LOCK(tsts));
} 

int tsts_add(TSTreeSet *tsts, void *element) {
   int result;

   pthread_mutex_lock(LOCK(tsts));
   result = ts_add(tsts->ts, element);
   pthread_mutex_unlock(LOCK(tsts));
   
   return result;
}

int tsts_ceiling(TSTreeSet *tsts, void *element, void **ceiling) {
   int result;

   pthread_mutex_lock(LOCK(tsts));
   result = ts_ceiling(tsts->ts, element, ceiling);
   pthread_mutex_unlock(LOCK(tsts));

   return result;
}

void tsts_clear(TSTreeSet *tsts, void (*userFunction)(void *element)) {
   pthread_mutex_lock(LOCK(tsts));
   ts_clear(tsts->ts, userFunction);
   pthread_mutex_unlock(LOCK(tsts));
}

int tsts_contains(TSTreeSet *tsts, void *element) {
   int result;

   pthread_mutex_lock(LOCK(tsts));
   result = ts_contains(tsts->ts, element);
   pthread_mutex_unlock(LOCK(tsts));
   
   return result;
}

int tsts_first(TSTreeSet *tsts, void **element) {
   int result;

   pthread_mutex_lock(LOCK(tsts));
   result = ts_first(tsts->ts, element);
   pthread_mutex_unlock(LOCK(tsts));
   
   return result;
}

int tsts_floor(TSTreeSet *tsts, void *element, void **floor) {
   int result;

   pthread_mutex_lock(LOCK(tsts));
   result = ts_floor(tsts->ts, element, floor);
   pthread_mutex_unlock(LOCK(tsts));
   
   return result;
}

int tsts_higher(TSTreeSet *tsts, void *element, void **higher) {
   int result;

   pthread_mutex_lock(LOCK(tsts));
   result = ts_higher(tsts->ts, element, higher);
   pthread_mutex_unlock(LOCK(tsts));
   
   return result;
}

int tsts_isEmpty(TSTreeSet *tsts) {
   int result;

   pthread_mutex_lock(LOCK(tsts));
   result = ts_isEmpty(tsts->ts);
   pthread_mutex_unlock(LOCK(tsts));
   
   return result;
}

int tsts_last(TSTreeSet *tsts, void **element) {
   int result;

   pthread_mutex_lock(LOCK(tsts));
   result = ts_last(tsts->ts, element);
   pthread_mutex_unlock(LOCK(tsts));
   
   return result;
}

int tsts_lower(TSTreeSet *tsts, void *element, void **lower) {
   int result;

   pthread_mutex_lock(LOCK(tsts));
   result = ts_lower(tsts->ts, element, lower);
   pthread_mutex_unlock(LOCK(tsts));
   
   return result;
}

int tsts_pollFirst(TSTreeSet *tsts, void **element) {
   int result;

   pthread_mutex_lock(LOCK(tsts));
   result = ts_pollFirst(tsts->ts, element);
   pthread_mutex_unlock(LOCK(tsts));
   
   return result;
}

int tsts_pollLast(TSTreeSet *tsts, void **element) {
   int result;

   pthread_mutex_lock(LOCK(tsts));
   result = ts_pollLast(tsts->ts, element);
   pthread_mutex_unlock(LOCK(tsts));
   
   return result;
}

int tsts_remove(TSTreeSet *tsts, void *element, void (*userFunction)(void *element)) {
   int result;

   pthread_mutex_lock(LOCK(tsts));
   result = ts_remove(tsts->ts, element, userFunction);
   pthread_mutex_unlock(LOCK(tsts));
   
   return result;
}

long tsts_size(TSTreeSet *tsts) {
   long size = 0;
   
   pthread_mutex_lock(LOCK(tsts));
   size = ts_size(tsts->ts);
   pthread_mutex_unlock(LOCK(tsts));

   return size;
}

void **tsts_toArray(TSTreeSet *tsts, long *len) {
   void **tmp;

   pthread_mutex_lock(LOCK(tsts));
   tmp = ts_toArray(tsts->ts, len);
   pthread_mutex_unlock(LOCK(tsts));

   return tmp;
}

TSIterator *tsts_it_create(TSTreeSet *tsts) {
   TSIterator *it = NULL;
   void **tmp;
   long len;

   pthread_mutex_lock(LOCK(tsts));
   tmp = ts_toArray(tsts->ts, &len);

   if (tmp != NULL) {
      it = tsit_create(LOCK(tsts), len, tmp);
      if (it == NULL)
         free(tmp);
   }
   if (it == NULL)
      pthread_mutex_unlock(LOCK(tsts));

   return it;
}
