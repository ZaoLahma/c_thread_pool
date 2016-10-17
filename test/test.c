/*
 * test.c
 *
 *  Created on: Oct 16, 2016
 *      Author: janne
 */

#include "../inc/thread_pool.h"
#include "../inc/thread_pool_impl.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define EXPECT(this, that) \
if(this != that) \
{\
	printf(#this " (0x%x) != " #that " (0x%x)\n", \
		  (unsigned int)this, \
		  (unsigned int)that);\
}\
else\
{\
	printf(#this " == " #that " validated OK\n");\
}

void* threadFunc(void* arg)
{
	printf("This is executed in a thread\n");

	unsigned int* testInt = (unsigned int*)arg;
	EXPECT(1, *testInt);

	free(testInt);

	return NULL;
}

int main(void)
{
	struct ThreadPool* threadPool = get_thread_pool();

	unsigned int* testInt = (unsigned int*)malloc(sizeof(unsigned int));
	*testInt = 1;
	threadPool->execute_job(&threadFunc, testInt);

	sleep(1);

	free(threadPool);

	return 0;
}
