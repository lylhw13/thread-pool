#include "thread-poll.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void function(void *i)
{
    // LOGD(__FUNCTION__);
    // LOGD("%s\n", __FUNCTION__);
    pthread_t thr;
    thr = pthread_self();
    printf("id %ld print %d\n", (long)thr, *(int*)i);
    // // sleep(random());
    // sleep(0.5);
}

int main()
{
    int i;
    threadpoll_t *tp;

    tp = threadpoll_init(fix_num);

    for (int i = 0; i< 10; ++i) {
        int j = i;
        job_t *job = (job_t *)malloc(sizeof(job_t));
        job->jobfun = function;
        job->args = &j;
        threadpoll_add_job(tp, job);
        sleep(1);
    }
    // tp->shutdown = shutdown_waitall;
    threadpool_destory(tp);
    return 0;
}