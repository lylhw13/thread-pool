#include "thread-poll.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void error(const char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

void* threadpoll_do_job(void * threadpoll)
{
    // LOGD("%s\n", __FUNCTION__);

    threadpoll_t *tp = (threadpoll_t *)threadpoll;
    job_t job;

    for(;;) {
        // LOGD("for\n");

        if (pthread_mutex_lock(&(tp->job_lock)))
            error("lock\n");

        while ((tp->jobsnum == 0) && (!tp->shutdown))
            pthread_cond_wait(&(tp->notify), &(tp->job_lock));

        if (tp->shutdown == shutdown_immediate || 
            (tp->shutdown == shutdown_waitall && tp->jobsnum == 0)) {
                LOGD("break jobsnum %d\n", tp->jobsnum);
            break;
        }
        job.jobfun = tp->job_head->jobfun;
        job.args = tp->job_head->args;
        tp->job_head = tp->job_head->next;
        tp->jobsnum --;

        pthread_mutex_unlock(&(tp->job_lock));


        (*(job.jobfun))(job.args);
    }
    // remove this pthread

    // LOGD("quit job")
    pthread_mutex_unlock(&(tp->job_lock));
    pthread_exit(NULL);
    return NULL;
}

void threadpoll_add_worker(threadpoll_t *tp)
{
    // LOGD("%s\n", __FUNCTION__);
    pthread_mutex_lock(&(tp->job_lock));

    pthread_mutex_lock(&(tp->worker_lock));
    // LOGD("worker lock\n");

    worker_t *worker;
    worker = (worker_t *)malloc(sizeof(worker_t));
    if (!worker)
        error("create worker\n");

    if (pthread_create(&(worker->thread), NULL, &threadpoll_do_job, (void *)tp) != 0)
        error("pthread create\n");
    

    if (tp->worker == NULL) {
        tp->worker = worker;
    }
    else {
        worker_t *tmp = tp->worker->next;

        if (tmp == NULL) {
            tp->worker->next = worker;
            worker->prev = tp->worker;
        }
        else {
            tp->worker->next = worker;
            worker->next = tmp;
            tmp->prev = worker;
            worker->prev = tp->worker;
        }

    }

    tp->workersnum++;
    LOGD("woker num %d\n", tp->workersnum);

    pthread_mutex_unlock(&(tp->worker_lock));
    pthread_mutex_unlock(&(tp->job_lock));
    // LOGD("worker unlock\n");

}

threadpoll_t *threadpoll_init (threadpoll_dynamic_t dynamic)
{
    // LOGD("%s\n", __FUNCTION__);

    threadpoll_t *tp;
    int i;
    tp = (threadpoll_t*)malloc(sizeof(threadpoll_t));
    if (!tp)
        error("malloc threadpool");

    tp->worker = NULL;
    tp->jobsnum = 0;
    tp->workersnum = 0;
    tp->shutdown = no_shutdown;
    tp->dynamic = dynamic;

    if (pthread_mutex_init(&(tp->worker_lock), NULL) || pthread_mutex_init(&(tp->job_lock), NULL) || pthread_cond_init(&(tp->notify), NULL)) {
        goto err;
    }

    if (dynamic == fix_num) {
        for (i = 0; i < DEFAULT_THREAD_NUM; ++ i) {
            threadpoll_add_worker(tp);
            // sleep(1);
        }
        return tp;
    }
    return tp;

err:
    free(tp);   /* if tp is null, it will reach this step */
    return NULL;
}



int threadpoll_add_job(threadpoll_t *tp, job_t *job)
{
    LOGD("%s\n", __FUNCTION__);
    int err = 0;
    if (tp == NULL || job->jobfun == NULL)
        return threadpoll_invalid;

    if (pthread_mutex_lock(&(tp->job_lock)) != 0) {
        LOGD("lock error");
        return threadpoll_lock_failure;
    }

    if (tp->job_head == NULL)
        tp->job_head = job;
    else {
        job->next = tp->job_head->next;
        tp->job_head->next = job;
    }

    tp->jobsnum++;

    printf("jobsnum %d\n", tp->jobsnum);

    if (pthread_cond_signal(&(tp->notify)) != 0) {
        err = threadpoll_lock_failure;
    }

    if (pthread_mutex_unlock(&(tp->job_lock)) != 0) {
        err = threadpoll_lock_failure;
    }
    return err;
}

void threadpool_destory(threadpoll_t *tp)
{
    pthread_mutex_lock(&(tp->worker_lock));
    tp->shutdown = shutdown_waitall;
    if (pthread_cond_broadcast(&(tp->notify)) != 0)
        error("broadcast error");
    pthread_mutex_unlock(&(tp->worker_lock));

    worker_t *worker_p = tp->worker;
    while (worker_p) {
        pthread_join(worker_p->thread, NULL);
        worker_p = worker_p->next;
    }
    return;
}