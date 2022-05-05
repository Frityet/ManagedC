#include "test.h"

#include <math.h>

typedef struct Type1 Type1_t;
typedef struct Type2 Type2_t;

struct Type1 {
    Type2_t *reference;
    int     i;
};

struct Type2 {
    Type1_t *reference;
    char    c;
};

static void release_type1(Type1_t *ref)
{
    LOG_INFO("I: %d, C: %c", ref->i, ref->reference->c);
    mc_release(ref->reference);
}

static void release_type2(Type2_t *ref)
{
    LOG_INFO("C: %c, I: %d", ref->c, ref->reference->i);
    mc_release(ref->reference);
}

TEST(circularreference)
{
    auto Type1_t *obj1 = mc_alloc_managed(sizeof(*obj1), 1, (void *)release_type1);
    obj1->i = 100;
    {
        auto Type2_t *obj2 = mc_alloc_managed(sizeof(*obj2), 1, (void *)release_type2);
        obj2->c = 'X';
        obj1->reference = mc_reference(obj2);
        obj2->reference = mc_reference(obj1);
    }

    TEST_EXPR(obj1->i == 100, "Reference 1 value is unexpected ('%d' instead '%d')!", obj1->i, 100);
    TEST_EXPR(obj1->reference->c == 'X', "Reference 2 value is unexpected ('%c' instead '%c')!", obj1->reference->c, 'X');

    return true;
}
