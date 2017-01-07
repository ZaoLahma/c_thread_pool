#include "../inc/thread_pool.h"

#include <pthread.h>
#include <stdlib.h>

struct QueueItem
{
	void* (*thread_func)(void*);
	void* args;
	struct QueueItem* next;
};

struct Thread
{
    pthread_t thread;
    int active;
    struct QueueItem* jobQueue;
    struct Thread* next;
};

struct ThreadContext
{
    pthread_mutex_t context_mutex;
    struct QueueItem* jobQueue;
};

void* thread_func(void* arg)
{

    struct Thread* thread = (struct Thread*)(arg);

    while(thread->active)
    {

    }

    return 0;
}

struct ThreadContext* init_thread_pool()
{
    struct ThreadContext* threadCtxt = (struct ThreadContext*)malloc(sizeof(struct ThreadContext));

    pthread_mutex_init(&threadCtxt->context_mutex, 0);
    threadCtxt->jobQueue = 0;

    return threadCtxt;
}
