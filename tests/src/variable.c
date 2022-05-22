/**
 * @file variable.c
 * @brief Tests a basic allocation and deallocation.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
extern int errno;

#include "managed.h"
#include "test.h"

TEST(variable)
{
    auto int *num = mc_alloc_managed(sizeof(int), 1, NULL);
    if (!TEST_EXPR(num != NULL, "Could not allocate! Reason: %s", strerror(errno)))
        return false;
    *num = 4;
    if (!TEST_EXPR(*num == 4, "Could not set value!, error: %s", strerror(errno)))
        return false;

    return true;
}
