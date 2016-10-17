/*
 * c_thread_pool.h
 *
 *  Created on: Oct 16, 2016
 *      Author: janne
 */

#ifndef INC_THREAD_POOL_IMPL_H_
#define INC_THREAD_POOL_IMPL_H_

#include "./thread_pool.h"

struct ThreadList;

//Function implementation
struct ThreadPool* get_thread_pool();
void execute_job_impl(void* (*jobFunc)(void*), void* arg);

#endif /* INC_THREAD_POOL_IMPL_H_ */
