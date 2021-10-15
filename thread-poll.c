#include "thread-poll.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void error(const char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

void function(void *i)
{
    pthread_t thr;
    thr = pthread_self();
    printf("id %ld print %d", (long)thr, *(int*)i);
}

void do_job(void * threadpoll)
{
    threadpoll_t *tp = (threadpoll_t *)threadpoll;
    job_t job;

    for(;;) {
        pthread_mutex_lock(&(tp->lock));

        while (tp->jobsnum == 0 && (!tp->shutdown))
            pthread_cond_wait(&(tp->notify), &(tp->lock));

        if (tp->shutdown == shutdown_nowait || 
            (tp->shutdown == shutdown_waitall && tp->jobsnum == 0)) {
            break;
        }
        job.jobfun = tp->job_head->jobfun;
        job.args = tp->job_head->args;
        tp->job_head = tp->job_head->next;
        tp->jobsnum --;

        pthread_mutex_unlock(&(tp->lock));


        (*(job.jobfun))(job.args);
    }
    tp->worker--;

    pthread_mutex_unlock(&(tp->lock));
    pthread_exit(NULL);
    return;
}

threadpoll_t *threadpoll_init (threadpoll_dynamic_t dynamic)
{
    threadpoll_t *tp;
    int i;
    tp = (threadpoll_t*)malloc(sizeof(threadpoll_t));
    if (!tp)
        error("malloc threadpool");

    tp->worker->next = NULL;
    tp->worker->prev = NULL;
    tp->jobsnum = 0;
    tp->workersnum = 0;
    tp->shutdown = 0;
    tp->dynamic = dynamic;

    if (pthread_mutex_init(&(tp->lock), NULL) || pthread_cond_init(&(tp->notify), NULL)) {
        goto err;
    }

    if (dynamic == fix_num) {
        for (i = 0; i < DEFAULT_THREAD_NUM; ++ i) {
            add_worker(tp);
        }
        return tp;
    }
    return tp;

err:
    free(tp);   /* if tp is null, it will reach this step */
    return NULL;
}

void add_worker(threadpoll_t *tp)
{
    worker_t *worker;
    worker = (worker_t *)malloc(sizeof(worker_t));
    if (!worker)
        error("create worker");

    pthread_create(&(worker->thread), NULL, do_job, (void *)tp);

    if (tp->worker == NULL) {
        tp->worker = worker;
        return;
    }

    worker_t *tmp = tp->worker->next;

    if (tmp == NULL) {
        tp->worker->next = worker;
        worker->prev = tp->worker;
        return;
    }
    else {
        tp->worker->next = worker;
        worker->next = tmp;
        tmp->prev = worker;
        worker->prev = tp->worker;
    }
}

int threadpoll_add_job(threadpoll_t *tp, job_t *job)
{
    int err = 0;
    if (tp == NULL || job->jobfun == NULL)
        return threadpoll_invalid;

    if (pthread_mutex_lock(&(tp->lock)) != 0) {
        return threadpoll_lock_failure;
    }

    if (tp->job_head == NULL)
        tp->job_head = job;
    else
        tp->job_head->next = job;

    tp->jobsnum++;

    if (pthread_cond_signal(&(tp->notify)) != 0) {
        err = threadpoll_lock_failure;
    }

    if (pthread_mutex_unlock(&(tp->lock)) != 0) {
        err = threadpoll_lock_failure;
    }
    return err;
}