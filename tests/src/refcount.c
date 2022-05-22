/**
 * @file refcount.c
 * @brief Tests reference counting and deallocation
 */

#include "test.h"

struct CustomStruct {
    char *str;
};

struct MyStruct {
    int *intref;
    struct CustomStruct *sref;
};

static void CustomStruct_free(struct CustomStruct *ptr)
{
    mc_release(ptr->str);
}

static void MyStruct_free(struct MyStruct *ptr)
{
    mc_release(ptr->sref);
    mc_release(ptr->intref);
}

static void add_reference(struct MyStruct *s, int val)
{
    auto int *i = mc_alloc_managed(sizeof(int), 1, NULL);
    *i = val;
    s->intref = mc_reference(i);
}

TEST(refcount)
{
    auto struct MyStruct *obj = mc_alloc_managed(sizeof(*obj), 1, (void *)MyStruct_free);

    {
        auto struct CustomStruct *custrct = mc_alloc_managed(sizeof(*custrct), 1, (void *)CustomStruct_free);
        custrct->str = mc_managed_string("Hello!", 6);
        obj->sref = mc_reference(custrct);
        add_reference(obj, 10);
    }

    if (!TEST_EXPR(strncmp(obj->sref->str, "Hello!", mc_countof(obj->sref)) == 0, "Sref str: %s", obj->sref->str))
        return false;
        
    if (!TEST_EXPR(*obj->intref == 10, "Intref: %d", *obj->intref))
        return false;

    return true;
}
