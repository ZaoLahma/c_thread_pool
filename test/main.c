#include "../inc/thread_pool.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

void* thread_function(void* arg)
{
    printf("Executed in thread. arg: 0x%x\n", *((int*)arg));
    usleep(10);
    return 0;
}

int main(void)
{
    unsigned int maxThreads = 4;
    struct ThreadContext* context = init_thread_pool(maxThreads);

    int test0 = 0;
    int test1 = 1;

    int i = 0;
    for(i = 0; i < 100; ++i)
    {
        sched_job(context, &thread_function, &test0);
        sched_job(context, &thread_function, &test1);
        sched_job(context, &thread_function, &test0);
        sched_job(context, &thread_function, &test1);
        usleep(200);
        sched_job(context, &thread_function, &test0);
        sched_job(context, &thread_function, &test1);
        sched_job(context, &thread_function, &test0);
    }

    usleep(1000);

    destroy_thread_pool(context);

    return 0;
}

