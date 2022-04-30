#include "test.h"

// #include "mstring.h"

struct CustomStruct {
    char *str;
};

struct MyStruct {
    int *intref;
    struct CustomStruct *sref;
};

static void CustomStruct_free(struct CustomStruct *ptr)
{
    release(ptr->str);
}

static void MyStruct_free(struct MyStruct *ptr)
{
    release(ptr->sref);
    release(ptr->intref);
}

static void add_reference(struct MyStruct *s, int val)
{
    auto var i = new(int);
    *i = val;
    s->intref = ref(i);
}

TEST(refcount)
{
    let auto obj = new(struct MyStruct, (FreePointer_f *)MyStruct_free);
    {
        let auto custrct = new(struct CustomStruct, (FreePointer_f *)CustomStruct_free);
        custrct->str = mstr("Hello!");
        obj->sref = ref(custrct);
        add_reference(obj, 10);
    }

    TEST_EXPR(strncmp(obj->sref->str, "Hello!", countof(obj->sref)) == 0, "Sref str: %s", obj->sref->str);
    TEST_EXPR(*obj->intref == 10, "Intref: %d", *obj->intref);

    return true;
}
