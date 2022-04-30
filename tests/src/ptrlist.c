/**
 * @file ptrlist.c
 * @brief Tests the allocation and deallocation of a list of managed pointers.
 */

#include "test.h"

TEST(ptrlist)
{
    auto string *list = mc_alloc_managed(sizeof(string), 10, mc_free_managed);
    for (int i = 0; i < 10; i++) {
        char buf[16];
        sprintf(buf, "Value: %d", i);
        list[i] = managed_string(buf, 16);
    }

    TEST_EXPR(strncmp(list[5], "Value: 5", mc_countof(list[5])) == 0, "Unexpected value! (Expected \"Value: 5\")");

    return true;
}
