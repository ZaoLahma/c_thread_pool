#include "../inc/thread_pool.h"
#include <stdlib.h>

int main(void)
{
    struct ThreadContext* context = init_thread_pool();

    free(context);

    return 0;
}
