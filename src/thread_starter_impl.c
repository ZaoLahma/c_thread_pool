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

void execute_job_detached_thread_impl(void* (*thread_func)(void*), void* arg)
{
	pthread_t thread;
	int threadStatus = pthread_create(&thread, NULL, thread_func, arg);
	threadStatus = pthread_detach(thread);
}

struct ThreadStarter* get_thread_starter(unsigned int type)
{
	struct ThreadStarter* threadPool = (struct ThreadStarter*)malloc(sizeof(struct ThreadStarter));

	switch(type)
	{
	case DETACHED:
		threadPool->execute_function = &execute_job_detached_thread_impl;
		break;
	default:
		threadPool->execute_function = NULL;
		break;
	}

	return threadPool;
}
