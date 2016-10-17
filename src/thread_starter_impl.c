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

void execute_job_impl(void* (*jobFunc)(void*), void* arg)
{
	//Obviously this is not a thread pool, but it's a start...
	pthread_t thread;
	int threadId = pthread_create(&thread, NULL, jobFunc, arg);
	threadId = pthread_detach(thread);
}

struct ThreadStarter* get_thread_starter(unsigned int type)
{
	struct ThreadStarter* threadPool = (struct ThreadStarter*)malloc(sizeof(struct ThreadStarter));

	switch(type)
	{
	case DETACHED:
		threadPool->execute_job = &execute_job_impl;
		break;
	default:
		threadPool->execute_job = NULL;
		break;
	}

	return threadPool;
}
