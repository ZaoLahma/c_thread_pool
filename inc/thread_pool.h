#ifndef HEADER_THREAD_POOL
#define HEADER_THREAD_POOL

struct ThreadContext;

extern struct ThreadContext* init_thread_pool();
extern void sched_job(struct ThreadContext* context, void* (*thread_func)(void*), void* args);

#endif
