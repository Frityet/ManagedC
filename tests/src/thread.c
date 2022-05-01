/**
 * @file thread.c
 * @brief Tests for race conditions and thread saftey
 */

#include "test.h"

#include <pthread.h>

void *thread_func(int **ref)
{
    //You can get a the reference from the main or this thread
    auto int *ptr = mc_reference(*ref);
    ptr = mc_realloc_managed(ptr, 8);
    TEST_EXPR(ptr != NULL, "Could not reallocate reference!");
    for (int i = 0; i < 8; i++) {
        ptr[i] = 8 - i;
    }

    *ref = ptr;
    return ptr;
}

bool resize_ref(int **ref)
{
    auto int *ptr = mc_reference(*ref);
    ptr = mc_realloc_managed(ptr, 16);
    TEST_EXPR(ptr != NULL, "Could not reallocate reference!");

    *ref = ptr;
    return true;
}

TEST(thread)
{
    auto int *ptr = mc_alloc_managed(sizeof(int), 1, NULL);
    pthread_t thr;
    pthread_create(&thr, NULL, thread_func, &ptr);
    resize_ref(&ptr);

    LOG_DEBUG("Ptr count: %d", mc_countof(ptr));
    pthread_join(thr, NULL);

    LOG_DEBUG("Ptr count: %d", mc_countof(ptr));

    return true;
}
