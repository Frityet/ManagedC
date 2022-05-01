/**
 * @file customdtor.c
 * @brief Tests a custom deconstructor for a managed type.
 */

#include "test.h"

struct MyStruct {
    int num;
    string reference;
};

static void MyStruct_free(struct MyStruct *mystruct)
{
#ifdef __clang__
    (^bool(void) {
        TEST_EXPR(mystruct->num == 16, "Expected value to be 16 (got %d!)", mystruct->num);
        TEST_EXPR(strncmp(mystruct->reference, "Hello", mc_countof(mystruct->reference)) == 0, "Expected \"Hello\", (got %s)", mystruct->reference);
        return true;
    })();
#endif

    mc_release(mystruct->reference);
}

TEST(customdtor)
{
    auto struct MyStruct *obj = mc_alloc_managed(sizeof(*obj), 1, (void *)MyStruct_free);
    obj->num = 16;
    obj->reference = managed_string("Hello", 5);
    return true;
}
