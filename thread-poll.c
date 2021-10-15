#include "thread-poll.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void error(const char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

void do_job(void * argument)
{
    do_job(argument);
}

threadpoll_t *threadpoll_init (threadpoll_dynamic_t dynamic)
{
    threadpoll_t *tp;
    tp = (threadpoll_t*)malloc(sizeof(threadpoll_t));
    if (!tp)
        error("create threadpool");

    tp->worker_head->next = NULL;
    tp->worker_head->prev = NULL;

    if (dynamic == fix_num) {

    }

}

void add_worker(threadpoll_t *tp)
{
    worker_t *worker;
    worker = (worker_t *)malloc(sizeof(worker_t));
    if (!worker)
        error("create worker");
    // pthread_t *thread;
    // thread = (pthread_t *)malloc(sizeof(pthread_t));
    // if (!thread)
    //     error("malloc thread");

    pthread_create(&(worker->thread), NULL, do_job, (void *)tp);

    tp->worker_head->next = worker;
    worker->prev = tp->worker_head;

}