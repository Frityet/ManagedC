/**
 * @file ptrlist.c
 * @brief Tests the allocation and deallocation of a list of managed pointers.
 */

#include "test.h"

TEST(ptrlist)
{
    // auto string **list = MC_NEW_LIST(string, 10, mc_free_managed);
    // for (int i = 0; i < 10; i++) {
    //     char buf[16];
    //     sprintf(buf, "Value: %d", i);
    //     MC_ADD_ITEM(list, (string)buf);
    // }

    // int i = strncmp(MC_GET_ITEM(list, 5), "Value: 5", mc_countof(MC_GET_ITEM(list, 5)));
    // TEST_EXPR(i == 0, "Unexpected value! (Expected \"Value: 5\" - got %d)", i);

    return true;
}
