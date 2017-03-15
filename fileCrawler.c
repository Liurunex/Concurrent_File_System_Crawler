#define _DEFAULT_SOURCE 1
/* 
 * Author: Zhixin Liu
 * Duck ID: zhixinl
 * UO ID: 951452405
 * Authorship Statement:
 * This is my own work,
 * except the help functions used in fileCrawlerSinger.c
 */

#include <sys/types.h>
#include <dirent.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "tslinkedlist.h"
#include "tstreeset.h"
#include "linkedlist.h"
#include "treeset.h"
#include "re.h"

#define LOCK(ts) &((ts)->lock)

int workersAllclear = 0; /* marker to determine if all work is done */
int active_counter = 0; /* record # of active worker, it is a critical section!!!!!! */
int signal_Main = 1; 

pthread_mutex_t worker_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t RE_lock = PTHREAD_MUTEX_INITIALIZER; /* make the fucntion called from re class to be thread-safe*/
pthread_cond_t cond_WQ = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_Main = PTHREAD_COND_INITIALIZER;

struct tslinkedlist {
   LinkedList *ll;
   pthread_mutex_t lock;
};

struct tstreeset {
  TreeSet *ts;
  pthread_mutex_t lock;
};

typedef struct arg_ll_ts {
    TSLinkedList *ll;
    TSTreeSet *ts;
    RegExp *reg;
} All_ADTs;

/* thread-safe function to increse active_counter*/
static void ts_counter_increase () {
	pthread_mutex_lock(&worker_lock);
	active_counter++;
	pthread_mutex_unlock(&worker_lock);
}
/* thread-safe function to decrease active_counter*/
static void ts_counter_reduce () {
	pthread_mutex_lock(&worker_lock);
	active_counter--;
	pthread_mutex_unlock(&worker_lock);
}

/*
static int ts_counter_judge() {
	int result = 0;
	pthread_mutex_lock(&worker_lock);
	if (active_counter == 1) { 
		result = 1;
		if (signal_Main){
			signal_Main = 0;
			workersAllclear = 1;
			pthread_cond_signal(&cond_Main);
		}
	}
	pthread_mutex_unlock(&worker_lock);

	return result;
}
*/

static void cvtPattern(char pattern[], const char *bashpat) {
	char *p = pattern;
	*p++ = '^';
	while (*bashpat != '\0') {
		switch (*bashpat) {
			case '*':
				*p++ = '.';
				*p++ = '*';
				break;
			case '.':
				*p++ = '\\';
				*p++ = '.';
				break;
			case '?':
				*p++ = '.';
				break;
			default:
				*p++ = *bashpat;
		}
		bashpat++;
	}
	*p++ = '$';
	*p = '\0';
}

/* change the original one to non-recursive version, used inside applyRe() */
static int processDirectory(char *dirname, TSLinkedList *ll, int verbose) {
	DIR *dd;
	char *sp;
	int len, status = 1;
	char d[4096];

   	/* eliminate trailing slash, if there */
	strcpy(d, dirname);
	len = strlen(d);
	if (len > 1 && d[len-1] == '/')
		d[len-1] = '\0';
   
	/* open the directory */
	if ((dd = opendir(d)) == NULL) {
		if (verbose)
			fprintf(stderr, "Error opening directory `%s'\n", d);
		return 1;
	}
   
	/* duplicate directory name to insert into linked list */
	sp = strdup(d);
	if (sp == NULL) {
		fprintf(stderr, "Error adding `%s' to linked list\n", d);
		status = 0;
		goto cleanup;
	}
	if (!tsll_add(ll, sp)) {
		fprintf(stderr, "Error adding `%s' to linked list\n", sp);
		free(sp);
		status = 0;
		goto cleanup;
	}
	if (len == 1 && d[0] == '/')
		d[0] = '\0';

cleanup:
	(void) closedir(dd);
	return status;
}

static int scmp(void *a, void *b) {
	return strcmp((char *)a, (char *)b);
}

/* change it so that it would adding scaned direcotry into WorkQueue */
static int applyRe(char *dir, RegExp *reg, All_ADTs *alls) {
	DIR *dd;
	struct dirent *dent;
	int re_match_result;
	int status = 1;
	char d[4096];
	TSTreeSet *ts = alls->ts;
	TSLinkedList *ll = alls->ll;
	
	strcpy(d, dir);

	if ((dd = opendir(dir)) == NULL) {
     	fprintf(stderr, "Error opening directory `%s'\n", dir);
      	return 0;
   	}
  	
  	/* scans the entries in the directroy */
	while (status && (dent = readdir(dd)) != NULL) {
		if (strcmp(".", dent->d_name) == 0 || strcmp("..", dent->d_name) == 0)
			continue;

		/* the case where the entry is file*/
		if (!(dent->d_type & DT_DIR)) {
			char b[4096], *sp;

			/* make the re_match() call be thread-safe */
			pthread_mutex_lock(&RE_lock);
			re_match_result = re_match(reg, dent->d_name);
			pthread_mutex_unlock(&RE_lock);
			if (! re_match_result)
				continue;
			sprintf(b, "%s/%s", dir, dent->d_name);

			if ((sp = strdup(b)) != NULL) {
				if (!tsts_add(ts, sp)) {
					fprintf(stderr, "Error adding `%s' to tree set\n", sp);
					free(sp);
					status = 0;
					break;
				}
			} 
			else {
				fprintf(stderr, "Error adding `%s' to tree set\n", b);
				status = 0;
				break;
			}
      	} 

      	/* the case where the entry is directory*/
      	else {
      		char bb[4096];
      		/* get the absolute address*/
         	sprintf(bb, "%s/%s", d, dent->d_name);
		 	status = processDirectory(bb, ll, 1);
		 	/* if successfully write into WorkQue, signal waiting thread */
		 	if (status)	
		 		pthread_cond_signal(&cond_WQ);
      	}
   	}
   (void) closedir(dd);
   return status;
}

/* worker thread function */
static void *work_thread(void *arg) {
	char *sp;
	int stat;
	All_ADTs *alls = (All_ADTs *) arg;
	TSLinkedList *ll = alls->ll;
	RegExp *reg = alls->reg;

	while (1) {

		ts_counter_increase();

		/* the case when WorkQueue is not empty */
		if (tsll_removeFirst(ll, (void **)&sp)) {
			/* retrieve one directory and scan it */
			stat = applyRe(sp, reg, alls);
			free(sp);
			if (! stat) {
				fprintf(stderr, "Error: applyRe() error \n");
				ts_counter_reduce();
				break;	
			}
			else 
				ts_counter_reduce();
		}
		/* the case when WorkQueue is empty */
		else {
			pthread_mutex_lock(&worker_lock);
			/* if should trigger all workers go terminated */
			if (active_counter == 1) { 
				active_counter--;
				if (signal_Main){
					signal_Main = 0;
					workersAllclear = 1;
					pthread_cond_signal(&cond_Main);
				}
				pthread_mutex_unlock(&worker_lock);
				break;
			}

			/* when WorkQueue is empty and have other workers are working, wait for conditioanl varible */
			while (tsll_size(ll) == 0 && active_counter > 1) {
				active_counter--;
				pthread_cond_wait(&cond_WQ, &worker_lock);
				active_counter++;
				/* after awake, worker find all work has been done */
				if (workersAllclear)
					break;
			}

			/* if awake thread should go to terminate */
			if (workersAllclear) {
				active_counter--;
				pthread_mutex_unlock(&worker_lock);	
				break;
			}

			active_counter--;
			pthread_mutex_unlock(&worker_lock);
		}
	}
	/* inform other workers to awake*/
	pthread_cond_broadcast(&cond_WQ);
	return NULL;
}

int main(int argc, char *argv[]) {
	
	char pattern[4096];
	TSLinkedList *ll = NULL;
	TSTreeSet *ts = NULL;
	RegExp *reg;
	TSIterator *it;
	All_ADTs *thread_arg = NULL;
	char* crawler_threads;
	int thread_num = 2;
	int i = 0;
	pthread_t *worker = NULL;

	if ((crawler_threads = getenv("CRAWLER_THREADS")) != NULL) {
		thread_num = atoi(crawler_threads);
		if (thread_num < 1)
			thread_num = 2;
	}

	if (argc < 2) {
		fprintf(stderr, "Usage: ./fileCrawler pattern [dir] ...\n");
		return -1;
	}

	/* convert bash expression to regular expression and compile */
	cvtPattern(pattern, argv[1]);
	if ((reg = re_create()) == NULL) {
		fprintf(stderr, "Error creating Regular Expression Instance\n");
		return -1;
	}
	if (! re_compile(reg, pattern)) {
		char eb[4096];
		re_status(reg, eb, sizeof eb);
		fprintf(stderr, "Compile error - pattern: `%s', error message: `%s'\n",
		pattern, eb);
		re_destroy(reg);
		return -1;
	}
	
	/* create linked list and treeset */
	if ((ll = tsll_create()) == NULL) {
		fprintf(stderr, "Unable to create linked list\n");
		goto done;
	}
	if ((ts = tsts_create(scmp)) == NULL) {
		fprintf(stderr, "Unable to create tree set\n");
		goto done;
	}

	/* malloc the data structure which would store all data needed by worker */
	thread_arg = (All_ADTs *)malloc(sizeof(All_ADTs));

	if (thread_arg == NULL){
		fprintf(stderr, "Error: cannot malloc \n");
		goto done;
	}

	thread_arg->ll = ll;
	thread_arg->ts = ts;
	thread_arg->reg = reg;

	/* main thread write directory name into linked list */
	if (argc == 2) {
		 if (! processDirectory(".", ll, 1)) {
			fprintf(stderr, "Error to write directory name into linkedlist\n");
			goto done;
		}
	} 
	else {
		int i;
		for (i = 2; i < argc; i++) {
			if (! processDirectory(argv[i], ll, 1)) {
				fprintf(stderr, "Error to write directory name into linkedlist\n");
				goto done;
			}
		}
	}

	/* create the worker list */
	worker = (pthread_t *)malloc(sizeof(pthread_t) * thread_num);

	if (worker == NULL) {
		fprintf(stderr, "Error: cannot malloc \n");
		goto done;
	}

	/* create threads for worker*/
	for (i = 0; i < thread_num; i++) {
        if (pthread_create(&worker[i], NULL, &work_thread, (void *) thread_arg)) {
        	fprintf(stderr, "Error to create threads\n");
    		goto done;
        }
    }

    /* waiting for each worker to terminate */
	for (i = 0; i < thread_num; i++) {
		pthread_join(worker[i], NULL);
    }

    /* Harvest Thread */
    /* wait for all worker finished */
    pthread_mutex_lock(&worker_lock);
    while (!workersAllclear || (tsts_size(ts) == 0))
    	pthread_cond_wait(&cond_Main, &worker_lock);
	
	/* create iterator to traverse files matching pattern in sorted order */
	if ((it = tsts_it_create(ts)) == NULL) {
		fprintf(stderr, "Unable to create iterator over tree set\n");
		goto done;
	}
	while (tsit_hasNext(it)) {
		char *s;
		(void) tsit_next(it, (void **)&s);
		printf("%s\n", s);
	}
	tsit_destroy(it);
	pthread_mutex_unlock(&worker_lock);
    
	/* cleanup after ourselves so there are no memory leaks */
	done:
		if (ll != NULL)
			tsll_destroy(ll, free);
		if (ts != NULL)
			tsts_destroy(ts, free);
		if (thread_arg != NULL)
			free(thread_arg);
		if (worker != NULL)
			free(worker);
		re_destroy(reg);

		return 0;
}