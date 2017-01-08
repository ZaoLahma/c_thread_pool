#include "../inc/thread_pool.h"
#include <stdlib.h>
#include <unistd.h>

void* thread_function(void* arg)
{
    printf("Executed in thread\n");
    return 0;
}

int main(void)
{
    struct ThreadContext* context = init_thread_pool();

    sched_job(context, &thread_function, 0);

    usleep(500);

    free(context);

    return 0;
}
