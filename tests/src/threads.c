#include <stdint.h>
#include <stdio.h>

#include "managed.h"
#include "managed/mlist.h"
#include "test.h"

enum {
    THREAD_COUNT = 128
};

static void *add_100_nums(mlist_t(int) *list)
{
    size_t i = 0;
    for (; i < 100; i++)
        ASSERT(mlist_add(list, &i) == 0, "Could not add item to list!");

    mc_free(list);
    return NULL;
}

declaretest(threads)
{
    pthread_t threads[THREAD_COUNT];
    mlist_t(size_t) *numlist = mlist_new(size_t, NULL);
    size_t i = 0;

    for (i = 0; i < THREAD_COUNT; i++)
        ASSERT(pthread_create(&threads[i], NULL, (void *(*)(void *))add_100_nums, mc_ref((void *)numlist)) == 0, "Could not create thread!");
    
    for (i = 0; i < THREAD_COUNT; i++)
        ASSERT(pthread_join(threads[i], NULL) == 0, "Could not join thread!");
    
    ASSERT(mc_countof(numlist) == THREAD_COUNT * 100, "Numbers were not added!");

    mc_free(numlist);
    return success;
}
