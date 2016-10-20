/*
 * thread_pool_impl.c
 *
 *  Created on: Oct 16, 2016
 *      Author: janne
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "../inc/thread_starter_impl.h"

//Local variables
static struct QueueItem* queue = NULL;
static pthread_mutex_t mutex;
static struct PoolThreadFunc* threads;
static int num_threads = 0;

//Detached thread impl
void execute_job_detached_thread_impl(void* (*thread_func)(void*), void* arg)
{
	pthread_t thread;
	int threadStatus = pthread_create(&thread, NULL, thread_func, arg);
	threadStatus = pthread_detach(thread);
}

//Thread pool impl
struct QueueItem
{
	void* (*thread_func)(void*);
	void* args;
	struct QueueItem* next;
};

void queue_execution(struct QueueItem* item)
{
	pthread_mutex_lock(&mutex);
	//printf("queue_execution called\n");

	struct QueueItem* lastItem = queue;

	if(lastItem == NULL)
	{
		//printf("First queue item\n");
		queue = item;
		pthread_mutex_unlock(&mutex);
		return;
	}

	while(lastItem->next != NULL)
	{
		//printf("Stepping through queue\n");
		lastItem = lastItem->next;
	}

	lastItem->next = item;
	pthread_mutex_unlock(&mutex);
}

struct QueueItem* pop_queue()
{
	//printf("pop_queue called\n");
	pthread_mutex_lock(&mutex);
	struct QueueItem* retVal = queue;
	if(queue != NULL)
	{
		queue = queue->next;
	}

	pthread_mutex_unlock(&mutex);
	//printf("pop_queue returning: %p\n", retVal);
	return retVal;
}

struct PoolThreadFunc
{
	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int busy;
	struct PoolThreadFunc* next;
};

void* pool_thread_func(void* arg)
{
	//printf("pool_thread_func called\n");
	struct QueueItem* queuePtr = NULL;
	struct PoolThreadFunc* poolThreadFunc = (struct PoolThreadFunc*)arg;
	poolThreadFunc->busy = 1;
	while(1)
	{
		queuePtr = pop_queue();
		while(queuePtr != NULL)
		{
			//printf("Executing queue: %p\n", queuePtr);
			queuePtr->thread_func(queuePtr->args);
			free(queuePtr);
			queuePtr = pop_queue();
		}

		pthread_mutex_lock(&poolThreadFunc->mutex);
		poolThreadFunc->busy = 0;
		//printf("thread_func waiting for cond\n");
		pthread_cond_wait(&poolThreadFunc->cond,
						  &poolThreadFunc->mutex);
		pthread_mutex_unlock(&poolThreadFunc->mutex);
		poolThreadFunc->busy = 1;
	}

	return NULL;
}

void execute_job_thread_pool_impl(void* (*thread_func)(void*), void* arg)
{
	struct QueueItem* queueItem = (struct QueueItem*)malloc(sizeof(struct QueueItem));

	queueItem->thread_func = thread_func;
	queueItem->args = arg;
	queueItem->next = NULL;

	queue_execution(queueItem);

	struct PoolThreadFunc* thread = threads;
	while(thread != NULL)
	{
		//printf("execute_job_thread_pool_impl taking mutex\n");
		if(thread->busy == 0)
		{
			pthread_mutex_lock(&thread->mutex);
			pthread_cond_signal(&thread->cond);
			pthread_mutex_unlock(&thread->mutex);
			return;
		}
		//printf("execute_job_thread_pool_impl leaving mutex\n");
		thread = thread->next;
		//printf("thread: %p\n", thread);

	}
	//printf("execute_job_thread_pool_impl exit\n");

	num_threads++;
	printf("num_threads: %d\n", num_threads);

	thread = (struct PoolThreadFunc*)malloc(sizeof(struct PoolThreadFunc));
	pthread_mutex_init(&thread->mutex, NULL);
	pthread_cond_init(&thread->cond, NULL);
	pthread_create(&thread->thread, NULL, &pool_thread_func, thread);
	if(NULL == threads)
	{
		threads = thread;
	}
	else
	{
		struct PoolThreadFunc* lastElem = threads;
		int numThreads = 1;

		while(lastElem->next != NULL)
		{
			lastElem = lastElem->next;
			numThreads++;
		}
		lastElem->next = thread;
	}
}

//The API
struct ThreadStarter* get_thread_starter(unsigned int type)
{
	struct ThreadStarter* threadStarter = (struct ThreadStarter*)malloc(sizeof(struct ThreadStarter));

	switch(type)
	{
	case DETACHED:
		threadStarter->execute_function = &execute_job_detached_thread_impl;
		break;
	case POOL:
		pthread_mutex_init(&mutex, NULL);
		threadStarter->execute_function = &execute_job_thread_pool_impl;
		break;
	default:
		threadStarter->execute_function = NULL;
		break;
	}

	return threadStarter;
}
