#include "thread-poll.h"
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
    threadpoll_t *tp;

    tp = threadpoll_init(fix_num);

    for (i = 0; i< 10; ++i) {
        int* j = malloc(sizeof(int));
        job_t *job = (job_t *)malloc(sizeof(job_t));
        job->jobfun = function;
        *j = i;
        job->args = j;
        threadpoll_add_job(tp, job);
        // sleep(1);
    }
    // tp->shutdown = shutdown_waitall;
    sleep(3);
    threadpool_destory(tp);
    return 0;
}