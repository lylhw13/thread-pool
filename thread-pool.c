#include "thread-pool.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void error(const char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

int tune_num(int target)
{
    if (target > MAX_THREAD_NUM)
        target = MAX_THREAD_NUM;
    if (target < MIN_THREAD_NUM)
        target = MIN_THREAD_NUM;
    return target;
}

void* threadpool_do_job(void * threadpool)
{
    // LOGD("%s\n", __FUNCTION__);
    worker_t *curr, *prev;
    int best_workernum;
    pthread_t thr = pthread_self();
    threadpool_t *tp = (threadpool_t *)threadpool;

begin:
    for(;;) {
        job_t *job;

        if (pthread_mutex_lock(&(tp->job_lock)))
            error("lock\n");

        while ((tp->jobsnum == 0) && (!tp->shutdown))
            pthread_cond_wait(&(tp->notify), &(tp->job_lock));

        pthread_mutex_lock(&(tp->worker_lock));
        if (tp->shutdown == shutdown_immediate || 
            (tp->shutdown == shutdown_waitall && tp->jobsnum == 0)) {
            // LOGD("break jobsnum %d\n", tp->jobsnum);
            break;
        }
        if (tp->dynamic == fix_num) {
            if (tp->target_workernum < tp->workersnum) {
                break;
            }
        }
        else {
            if (tp->last_workerchange + TIME_INTERVAL < time(NULL)) {
                best_workernum = (int)(tp->jobsnum / JOB_WORKER_RATIO);
                if (best_workernum < tp->workersnum && MIN_THREAD_NUM < tp->workersnum) {
                    break;
                }
            }
        }
        printf("jobnum is %d, thread_num is %d\n",tp->jobsnum, tp->workersnum);
        pthread_mutex_unlock(&(tp->worker_lock));
        if (tp->jobsnum == 0)
            continue;


        job = tp->job_head;
        tp->job_head = tp->job_head->next;
        tp->jobsnum --;

        pthread_mutex_unlock(&(tp->job_lock));

        (*((*job).jobfun))((*job).args);

        /* free memory */
        if (job->args)  /* this is shoud be malloc variable */
            free(job->args);
        free(job);
    }

    if (pthread_equal(thr, tp->worker_head->thread) != 0) {
        curr = tp->worker_head;
        tp->worker_head = curr->next;
    }
    else {
        for (prev = tp->worker_head; prev->next != NULL; prev = prev->next) {
            curr = prev->next;
            if (pthread_equal(thr, curr->thread) != 0)
                break;
        }

        if (curr == NULL)
            error("can't locat current thread in the worker list");
        prev->next = curr->next;
    }

    free(curr);
    tp->workersnum--;
    tp->last_workerchange = time(NULL);
    // LOGD("remove a worker, leave %d\n", tp->workersnum),
    pthread_mutex_unlock(&(tp->worker_lock));
    pthread_mutex_unlock(&(tp->job_lock));

    pthread_exit(NULL);
    return NULL;
}

static void threadpool_add_worker_withoutlock(threadpool_t *tp)
{
    // LOGD("%s\n", __FUNCTION__);
    worker_t *worker;
    worker = (worker_t *)malloc(sizeof(worker_t));
    if (!worker)
        error("create worker\n");

    if (pthread_create(&(worker->thread), NULL, &threadpool_do_job, (void *)tp) != 0)
        error("pthread create\n");
    

    if (tp->worker_head == NULL) {
        tp->worker_head = worker;
    }
    else {
        worker->next = tp->worker_head->next;
        tp->worker_head->next = worker;
    }

    tp->workersnum++;
    tp->last_workerchange = time(NULL);
    LOGD("woker num %d, time at %ld\n", tp->workersnum, (long)tp->last_workerchange);
}


static void threadpool_add_worker(threadpool_t *tp)
{
    // LOGD("%s\n", __FUNCTION__);

    pthread_mutex_lock(&(tp->worker_lock));
    threadpool_add_worker_withoutlock(tp);
    pthread_mutex_unlock(&(tp->worker_lock));
}


threadpool_t *threadpool_init (int workernum, threadpool_dynamic_t dynamic)
{
    // LOGD("%s\n", __FUNCTION__);
    threadpool_t *tp;
    int i;
    tp = (threadpool_t*)malloc(sizeof(threadpool_t));
    if (!tp)
        error("malloc threadpool"); 

    tp->worker_head = NULL;
    tp->job_head = NULL;
    tp->job_tail = NULL;
    tp->jobsnum = 0;
    tp->workersnum = 0;
    tp->target_workernum = tune_num(workernum);
    tp->shutdown = no_shutdown;
    tp->dynamic = dynamic;

    if (pthread_mutex_init(&(tp->worker_lock), NULL) || pthread_mutex_init(&(tp->job_lock), NULL) || pthread_cond_init(&(tp->notify), NULL)) {
        goto err;
    }

    if (dynamic == fix_num) {
        for (i = 0; i < tp->target_workernum; ++ i) {
            threadpool_add_worker(tp);
        }
        return tp;
    }
    return tp;

err:
    free(tp);   /* if tp is null, it will reach this step */
    return NULL;
}


int threadpool_add_job(threadpool_t *tp, job_t *job)
{
    // LOGD("%s\n", __FUNCTION__);
    int i;
    int err = 0;
    if (tp == NULL || job->jobfun == NULL)
        return threadpool_invalid;

    if (pthread_mutex_lock(&(tp->job_lock)) != 0) {
        LOGD("lock error");
        return threadpool_lock_failure;
    }

    if (tp->job_head == NULL) {
        tp->job_head = job;
        tp->job_tail = job;
    }
    else {
        tp->job_tail->next = job;
        tp->job_tail = job;
    }

    tp->jobsnum++;

    /* whether need to add worker */
    if (tp->dynamic && tp->last_workerchange + TIME_INTERVAL < time(NULL)) {
        pthread_mutex_lock(&(tp->worker_lock));
        int best_workernum = tune_num((int)(tp->jobsnum / JOB_WORKER_RATIO));
        // if (best_workernum >= MAX_THREAD_NUM)
        //     best_workernum = MAX_THREAD_NUM;

        LOGD("best worker num is %d\n", best_workernum);
        for (i = tp->workersnum; i < best_workernum; ++i) {
            threadpool_add_worker_withoutlock(tp);
        }
        pthread_mutex_unlock(&(tp->worker_lock));
    }
    // printf("jobsnum %d\n", tp->jobsnum);

    if (pthread_cond_signal(&(tp->notify)) != 0) {
        err = threadpool_lock_failure;
    }

    if (pthread_mutex_unlock(&(tp->job_lock)) != 0) {
        err = threadpool_lock_failure;
    }
    return err;
}

void threadpool_destory(threadpool_t *tp, threadpool_shutdown_t shutdown_type)
{
    if (tp == NULL || tp->shutdown)
        return;

    /* lock job_lock */
    if (pthread_mutex_lock(&(tp->job_lock)) != 0) {
        LOGD("thread %ld function %s error %s\n", (long)pthread_self() ,__FUNCTION__, "lock job_lock" );
        error("lock job_lock");
    }

    tp->shutdown = shutdown_type;
    /* wake up all wokers */
    if (pthread_cond_broadcast(&(tp->notify)) != 0)
        error("broadcast error");
    if (pthread_mutex_unlock(&(tp->job_lock)) != 0)
        error("unlcok job_lock");

    while (tp->workersnum) {
        ;
    }
    return;
}

void threadpool_change_target_workernum(threadpool_t *tp, int target)
{
    printf("set target %d\n", target);

    int i;
    if (tp == NULL || tp->shutdown)
        return;

    target = tune_num(target);

    if (pthread_mutex_lock(&(tp->job_lock)) != 0) {
        LOGD("thread %ld function %s error %s\n", (long)pthread_self() ,__FUNCTION__, "lock job_lock" );
        error("lock job_lock");
    }
    if (pthread_mutex_lock(&(tp->worker_lock)) != 0) {
        pthread_mutex_unlock(&(tp->job_lock));
        return;
    }
    tp->target_workernum = target;
    if (tp->target_workernum >= tp->workersnum) {
        while (tp->target_workernum > tp->workersnum ) {
            threadpool_add_worker_withoutlock(tp);
        }
    }
    else {
        /* wakeup */
        if (pthread_cond_broadcast(&(tp->notify)) != 0)
            error("broadcast error");
    }

    pthread_mutex_unlock(&(tp->worker_lock));
    pthread_mutex_unlock(&(tp->job_lock));

    return ;
}