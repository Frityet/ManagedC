#include "test.h"

struct MyStruct {
    int num;
    string reference;
};

static void MyStruct_free(struct MyStruct *mystruct)
{
    (^bool(void) {
        TEST_EXPR(mystruct->num == 16, "Expected value to be 16 (got %d!)", mystruct->num);
        TEST_EXPR(strncmp(mystruct->reference, "Hello", countof(mystruct->reference)) == 0, "Expected \"Hello\", (got %s)", mystruct->reference);
        return true;
    })();

    release(mystruct->reference);
}

TEST(customdtor)
{
    auto var obj = new(struct MyStruct, (FreePointer_f *)MyStruct_free);
    obj->num = 16;
    obj->reference = mstr("Hello");
    return true;
}
