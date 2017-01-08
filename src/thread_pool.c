#include "../inc/thread_pool.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

struct QueueItem
{
	void* (*thread_func)(void*);
	void* args;
	struct QueueItem* next;
};

struct ThreadContext;

struct Thread
{
    pthread_t thread;
    int active;
    int busy;
    struct ThreadContext* threadContext;
    struct Thread* next;
};

struct ThreadContext
{
    pthread_mutex_t contextMutex;
    pthread_mutex_t jobQueueMutex;
    struct Thread* threads;
    struct QueueItem* jobQueue;
};

static void push_queue_item(struct ThreadContext* context, struct QueueItem* item)
{
    pthread_mutex_lock(&context->jobQueueMutex);
    struct QueueItem* lastItem = context->jobQueue;

    if(lastItem == 0)
    {
        context->jobQueue = item;
    }
    else
    {
        while(lastItem->next != 0)
        {
            lastItem = lastItem->next;
        }

        lastItem->next = item;
    }

    pthread_mutex_unlock(&context->jobQueueMutex);
}

static struct QueueItem* pop_queue_item(struct ThreadContext* context)
{
    pthread_mutex_lock(&context->jobQueueMutex);

    struct QueueItem* retVal = context->jobQueue;
    if(context->jobQueue != 0)
    {
        context->jobQueue = context->jobQueue->next;
    }

    pthread_mutex_unlock(&context->jobQueueMutex);

    return retVal;
}

void* pool_thread_func(void* arg)
{

    printf("thread_pool_func called\n");

    struct Thread* thread = (struct Thread*)(arg);

    while(thread->active)
    {
        thread->busy = 1;
        struct QueueItem* item = 0;

        while((item = pop_queue_item(thread->threadContext)) != 0)
        {
            item->thread_func(item->args);
            free(item);
        }
    }

    return 0;
}

struct ThreadContext* init_thread_pool()
{
    struct ThreadContext* threadCtxt = (struct ThreadContext*)malloc(sizeof(struct ThreadContext));

    pthread_mutex_init(&threadCtxt->contextMutex, 0);
    pthread_mutex_init(&threadCtxt->jobQueueMutex, 0);
    threadCtxt->jobQueue = 0;

    return threadCtxt;
}

static void create_thread(struct ThreadContext* context)
{
    struct Thread* lastThread = context->threads;

    if(lastThread == 0)
    {
        context->threads = (struct Thread*)malloc(sizeof(struct Thread));
        lastThread = context->threads;
    }
    else
    {
        while(lastThread->next != 0)
        {
            lastThread = lastThread->next;
        }
        lastThread->next = (struct Thread*)malloc(sizeof(struct Thread));
        lastThread = lastThread->next;
    }
    lastThread->active = 1;
    lastThread->busy = 0;
    lastThread->next = 0;
    lastThread->threadContext = context;
    lastThread->thread = pthread_create(&context->threads->thread, 0, &pool_thread_func, context->threads);
}

void sched_job(struct ThreadContext* context, void* (*thread_func)(void*), void* args)
{
    pthread_mutex_lock(&context->contextMutex);

    struct QueueItem* item = (struct QueueItem*)malloc(sizeof(struct QueueItem));
    item->thread_func = thread_func;
    item->args = args;
    item->next = 0;

    push_queue_item(context, item);

    if(context->threads == 0)
    {
        create_thread(context);
    }

    struct Thread* thread = context->threads;

    while(thread != 0)
    {
        if(thread->busy == 0)
        {
            printf("Notifying available thread\n");
            return;
        }

        thread = thread->next;
    }

    pthread_mutex_unlock(&context->contextMutex);
}
