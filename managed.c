/**
 * @file    managed.c
 * @author  Amrit Bhogal (ambhogal01@gmail.com)
 * @brief   Implementation of managed pointers
 * @version 1.0
 * @date 2021-11-22
 * 
 * @copyright Copyright Amrit Bhogal(c) 2021
 * 
 */

#include "managed.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "utilities.h"


ATTRIBUTE(always_inline)
struct managed_pointer *metadataof(void *ptr)
{
    return ptr - sizeof(struct managed_pointer);
}

void default_deconstructor(void *ptr)
{
    // free(ptr);
}

void *managed_alloc(size_t size, size_t count, void (*deconstructor)(void *))
{
    size_t realsize = size * count;
    struct managed_pointer *mptr = malloc(realsize + sizeof(struct managed_pointer));
    mptr->size = realsize;
    mptr->count = count;

    if (mptr->deconstructor == NULL) {
        mptr->deconstructor = default_deconstructor;
    } else {
        mptr->deconstructor = free;
    }

    mptr->data = mptr + 1;
    return mptr->data;
}

void *resize_managed_alloc(void *alloc, size_t size)
{
    //First, get the metadata inside the allocation
    struct managed_pointer *mdata = metadataof(alloc);
    assert(mdata->data == alloc);

    //Then add on the new size, we will use the old size when we copy the memory
    int oldsize = mdata->size,
        newsize = oldsize + size;

    //Allocate the new memory and copy the old data
    void *newptr = managed_alloc(newsize, 1, mdata->deconstructor);
    memcpy(newptr, alloc, oldsize);

    //Free the old allocation
    mdata->deconstructor(alloc);
    free(mdata);
    return newptr;
}

void *resize_managed_array(void *array, int count)
{
    int oldcount = metadataof(array)->count;
    void *newalloc = resize_managed_alloc(array, sizeof(array[0]) * count); //fuck your memory safety
    struct managed_pointer *mdata = metadataof(newalloc);
    mdata->count += count + oldcount; 
    return newalloc;
}

void free_managed(void *ptr)
{
    void *realptr = *(void **)ptr;
    struct managed_pointer *mdata = metadataof(realptr);

    if (mdata->data != ptr) {
        fprintf(stderr, "Error: metadata pointer (%p) does not equal given pointer (%p)! Are you sure this a managed pointer (allocated with alloc_managed)?\n", mdata->data, ptr);
    }

    // assert(mdata->data == ptr);
    
    mdata->deconstructor(ptr);
    free(mdata);
}

char *mstr(const char *string)
{
    size_t len = strlen(string);
    char *str = new(char, len);
    strncpy(str, string, len);
    return str;
}
