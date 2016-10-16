/*
 * thread_pool_if.h
 *
 *  Created on: Oct 16, 2016
 *      Author: janne
 */

#ifndef INC_THREAD_POOL_H_
#define INC_THREAD_POOL_H_

struct ThreadPool {
	void (*execute_job)();
};

#endif /* INC_THREAD_POOL_H_ */
