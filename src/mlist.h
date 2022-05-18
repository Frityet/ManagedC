#pragma once

#include <string.h>

#include "managed.h"

#define MC_NEW_LIST(type, count, free) (type **)MC_ADD_PREFIX(new_list)(sizeof(type), count, free)
#define MC_GET_ITEM(l, ...) (*(l))[__VA_ARGS__]
#define MC_ADD_ITEM(l, v) ({\
        typeof((**l)) t = v; /*Ensuring the types are correct*/\
        MC_ADD_PREFIX(add_to_list)((void *)l, &t);\
    })

#if defined(__clang__) || defined (__llvm__)
#   pragma clang assume_nonnull begin
#endif

ATTRIBUTE(used)
static inline void MC_ADD_PREFIX(free_list)(void *nonnull *nonnull ptr)
{
    //Because the count of the list pointer is used for the count of used items on the list, we must set this to 0 to avoid a segfault
    //This is unbelievably unsafe, too bad I don't care!

    MC_ADD_PREFIX(free_managed)(ptr);
}

//Because reallocation breaks references to a pointer, we must declare this as a double pointer
ATTRIBUTE(used)
static void *null_unspec *nullable MC_ADD_PREFIX(new_list)(int typesize, int count, MC_ADD_PREFIX(FreePointer_f) *nullable free)
{
    //The count **must** be even for growing to work
    count = count < 2 ? 2 : count;
    
    //We must have a double pointer for thread and reference safe adding and deletion
    void    **listptr = (void **)MC_ADD_PREFIX(alloc_managed)(sizeof(void **), 1, (void *)MC_ADD_PREFIX(free_list)),
            *alloc = MC_ADD_PREFIX(alloc_managed)(typesize, count, free);
    if (listptr == NULL) 
        return NULL;

    //The count on the parent pointer will be used for the used items in the list
    struct MC_ADD_PREFIX(PointerMetadata) *mdata = MC_ADD_PREFIX(metadataof)(listptr);
    mdata->count = 0;
    
    *listptr = alloc;

    return listptr;
}

ATTRIBUTE(used)
static int MC_ADD_PREFIX(grow_list)(void *nonnull *nonnull list)
{
    struct MC_ADD_PREFIX(PointerMetadata) *mdata = MC_ADD_PREFIX(metadataof)(*list);
    void *new = MC_ADD_PREFIX(realloc_managed)(*list, mdata->count *= 1.5); //Golden ratio is the most efficient for some reason
    if (new == NULL) return 0;

    *list = new;
    return mdata->count;
}

ATTRIBUTE(used)
static int MC_ADD_PREFIX(add_to_list)(void *nonnull *nonnull list, void *nonnull item)
{
    struct MC_ADD_PREFIX(PointerMetadata)   *mdata = MC_ADD_PREFIX(metadataof)(*list),
                                            *listmdata = MC_ADD_PREFIX(metadataof)(list);
    if (listmdata->count >= mdata->count - 1) {
        if (!MC_ADD_PREFIX(grow_list)(list)) return 0;
    }

    memcpy(*list + (mdata->typesize * listmdata->count), item, mdata->typesize);

    listmdata->count++;
    return listmdata->count;
}

ATTRIBUTE(used)
static int MC_ADD_PREFIX(remove_from_list)(void *nonnull *nonnull list, int index)
{
    struct MC_ADD_PREFIX(PointerMetadata)   *mdata = MC_ADD_PREFIX(metadataof)(*list),
                                            *listmdata = MC_ADD_PREFIX(metadataof)(list);
    memcpy(*list, *list, mdata->typesize * (index > 0 ? index - 1 : index));
    index++;
    memcpy(*list + index - 1, *list + index, mdata->typesize * (listmdata->count - index));

    listmdata->count--;
    return listmdata->count;
}

ATTRIBUTE(used)
static void *nullable MC_ADD_PREFIX(list_toarray)(void *nonnull *nonnull list)
{
    struct MC_ADD_PREFIX(PointerMetadata)   *mdata = MC_ADD_PREFIX(metadataof)(*list),
                                            *listmdata = MC_ADD_PREFIX(metadataof)(list);

    void *alloc = MC_ADD_PREFIX(alloc_managed)(mdata->typesize, listmdata->count, mdata->on_free);
    if (alloc == NULL)
        return NULL;

    memcpy(alloc, *list, mdata->typesize * listmdata->count); 

    return alloc;
}

#if defined(__clang__) || defined (__llvm__)
#   pragma clang assume_nonnull end
#endif
