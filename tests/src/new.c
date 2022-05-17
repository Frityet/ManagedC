#include "test.h"

struct MyStruct {
    int num;
    string str;
    string *array;
};

TEST(new)
{

    auto struct MyStruct *s = new(struct MyStruct, {
        .num = 10,
        .str = mc_managed_string("Whoop!", 6),
        .array = mc_alloc_managed(sizeof(string), 10, mc_free_managed)
    });

    puts(s->str);
    puts(s->array[0]);

    return true;
}
