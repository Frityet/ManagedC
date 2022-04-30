#pragma once
#include <stdlib.h>

#pragma clang assume_nonnull begin

//libX11 decides to be a lil bitch and define every symbol (including types and macros) in PascalCase. This results in
//some random ass macros being defined, such as Bool.
#ifdef Bool
#undef Bool
#endif



#ifdef bool
#undef bool
#undef true
#undef false
#endif

/**
 * @brief C11 Bools suck massive balls, so this is competent.
 */
typedef enum Bool: _Bool { true = (_Bool)1, false = (_Bool)0 } bool;


#define ATTRIBUTE(...) __attribute__((__VA_ARGS__))

/**
 *  @breif Automatically calls free_managed at the end of the scope.
 */
#define auto    ATTRIBUTE(cleanup(free_managed))
#define var     __auto_type
#define let     const var

#define nullable        _Nullable
#define nonnull         _Nonnull
#define null_unspec    _Null_unspecified

/**
 * @brief Allows for C++ style function overloading.
 * @warning TODO: Clang specific, replace ASAP.
 */
#define overloadable ATTRIBUTE(overloadable)

/**
 * @brief Wrapper around the "managed_alloc" function that makes it's syntax better.
 * @refitem managed_alloc
 */
#define new(type, ...) (type *)managed_alloc(sizeof(type) __VA_OPT__(,) __VA_ARGS__)

/**
 * @brief Gets a reference to a managed pointer, incrementing the pointer's reference count by 1.
 */
#define ref(obj) (typeof(obj))reference(obj)

/**
 * @brief FUCK YOU CLANG, MY FUNCTION IS USED YOU FUCKING IDIOT, DONT FUCKING ERROR ME OUT FOR YOUR IDOCITY.
 */
#define USED_MOTHERFUCKER ATTRIBUTE(used)
#define debug USED_MOTHERFUCKER let

/**
 * @brief Callback type for freeing a managed pointer.
 */
typedef void FreePointer_f(const void *nonnull);

/**
 * @brief Metadata about a managed pointer.
 */
struct PointerMetadata {
    /**
     * @brief Total size of the allocated data (without the size of the metadata).
     */
    unsigned int    total_size;

    /**
     * @brief Amount elements in the data pointer.
     */
    unsigned int    count;

    /**
     * @brief Size of the type represented in the data pointer.
     */
    unsigned int    typesize;

    /**
     * @brief Count of references to this pointer.
     */
    unsigned int    reference_count;

    /**
     * @brief Callback to be executed on each element in the data pointer.
     */
    FreePointer_f   *nullable on_free;

    /**
     * @brief Pointer to the allocated data.
     */
    void            *nonnull data;
};

/**
 * @brief Gets the metadata of a managed pointer.
 * @param ptr Managed pointer
 * @return struct PointerMetadata
 * @refitem Metadata of the pointer.
 */
static inline struct PointerMetadata *nullable metadataof(void *nonnull ptr)
{
    let mdata = (struct PointerMetadata *)(ptr - sizeof(struct PointerMetadata));

    return mdata->data == ptr ? mdata : NULL;
}

/**
 * @brief Gets the count of items in the managed pointer.
 * @param ptr Managed pointer
 * @return Count of items in the pointer.
 */
static inline int countof(void *nonnull ptr)
{
    let mdata = metadataof(ptr);
    if (mdata == NULL)
        return 0;
    return (int)mdata->count;
}

/**
 * @brief Gets a reference to the managed pointer, incrementing the reference counter.
 * @param ptr Managed pointer.
 * @return ptr
 */
static inline void *nullable reference(void *nonnull ptr)
{
    var mdata = metadataof(ptr);
    if (mdata == NULL)
        return NULL;
    mdata->reference_count++;
    return mdata->data;
}

/**
 * @brief Frees a managed pointer.
 * @param ref Pointer to the managed pointer (double pointer).
 */
USED_MOTHERFUCKER
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

/**
 * @brief Releases the resources behind a managed pointer.
 * @param ref Managed pointer
 */
USED_MOTHERFUCKER
static inline void release(const void *nonnull ref)
{ free_managed(&ref); }

/**
 * @brief Allocates a managed pointer.
 * @param size Size of the type behind the data pointer.
 * @param count Number of elements in this pointer.
 * @param on_free Callback to be executed on free.
 * @return Managed pointer, or @c NULL if unable to allocate.
 */
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

/**
 * @copydoc managed_alloc(unsigned int, unsigned int, FreePointer_f *nullable)
 */
USED_MOTHERFUCKER
overloadable static inline void *nullable managed_alloc(unsigned int size, unsigned int count)
{
    return managed_alloc(size, count, NULL);
}

/**
 * @copydoc managed_alloc(unsigned int, unsigned int, FreePointer_f *nullable)
 */
USED_MOTHERFUCKER
overloadable static inline void *nullable managed_alloc(unsigned int size, FreePointer_f *nullable on_free)
{
    return managed_alloc(size, 1, on_free);
}

/**
 * @copydoc managed_alloc(unsigned int, unsigned int, FreePointer_f *nullable)
 */
USED_MOTHERFUCKER
overloadable static inline void *nullable managed_alloc(unsigned int size)
{
    return managed_alloc(size, 1, NULL);
}

/**
 * @brief Reallocates a managed pointer.
 * @param ptr Managed pointer
 * @param count New count (multiplied with the managed pointer's @c typesize).
 * @return Pointer to the resized block of memory, or NULL if it could not reallocate.
 * @remarks This function works just as the realloc function, on success, the parametre @c ptr is freed.
 */
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
