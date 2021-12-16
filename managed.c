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


void default_deconstructor(ATTRIBUTE(unused) void *ptr)
{
    // free(ptr);
}

void *managed_alloc(size_t size, size_t count, void (*deconstructor)(void *))
{
    //Full size pf the allocation
    size_t realsize = size * count;

    //The realsize is the what the user will use
    //We also need to allocation size for the metadata
    struct managed_pointer *mptr = malloc(realsize + sizeof(struct managed_pointer));
    mptr->size = realsize;
    mptr->count = count;
    mptr->type_size = size;

    if (mptr->deconstructor == NULL) {
        mptr->deconstructor = default_deconstructor;
    } else {
        mptr->deconstructor = deconstructor;
    }

    //Just C things
    mptr->data = mptr + 1;
    return mptr->data;
}

void *resize_managed_alloc(void *alloc, size_t size)
{
    //First, get the metadata inside the allocation
    managed struct managed_pointer *mdata = metadataof(alloc);
    assert(mdata->data == alloc);

    //Then add on the new size, we will use the old size when we copy the memory
    size_t  oldsize = mdata->size,
            newsize = oldsize + size;

    //Allocate the new memory and copy the old data
    void *newptr = managed_alloc(newsize, 1, mdata->deconstructor);
    memcpy(metadataof(newptr)->data, mdata->data, oldsize);

    return newptr;
}

void *increase_managed_array(void *array, int count)
{
    //Get the metadata of the old array, and the metadata of a newly allocated array
    struct managed_pointer  *old_array = metadataof(array),
                            *new_array = metadataof(managed_alloc(old_array->type_size,
                                                                  old_array->count + count,
                                                                  old_array->deconstructor));

    //Copy the data and add the count
    new_array->count += count;
    new_array->data = memcpy(new_array->data, old_array->data, old_array->size);

    //This sometimes segfaults, but it's a memory leak otherwise
    free_managed(old_array->data);
    return new_array->data;
}

void free_managed(void *ptr)
{
    //The cleanup attribute returns a pointer to the variable,
    //therefore it's actually a pointer to a pointer, so we must dereference
    void *realptr = *(void **)ptr;
    struct managed_pointer *mdata = metadataof(realptr);

    if (mdata->data != realptr) {
        //TODO: This is probably a bad idea. Too bad!
        fprintf(stderr, "Error: metadata pointer (%p) does not equal given pointer (%p)! Are you sure this a managed pointer (allocated with alloc_managed)?\n", mdata->data, realptr);
        exit(EXIT_FAILURE);
    }
    
    mdata->deconstructor(ptr);
    free(mdata);
}

char *mstr(const char *string)
{
    //strlen 🤮🤮🤮
    size_t len = strlen(string);
    char *str = new(char, len);
    strncpy(str, string, len);
    return str;
}

char *mstrcpy(char *str)
{
    //This is literally the only difference between this and mstr, but it's faster because we don't call strlen
    size_t len = lengthof(str);
    char *newstr = new(char, len);
    strncpy(newstr, str, len);
    return newstr;
}
