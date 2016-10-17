/*
 * thread_pool_impl.c
 *
 *  Created on: Oct 16, 2016
 *      Author: janne
 */

#include "../inc/thread_pool_impl.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void execute_job_impl(void* (*jobFunc)(void*), void* arg)
{
	//Obviously this is not a thread pool, but it's a start...
	pthread_t thread;
	int threadId = pthread_create(&thread, NULL, jobFunc, arg);
	threadId = pthread_detach(thread);
}

struct ThreadPool* get_thread_pool()
{
	struct ThreadPool* threadPool = (struct ThreadPool*)malloc(sizeof(struct ThreadPool));

	threadPool->execute_job = &execute_job_impl;

	return threadPool;
}
