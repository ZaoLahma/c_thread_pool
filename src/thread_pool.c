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
    pthread_mutex_t threadMutex;
    pthread_cond_t execCondition;
    int active;
    int busy;
    int threadId;
    struct ThreadContext* threadContext;
    struct Thread* next;
};

struct ThreadContext
{
    pthread_mutex_t contextMutex;
    pthread_mutex_t jobQueueMutex;
    struct Thread* threads;
    int numThreads;
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
    struct Thread* thread = (struct Thread*)(arg);

    printf("Thread %d started\n", thread->threadId);

    while(thread->active)
    {
        struct QueueItem* item = 0;

        while((item = pop_queue_item(thread->threadContext)) != 0)
        {
            printf("Thread: %d handling job\n", thread->threadId);
            item->thread_func(item->args);
            free(item);
        }

        pthread_mutex_lock(&thread->threadMutex);
        thread->busy = 0;
        pthread_cond_wait(&thread->execCondition, &thread->threadMutex);
        thread->busy = 1;
        pthread_mutex_unlock(&thread->threadMutex);
    }

    return 0;
}

struct ThreadContext* init_thread_pool()
{
    struct ThreadContext* threadCtxt = (struct ThreadContext*)malloc(sizeof(struct ThreadContext));

    pthread_mutex_init(&threadCtxt->contextMutex, 0);
    pthread_mutex_init(&threadCtxt->jobQueueMutex, 0);
    threadCtxt->jobQueue = 0;
    threadCtxt->numThreads = 0;
    threadCtxt->threads = 0;

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
    pthread_mutex_init(&lastThread->threadMutex, 0);
    pthread_cond_init(&lastThread->execCondition, 0);
    lastThread->active = 1;
    lastThread->threadId = context->numThreads;
    lastThread->busy = 1;
    lastThread->next = 0;
    lastThread->threadContext = context;
    lastThread->thread = pthread_create(&lastThread->thread, 0, &pool_thread_func, lastThread);
    context->numThreads++;
    printf("Created new thread. threadId: %d\n", lastThread->threadId);
}

void sched_job(struct ThreadContext* context, void* (*thread_func)(void*), void* args)
{
    pthread_mutex_lock(&context->contextMutex);

    struct QueueItem* item = (struct QueueItem*)malloc(sizeof(struct QueueItem));
    item->thread_func = thread_func;
    item->args = args;
    item->next = 0;

    push_queue_item(context, item);

    struct Thread* thread = context->threads;

    while(thread != 0)
    {
        pthread_mutex_lock(&thread->threadMutex);
        if(thread->busy == 0)
        {
            printf("Notifying available thread\n");
            pthread_cond_signal(&thread->execCondition);
            pthread_mutex_unlock(&thread->threadMutex);
            pthread_mutex_unlock(&context->contextMutex);
            return;
        }

        pthread_mutex_unlock(&thread->threadMutex);

        thread = thread->next;
    }

    create_thread(context);

    pthread_mutex_unlock(&context->contextMutex);
}
