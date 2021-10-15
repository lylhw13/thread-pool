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
} threadpoll_dynamic_t; // ratio

typedef enum {
    no_shutdown = 0,
    shutdown_immediate,    /* immediate shutdown */
    shutdown_waitall,       /* after all jobs done */
} threadpoll_shutdown_t;


typedef struct threadpoll {
    pthread_mutex_t worker_lock;
    pthread_mutex_t job_lock;
    pthread_cond_t notify;

    int shutdown;
    int dynamic;

    int workersnum;
    worker_t *worker;

    int jobsnum;
    job_t *job_head;

} threadpoll_t;

typedef enum {
    threadpoll_invalid = -5,
    threadpoll_lock_failure,
} threadpoll_error_t;


#define MAX_THREAD_NUM 64
#define DEFAULT_THREAD_NUM 4
#define MIN_THREAD_NUM 4

threadpoll_t *threadpoll_init(threadpoll_dynamic_t dyanmic);

int threadpoll_add_job(threadpoll_t *tp, job_t *job);
void threadpool_destory(threadpoll_t *tp);

#define LOGD(...) fprintf(stderr, __VA_ARGS__)

#endif











// add threads dynamicly
// remove threads dynamicly 
// max threads
// min threads
// if less and equal min threads, don't reduce
// if larget and equal max threads, don't add

// threadp
