#include "test.h"

TEST(strings)
{
    string str = mstr("Hello, World!");
    ASRT_EXPR(mc_countof(str) == 13, "String length is not correct? Length: %zu", mc_countof(str));


    return true;
}
