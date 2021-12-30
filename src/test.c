//
// Created by Frityet on 2021-12-23.
//

#include <stdio.h>

#include "managed.h"

struct test_struct {
    int id;
    void *alloc;
};

void deallocate_test_struct(void *ptr)
{
    struct test_struct *s = (struct test_struct *)ptr;

    free(s->alloc);
}

int main(void)
{
    managed struct test_struct *obj = new(struct test_struct, 1, deallocate_test_struct);

    obj->id = 1;
    obj->alloc = malloc(10);
    
}
