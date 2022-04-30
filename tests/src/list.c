#include "test.h"

TEST(list)
{
    let auto list = new(int, 10);
    debug mdata = metadataof(list);

    for (int i = 0; i < 10; i++) {
        list[i] = i;
    }

    for (int i = 0; i < 10; i++) {
        LOG_DEBUG("%d", i);
    }

    TEST_EXPR(list[9] == 9, "Expected 10! Found %d", list[9]);

    return true;
}
