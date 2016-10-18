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

static struct QueueItem* queue = NULL;

pthread_mutex_t mutex;
void queue_execution(struct QueueItem* item)
{
	pthread_mutex_lock(&mutex);
	printf("queue_execution called\n");

	struct QueueItem* lastItem = queue;

	if(lastItem == NULL)
	{
		printf("First queue item\n");
		queue = item;
		pthread_mutex_unlock(&mutex);
		return;
	}

	while(lastItem->next != NULL)
	{
		printf("Stepping through queue\n");
		lastItem = lastItem->next;
	}

	lastItem->next = item;
	pthread_mutex_unlock(&mutex);
}

struct QueueItem* pop_queue()
{
	pthread_mutex_lock(&mutex);
	struct QueueItem* retVal = queue;
	queue = queue->next;

	pthread_mutex_unlock(&mutex);
	return retVal;
}

void* pool_thread_func(void* arg)
{
	struct QueueItem* queuePtr = queue;
	while(1)
	{
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

	queueItem = pop_queue();

	if(NULL != queueItem)
	{
		queueItem->thread_func(queueItem->args);
		free(queueItem);
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
