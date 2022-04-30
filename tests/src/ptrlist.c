#include "test.h"

TEST(ptrlist)
{
    let auto list = new(string, 10, free_managed);
    for (int i = 0; i < 10; i++) {
        char buf[16];
        sprintf(buf, "Value: %d", i);
        list[i] = mstr(buf);
    }

    TEST_EXPR(strncmp(list[5], "Value: 5", countof(list[5])) == 0, "Unexpected value! (Expected \"Value: 5\")");

    return true;
}
