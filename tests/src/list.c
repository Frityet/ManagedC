/**
 * @file list.c
 * @brief Tests allocation and reallocation (growing) of a list of integers.
 */

#include "test.h"

TEST(list)
{
    auto int *list = mc_alloc_managed(sizeof(*list), 10, NULL);

    for (int i = 0; i < 10; i++) {
        list[i] = i;
    }

    int *new = mc_realloc_managed(list, 16);
    TEST_EXPR(new != NULL, "Could not reallocate! Reason: %s", strerror(errno));
    list = new;

    for (int i = 10; i < 16; i++) {
        list[i] = i;
    }

    TEST_EXPR(list[15] == 15, "Expected 15! Found %d", list[15]);

    return true;
}
