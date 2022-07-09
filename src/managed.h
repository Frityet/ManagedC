/**
 * @file managed.h
 * @brief Implements a garbage collector for pointers.
 * @paragraph LICENCE:
 *                    GNU LESSER GENERAL PUBLIC LICENSE
 *                       Version 2.1, February 1999
 *
 * Copyright (C) 1991, 1999 Free Software Foundation, Inc.
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 *
 *  [This is the first released version of the Lesser GPL.  It also counts
 * as the successor of the GNU Library Public License, version 2, hence
 * the version number 2.1.]
 *
 */

#pragma once
#include <stdlib.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <string.h>

#if !defined(EVAL)
#   define EVAL(x) x
#endif

#if !defined(CONCAT)
#   define X_CONCAT(x, y) x##y
#   define CONCAT(x, y) X_CONCAT(x, y)
#endif

#if !defined(MC_PREFIX)
#   define MC_PREFIX mc_
#endif

#define MC_ADD_PREFIX(x) CONCAT(MC_PREFIX, x)

#if !defined(ATTRIBUTE)
#   define ATTRIBUTE(...) __attribute__((__VA_ARGS__))
#endif

#if !defined(auto) || !defined(MC_NO_AUTO) 
/**
 *  @brief Automatically calls free_managed at the end of the scope.
 */
#   define auto    ATTRIBUTE(cleanup(MC_ADD_PREFIX(free_managed)))
#endif


#if !defined(MC_NO_NULLABLE) && (defined(__clang__) || defined (__llvm__))
#   define nullable         _Nullable
#   define nonnull          _Nonnull
#   define null_unspec      _Null_unspecified
#else
#   define nullable
#   define nonnull
#   define null_unspec
#endif


#if !defined (MC_CMPXCHG)
#   define MC_CMPXCHG(ptr, expect, desired, success, failed) \
        __atomic_compare_exchange_n (ptr, expect, desired, true, success, failed)
#endif

#if !defined (MC_WARN_RES)
#   if defined (__clang__) || defined(__llvm__)
#       define MC_WARN_RES(...) __attribute__((warn_unused_result(__VA_ARGS__)))
#   else
#       define MC_WARN_RES(...) __attribute__((warn_unused_result))
#   endif
#endif

#if !defined(ref) || defined(MC_NO_REF)
/**
 * @brief Gets a reference to a managed pointer, incrementing the pointer's reference count by 1.
 */
#   define ref(obj) (typeof(obj))MC_ADD_PREFIX(reference)(obj)
#endif

/**
 * @brief Callback type for freeing a managed pointer.
 */
typedef void MC_ADD_PREFIX(FreePointer_f)(void *nonnull);

/**
 * @brief Metadata about a managed pointer.
 */
struct MC_ADD_PREFIX(PointerMetadata) {
    /**
     * @brief Amount elements in the data pointer.
     */
    size_t    count;

    /**
     * @brief Size of the type represented in the data pointer.
     */
    size_t    typesize;

    /**
     * @brief Count of references to this pointer.
     */
    size_t    reference_count;

    /**
     * @brief Callback to be executed on each element in the data pointer.
     */
    MC_ADD_PREFIX(FreePointer_f)    *nullable on_free;

    /**
     * @brief Pointer to the allocated data.
     */
    void                            *nonnull data;
};

/**
 * @brief Gets the metadata of a managed pointer.
 * @param ptr Managed pointer
 * @return struct MC_ADD_PREFIX(PointerMetadata)
 * @refitem Metadata of the pointer.
 */
ATTRIBUTE(used)
static inline struct MC_ADD_PREFIX(PointerMetadata) *nullable MC_ADD_PREFIX(metadataof)(void *nonnull ptr)
{
    if (ptr == NULL) return NULL;
    //Offsets the pointer by the size of the metadata, making the new address point right at the start of the metadata
    struct MC_ADD_PREFIX(PointerMetadata) *mdata = (struct MC_ADD_PREFIX(PointerMetadata) *)(ptr - sizeof(struct MC_ADD_PREFIX(PointerMetadata)));

    //High chance of causing a segmentation fault, especially if `mdata` is NULL.
    //Too bad I don't care!
    return mdata->data == ptr ? mdata : NULL;
}

/**
 * @brief Gets the count of items in the managed pointer.
 * @param ptr Managed pointer
 * @return Count of items in the pointer.
 */
ATTRIBUTE(used)
static inline int MC_ADD_PREFIX(countof)(void *nonnull ptr)
{
    struct MC_ADD_PREFIX(PointerMetadata) *mdata = MC_ADD_PREFIX(metadataof)(ptr);
    if (mdata == NULL)
        return 0;

    return (int)mdata->count;
}

/**
 * @brief Gets a reference to the managed pointer, incrementing the reference counter.
 * @param ptr Managed pointer.
 * @return ptr
 */
ATTRIBUTE(used)
static inline void *nullable MC_ADD_PREFIX(reference)(void *nonnull ptr)
{
    struct MC_ADD_PREFIX(PointerMetadata) *mdata = MC_ADD_PREFIX(metadataof)(ptr);
    if (mdata == NULL)
        return NULL;
    
    size_t old;
    do {
        old = __atomic_load_n(&mdata->reference_count, __ATOMIC_RELAXED);
    } while (MC_CMPXCHG(&mdata->reference_count, &old, old + 1, __ATOMIC_SEQ_CST, __ATOMIC_RELAXED) == 0);

    return mdata->data;
}

/**
 * @brief Frees a managed pointer.
 * @param ref Pointer to the managed pointer (double pointer).
 */
ATTRIBUTE(used)
static inline void MC_ADD_PREFIX(free_managed)(const void *nonnull ref)
{
    void *ptr = *((void **)ref);
    struct MC_ADD_PREFIX(PointerMetadata) *mdata = MC_ADD_PREFIX(metadataof)(ptr);

    //TODO: Find better solution?
    if (mdata == NULL)
        return;

    //We are freeing a pointer, so we can remove this reference and check if there is any other references.
    size_t old;
    do {
        old = __atomic_load_n(&mdata->reference_count, __ATOMIC_RELAXED);
    } while (MC_CMPXCHG(&mdata->reference_count, &old, old - 1, __ATOMIC_SEQ_CST, __ATOMIC_RELAXED) == 0);

    if (mdata->reference_count > 0)
        return;

    if (mdata->on_free != NULL) {
        for (size_t i = 0; i < mdata->count; i++) {
            //Call the metadata's "on_free" method for every item in the pointer.
            //Because the pointer is a void *, which has size of 1, we must multiply
            //the index with the size of the type.
            mdata->on_free(ptr + (mdata->typesize * i));
        }
    }

    free(mdata);
}

/**
 * @brief Releases the resources behind a managed pointer.
 * @param ref Managed pointer
 */
ATTRIBUTE(used)
static inline void MC_ADD_PREFIX(release)(const void *nonnull ref)
{ MC_ADD_PREFIX(free_managed)(&ref); }


/**
 * @brief Allocates a managed pointer.
 * @param size Size of the type behind the data pointer.
 * @param count Number of elements in this pointer.
 * @param on_free Callback to be executed on free.
 * @return Managed pointer, or @c NULL if unable to allocate.
 */
ATTRIBUTE(used)
MC_WARN_RES("This function returns a new allocated pointer on success, you must use the return value!")
static inline void *nullable MC_ADD_PREFIX(alloc_managed)(size_t size, size_t count, MC_ADD_PREFIX(FreePointer_f) *nullable on_free)
{
    size_t total_size = count * size;

    //Calloc initalises to 0, so it is best we use it.
    struct MC_ADD_PREFIX(PointerMetadata) *ptr = calloc(1, sizeof(struct MC_ADD_PREFIX(PointerMetadata)) + total_size);
    if (ptr == NULL) {
        return NULL;
    }

    *ptr = (struct MC_ADD_PREFIX(PointerMetadata)){
        .count          = count,
        .typesize       = size,
        .on_free        = on_free,
        .reference_count= 1
    };

    //The address of the actual data is just after the metadata.
    //We add 1 instead of `sizeof(*ptr)` because adding onto a pointer
    //increases its address by the size of the type * the count to add.
    ptr->data = ptr + 1;

    return ptr->data;
}

/**
 * @brief Reallocates a managed pointer.
 * @param ptr Managed pointer
 * @param count New count (multiplied with the managed pointer's @c typesize).
 * @return Pointer to the resized block of memory, or NULL if it could not reallocate.
 * @remarks This function works just as the realloc function, on success, the parametre @c ptr is freed.
 */
ATTRIBUTE(used)
MC_WARN_RES("This function returns the new reallocated pointer on success, you must use the return value!")
static inline void *nullable MC_ADD_PREFIX(realloc_managed)(void *nonnull ptr, size_t count)
{
    struct MC_ADD_PREFIX(PointerMetadata) *mdata = MC_ADD_PREFIX(metadataof)(ptr);
    size_t  size     = mdata->typesize,
            oldcount = mdata->count,
            newsize  = size * count;


    struct MC_ADD_PREFIX(PointerMetadata) *newptr = realloc(mdata, sizeof(struct MC_ADD_PREFIX(PointerMetadata)) + newsize);
    if (newptr == NULL) 
        return NULL;
    

    //The rest of the fields are copied by `realloc`.
    newptr->count   = count;
    newptr->data    = newptr + 1;

     if (oldcount < count)
        memset(newptr->data + (size * oldcount), 0, size * (count - oldcount));

    return newptr->data;
}

ATTRIBUTE(used)
static inline void *nullable MC_ADD_PREFIX(clone_managed)(void *nonnull ptr)
{
    struct MC_ADD_PREFIX(PointerMetadata) *mdata = MC_ADD_PREFIX(metadataof)(ptr);
    void *new = MC_ADD_PREFIX(alloc_managed)(mdata->typesize, mdata->typesize * mdata->count, mdata->on_free);

    memcpy(new, ptr, mdata->typesize * mdata->count);
    return new;
}

ATTRIBUTE(used)
static inline void *nullable MC_ADD_PREFIX(clone_unmanaged)(void *nonnull ptr)
{
    struct MC_ADD_PREFIX(PointerMetadata) *mdata = MC_ADD_PREFIX(metadataof)(ptr);
    void *new = calloc(mdata->typesize, mdata->count);
    if (new == NULL)
        return NULL;

    memcpy(new, ptr, mdata->typesize * mdata->count);

    return new;
}
