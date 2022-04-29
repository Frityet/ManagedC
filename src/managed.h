#pragma once
#include <stdlib.h>

#pragma clang assume_nonnull begin

#define ATTRIBUTE(...) __attribute__((__VA_ARGS__))

#define auto ATTRIBUTE(cleanup(free_managed))

#define var __auto_type
#define let const var

#define nullable        _Nullable
#define nonnull         _Nonnull
#define null_unspec    _Null_unspecified

#define overloadable ATTRIBUTE(overloadable)

#define new(type, ...) (type *)managed_alloc(sizeof(type) __VA_OPT__(,) __VA_ARGS__)
#define ref(obj) (typeof(obj))reference(obj)

#define USED_MOTHERFUCKER ATTRIBUTE(used)

typedef void FreePointer_f(const void *nonnull);

struct PointerMetadata {
    unsigned int    total_size, count, typesize, reference_count;
    FreePointer_f   *nullable on_free;
    void            *nonnull data;
};

static inline struct PointerMetadata *nullable metadataof(void *nonnull ptr)
{
    let mdata = (struct PointerMetadata *)(ptr - sizeof(struct PointerMetadata));

    return mdata->data == ptr ? mdata : NULL;
}

static inline int lengthof(void *nonnull ptr)
{
    let mdata = metadataof(ptr);
    if (mdata == NULL)
        return 0;
    return (int)mdata->count;
}

static inline void *nullable reference(void *nonnull ptr)
{
    var mdata = metadataof(ptr);
    if (mdata == NULL)
        return NULL;
    mdata->reference_count++;
    return mdata->data;
}

//overloadable static inline void free_managed(...) {}

ATTRIBUTE(used)
//overloadable
static inline void free_managed(const void *nonnull ref)
{
    void *ptr = *((void **)ref);
    var mdata = metadataof(ptr);
    mdata->reference_count--;

    if (mdata->reference_count > 0)
        return;

    if (mdata->on_free != NULL) {
        for (unsigned int i = 0; i < mdata->count; i++) {
            mdata->on_free(ptr + (mdata->typesize * i));
        }
    }
    free(mdata);
}

overloadable static inline void *nullable managed_alloc(unsigned int size, unsigned int count, FreePointer_f *nullable on_free)
{
    let total_size = count * size;

    struct PointerMetadata *ptr = calloc(1, sizeof(struct PointerMetadata) + total_size);
    if (ptr == NULL) {
//        LOG_ERROR("Could not alloc pointer with size %d (%d * %d)!", total_size, size, count);
        return NULL;
    }

    ptr->count = count;
    ptr->typesize = size;
    ptr->total_size = total_size;
    ptr->on_free = on_free;
    ptr->reference_count++;
    ptr->data = ptr + 1;

    return ptr->data;
}

USED_MOTHERFUCKER
overloadable static inline void *nullable managed_alloc(unsigned int size, unsigned int count)
{
    return managed_alloc(count, size, NULL);
}

USED_MOTHERFUCKER
overloadable static inline void *nullable managed_alloc(unsigned int size, FreePointer_f *nullable on_free)
{
    return managed_alloc(1, size, on_free);
}

USED_MOTHERFUCKER
overloadable static inline void *nullable managed_alloc(unsigned int size)
{
    return managed_alloc(1, size, NULL);
}

ATTRIBUTE(warn_unused_result("Use the return value, fucker"), used)
static inline void *nullable realloc_managed(void *nonnull ptr, unsigned int count)
{
    var mdata = metadataof(ptr);
    let size     = mdata->typesize,
        newsize  = size * count;

    struct PointerMetadata *newptr = realloc(mdata, sizeof(struct PointerMetadata) + newsize);
    if (newptr == NULL) {
//        LOG_ERROR("Could not realloc pointer at %p with size %d (%d * %d)!", ptr, newsize, size, count);
        return NULL;
    }

    newptr->count   = count;
    newptr->data    = newptr + 1;

    return newptr->data;
}

#pragma clang assume_nonnull end
