#include "../inc/thread_pool.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void* thread_function(void* arg)
{
    printf("Executed in thread. arg: 0x%x\n", *((int*)arg));
    usleep(10);
    return 0;
}

int main(void)
{
    struct ThreadContext* context = init_thread_pool();

    int test0 = 0;
    int test1 = 1;

    sched_job(context, &thread_function, &test0);
    sched_job(context, &thread_function, &test1);
    sched_job(context, &thread_function, &test0);
    sched_job(context, &thread_function, &test1);
    usleep(150);
    sched_job(context, &thread_function, &test0);
    sched_job(context, &thread_function, &test1);
    sched_job(context, &thread_function, &test0);

    usleep(500);

    destroy_thread_pool(context);

    return 0;
}
