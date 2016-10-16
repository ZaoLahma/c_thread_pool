/*
 * test.c
 *
 *  Created on: Oct 16, 2016
 *      Author: janne
 */

#include "../inc/thread_pool.h"
#include "../inc/thread_pool_impl.h"

int main(void)
{
	struct ThreadPool* threadPool = get_thread_pool();

	threadPool->execute_job();

	return 0;
}
