#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

#include "managed.h"
#include "managed/mlist.h"
#include "test.h"

enum {
    THREAD_COUNT = 256
};

static void *add_100_nums(mlist_t(size_t) *list)
{
    size_t i = 0;
    for (; i < 100; i++)
        ASSERT(mlist_add(list, &i) == 0, "Could not add item to list!");

    mc_free(list);
    return NULL;
}

static void *read_and_add(mlist_t(size_t) *list)
{
    size_t i = 0;
    long int c = mc_countof(list);
    ASSERT(c > 0, "Could not get count!");
    for (i = 0; i < (size_t)c; i++) {
        size_t *numptr = mlist_get(list, i);
        ASSERT(numptr != NULL, "Could not get element of list!");
        i += *numptr;
    }

    mc_free(list);
    return NULL;
}

declaretest(threads)
{
    pthread_t threads[THREAD_COUNT];
    mlist_t(size_t) *numlist = mlist_new(size_t, NULL);
    size_t i = 0;

    for (i = 0; i < THREAD_COUNT / 2; i++)
        ASSERT(pthread_create(&threads[i], NULL, (void *(*)(void *))&add_100_nums, mc_ref((void *)numlist)) == 0, "Could not create thread!");

    for (; i < THREAD_COUNT; i++)
        ASSERT(pthread_create(&threads[i], NULL, (void *(*)(void *))&read_and_add, mc_ref((void *)numlist)) == 0, "Could not create thread!");
    
    for (i = 0; i < THREAD_COUNT; i++)
        ASSERT(pthread_join(threads[i], NULL) == 0, "Could not join thread!");
    
    ASSERT(mc_countof(numlist) == (THREAD_COUNT / 2) * 100, "Numbers were not added!");

    mc_free(numlist);
    return success;
}
