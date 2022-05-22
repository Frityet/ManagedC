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
    if (!TEST_EXPR(mystruct->num == 16, "Expected value to be 16 (got %d!)", mystruct->num))
        exit(EXIT_FAILURE);

    if (!TEST_EXPR(strncmp(mystruct->reference, "Hello", mc_countof(mystruct->reference)) == 0, "Expected \"Hello\", (got %s)", mystruct->reference))
        exit(EXIT_FAILURE);


    mc_release(mystruct->reference);
}

TEST(customdtor)
{
    auto struct MyStruct *obj = mc_alloc_managed(sizeof(*obj), 1, (void *)MyStruct_free);
    obj->num = 16;
    obj->reference = mc_managed_string("Hello", 5);
    return true;
}
