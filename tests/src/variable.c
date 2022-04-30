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
    TEST_EXPR(num != NULL, "Could not allocate! Reason: %s", strerror(errno));
    *num = 4;
    TEST_EXPR(*num == 4, "Could not set value!");

    return true;
}
