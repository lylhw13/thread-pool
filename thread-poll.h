#ifndef THREAD_POLL_H
#define THREAD_POLL_H

#include <pthread.h>

typedef struct job {
    void (*jobfun)(void *);
    void * args;
    struct job *next;
} job_t;

typedef struct worker {
    pthread_t thread;
    struct worker *next;
    struct worker *prev;
} worker_t;

typedef enum {
    fix_num = 0,
    dynamic_num,
} threadpoll_dynamic_t;

typedef enum {
    shutdown_nowait,    /* immediate shutdown */
    shutdown,           /* after the current job done */
    shutdown_waitall,       /* after all jobs done */
} threadpoll_shutdown_t;


typedef struct threadpoll {
    pthread_mutex_t lock;
    pthread_cond_t notify;

    int shutdown;
    int dynamic;

    int workernums;
    worker_t *worker_head;

    int jobsnum;
    job_t *job_head;

} threadpoll_t;


#define MAX_THREAD_NUM 64
#define DEFAULT_THREAD_NUM 8
#define MIN_THREAD_NUM 4

#endif











// add threads dynamicly
// remove threads dynamicly 
// max threads
// min threads
// if less and equal min threads, don't reduce
// if larget and equal max threads, don't add

// threadp
