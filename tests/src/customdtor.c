/**
 * @file customdtor.c
 * @brief Tests a custom deconstructor for a managed type.
 */

#include "test.h"

#define STRING_TO_TEST "Hello!"

struct MyStruct {
    int num;
    string reference;
};

static void MyStruct_free(struct MyStruct *mystruct)
{
    if (!TEST_EXPR(mystruct->num == 16, "Expected value to be 16 (got %d!)", mystruct->num))
        return;

    if (!TEST_EXPR(strncmp(mystruct->reference, STRING_TO_TEST, mc_countof(mystruct->reference)) == 0, "Expected \"Hello\", (got %s)", mystruct->reference))
        return;


    mc_release(mystruct->reference);
}

TEST(customdtor)
{
    auto struct MyStruct *obj = mc_alloc_managed(sizeof(*obj), 1, (void *)MyStruct_free);
    obj->num = 16;
    obj->reference = mstr(STRING_TO_TEST);
    return true;
}
