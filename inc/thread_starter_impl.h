/*
 * c_thread_pool.h
 *
 *  Created on: Oct 16, 2016
 *      Author: janne
 */

#ifndef INC_THREAD_STARTER_IMPL_H_
#define INC_THREAD_STARTER_IMPL_H_

#include "thread_starter.h"

#define DETACHED 0
#define POOL     1

//Function implementation
struct ThreadStarter* get_thread_starter(unsigned int type);

#endif /* INC_THREAD_STARTER_IMPL_H_ */