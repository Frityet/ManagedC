#pragma once

#include <string.h>

#include "managed.h"

#define MC_INDEX_LIST(list, i) *list[i]

//Because reallocation breaks references to a pointer, we must declare this as a double pointer
static void **MC_ADD_PREFIX(new_list)(int typesize, int count, MC_ADD_PREFIX(FreePointer_f) *nullable free)
{
    count = count < 2 ? 2 : count;

    void    **listptr = (void **)MC_ADD_PREFIX(alloc_managed)(sizeof(void **), 1, MC_ADD_PREFIX(free_managed)),
            *alloc = MC_ADD_PREFIX(alloc_managed)(typesize, count, free);
    
    *listptr = alloc;

    return listptr;
}

static int MC_ADD_PREFIX(grow_list)(void **list)
{
    struct MC_ADD_PREFIX(PointerMetadata) *mdata = MC_ADD_PREFIX(metadataof)(*list);
    void *new = MC_ADD_PREFIX(realloc_managed)(*list, mdata->count *= 1.5); //Golden ratio is the most efficient for some reason
    if (new == NULL) return 0;

    *list = new;
    return mdata->count;
}

static int MC_ADD_PREFIX(add_to_list)(void **list, void *item)
{
    struct MC_ADD_PREFIX(PointerMetadata) *mdata = MC_ADD_PREFIX(metadataof)(*list);
    if (mdata->count >= mdata->count - 1) {
        if (!MC_ADD_PREFIX(grow_list)(list)) return 0;
    }

    memcpy(*list + (mdata->typesize * mdata->count), item, mdata->typesize);
    mdata->count++;
    return mdata->count;
}

static void MC_ADD_PREFIX(remove_from_list)(void **list, int index)
{
    struct MC_ADD_PREFIX(PointerMetadata) *mdata = MC_ADD_PREFIX(metadataof)(*list);
    memcpy(*list, *list, mdata->typesize * (index > 0 ? index - 1 : index));
    index++;
    memcpy(*list + index - 1, *list + index, mdata->typesize * (mdata->count - index));
}

static void MC_ADD_PREFIX(purge_from_list)(void **list, int count, int indexes[static count])
{
    int newlist[count];
    for (int i = 0; i < count; i++) {

    }

}

