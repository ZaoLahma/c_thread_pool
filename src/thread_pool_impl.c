/*
 * thread_pool_impl.c
 *
 *  Created on: Oct 16, 2016
 *      Author: janne
 */

#include "../inc/thread_pool_impl.h"
#include <stdlib.h>
#include <stdio.h>

void execute_job_impl()
{
	printf("Implement me\n");
}

struct ThreadPool* get_thread_pool()
{
	struct ThreadPool* threadPool = (struct ThreadPool*)malloc(sizeof(struct ThreadPool));

	threadPool->execute_job = &execute_job_impl;

	return threadPool;
}
