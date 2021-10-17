#include "../thread-pool.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void function(void *i)
{
    pthread_t thr;
    thr = pthread_self();
    sleep(random() % 30);
    LOGD("id %ld print %d\n", (long)thr, *(int*)i);
}

void test_dynamic()
{
    int i;
    threadpool_t *tp;
    tp = threadpool_init(0, dynamic_num);
    for (i = 0; i< 10; ++i) {
        int* j = malloc(sizeof(int));
        job_t *job = (job_t *)malloc(sizeof(job_t));
        job->jobfun = &function;
        *j = i;
        job->args = j;
        threadpool_add_job(tp, job);
        if (i % 10 == 0)
            sleep(random() % 30);
    }

    LOGD("sleep\n");
    while(tp->jobsnum > 0)  /* don't involve shutdown flag earlier */
        ;
    threadpool_destory(tp, shutdown_waitall);
    return;
}

void test_fix()
{
    int i;
    threadpool_t *tp;
    int workernum = 0;
    tp = threadpool_init(workernum, fix_num);
    for (i = 0; i< 100; ++i) {
        int* j = malloc(sizeof(int));
        job_t *job = (job_t *)malloc(sizeof(job_t));
        job->jobfun = function;
        *j = i;
        job->args = j;
        threadpool_add_job(tp, job);
        sleep(random() % 3);
        if (i > 1 && i % 10 == 0) {
            threadpool_change_target_workernum(tp, random() % 10);
        }
    }

    LOGD("sleep\n");
    while(tp->jobsnum > 0)
        ;
    threadpool_destory(tp, shutdown_waitall);
    return;
}

int main()
{
    test_dynamic();
    // test_fix();
    return 0;
}