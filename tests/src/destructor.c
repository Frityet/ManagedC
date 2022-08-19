#include "test.h"

struct MyStruct {
    int *array;
    mstring *str;
    struct managed_LinkedList *list;
};

void MyStruct_destroy(struct MyStruct *obj)
{
    mc_free(obj->array);
    mc_free(obj->str);
    mc_free(obj->list);
}

struct MyStruct *MyStruct_create(const char *str, size_t arrsiz, int data[arrsiz])
{
    struct MyStruct *self = mc_new(struct MyStruct, &MyStruct_destroy);
    ASSERT(self != NULL, "Could not allocate self!");

    self->str = mstr(str);
    ASSERT(self->str != NULL, "Could not allocate str!");

    self->array = mc_array(int, arrsiz, NULL);
    ASSERT(self->array != NULL, "Could not allocate array!");
    memcpy(self->array, data, arrsiz);

    self->list = mllist_new(int, NULL);

    return self;
}

declaretest(destructor)
{
    mc_auto struct MyStruct *obj = MyStruct_create("test", 10, (int []){ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });
    

    return success;
}
