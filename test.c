#include "thread-pool.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// lock jobslock before wokerlock

void function(void *i)
{
    // LOGD("%s\n", __FUNCTION__);
    pthread_t thr;
    thr = pthread_self();
    printf("id %ld print %d\n", (long)thr, *(int*)i);
    sleep(1);
}

int main()
{
    int i;
    threadpool_t *tp;
    int workernum = 2;

    tp = threadpool_init(workernum, fix_num);

    for (i = 0; i< 10; ++i) {
        int* j = malloc(sizeof(int));
        job_t *job = (job_t *)malloc(sizeof(job_t));
        job->jobfun = function;
        *j = i;
        job->args = j;
        threadpool_add_job(tp, job);
        // sleep(1);
    }
    sleep(2);
    workernum = 4;
    threadpool_change_target_workernum(tp, workernum);
    for (; i< 20; ++i) {
        int* j = malloc(sizeof(int));
        job_t *job = (job_t *)malloc(sizeof(job_t));
        job->jobfun = function;
        *j = i;
        job->args = j;
        threadpool_add_job(tp, job);
        // sleep(1);
    }
    // tp->shutdown = shutdown_waitall;
    sleep(3);
    threadpool_destory(tp, shutdown_waitall);
    return 0;
}