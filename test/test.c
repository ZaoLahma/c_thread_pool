/*
 * test.c
 *
 *  Created on: Oct 16, 2016
 *      Author: janne
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "../inc/thread_starter.h"
#include "../inc/thread_starter_impl.h"

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

void* thread_func(void* arg)
{
	printf("This is executed in a thread\n");

	unsigned int* testInt = (unsigned int*)arg;
	EXPECT(1, *testInt);

	//free(testInt);

	return NULL;
}

int main(void)
{
	printf("----- DETACHED TEST----- \n");

	struct ThreadStarter* threadStarter = get_thread_starter(DETACHED);

	unsigned int* testInt = (unsigned int*)malloc(sizeof(unsigned int));
	*testInt = 1;
	threadStarter->execute_function(&thread_func, testInt);

	sleep(1);

	free(threadStarter);

	printf("----- POOL TEST----- \n");

	threadStarter = get_thread_starter(POOL);

	threadStarter->execute_function(&thread_func, testInt);
	threadStarter->execute_function(&thread_func, testInt);
	threadStarter->execute_function(&thread_func, testInt);
	threadStarter->execute_function(&thread_func, testInt);
	threadStarter->execute_function(&thread_func, testInt);

	sleep(1);

	free(testInt);
	return 0;
}
