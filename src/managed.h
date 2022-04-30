#pragma once
#include <stdlib.h>

#if defined(__clang__)
#   pragma clang assume_nonnull begin
#endif

#if !defined(EVAL)
#   define EVAL(x) x
#endif

#if !defined(CONCAT)
#   define X_CONCAT(x, y) x##y
#   define CONCAT(x, y) X_CONCAT(x, y)
#endif

#if !defined(MC_NO_BOOLS)
//libX11 decides to be a lil bitch and define every symbol (including types and macros) in PascalCase. This results in
//some random ass macros being defined, such as Bool.
#   if defined (Bool)
#       undef Bool
#   endif

#   if defined (bool)
#       undef bool
#       undef true
#       undef false
#   endif

    /**
     * @brief C11 Bools suck massive balls, so this is competent.
     */
    typedef enum Bool: _Bool { true = (_Bool)1, false = (_Bool)0 } bool;
#else
#   include <stdbool.h>
#endif

#if (defined(var))
#   pragma push_macro("var")
#endif
#define var     __auto_type

#if (defined(let))
#   pragma push_macro("let")
#endif
#define let     const var

#if !defined(MC_PREFIX)
#   define MC_PREFIX mc_
#endif

#define MC_ADD_PREFIX(x) CONCAT(MC_PREFIX, x)

#define ATTRIBUTE(...) __attribute__((__VA_ARGS__))

#if defined(auto)
#   pragma push_macro("auto")
#endif
/**
 *  @brief Automatically calls free_managed at the end of the scope.
 */
#define auto    ATTRIBUTE(cleanup(MC_ADD_PREFIX(free_managed)))

#if !defined(MC_NO_NULLABLE) && defined(__clang__)
#   define nullable         _Nullable
#   define nonnull          _Nonnull
#   define null_unspec      _Null_unspecified
#else
#   define nullable
#   define nonnull
#   define null_unspec
#endif

//#if !defined(new) || defined(MC_NO_NEW)
///**
// * @brief Wrapper around the "managed_alloc" function that makes it's syntax better.
// * @param type Type of the allocation
// * @param ... Additional arguments for managed_alloc
// * @refitem managed_alloc
// */
//#   define new(type, ...) (type *)managed_alloc(sizeof(type) __VA_OPT__(,) __VA_ARGS__)
//#endif

#if !defined(ref) || defined(MC_NO_REF)
/**
 * @brief Gets a reference to a managed pointer, incrementing the pointer's reference count by 1.
 */
#   define ref(obj) (typeof(obj))MC_ADD_PREFIX(reference)(obj)
#endif

/**
 * @brief Callback type for freeing a managed pointer.
 */
typedef void MC_ADD_PREFIX(FreePointer_f)(const void *nonnull);

/**
 * @brief Metadata about a managed pointer.
 */
struct MC_ADD_PREFIX(PointerMetadata) {
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
    MC_ADD_PREFIX(FreePointer_f)   *nullable on_free;

    /**
     * @brief Pointer to the allocated data.
     */
    void            *nonnull data;
};

/**
 * @brief Gets the metadata of a managed pointer.
 * @param ptr Managed pointer
 * @return struct MC_ADD_PREFIX(PointerMetadata)
 * @refitem Metadata of the pointer.
 */
static inline struct MC_ADD_PREFIX(PointerMetadata) *nullable MC_ADD_PREFIX(metadataof)(void *nonnull ptr)
{
    let mdata = (struct MC_ADD_PREFIX(PointerMetadata) *)(ptr - sizeof(struct MC_ADD_PREFIX(PointerMetadata)));

    return mdata->data == ptr ? mdata : NULL;
}

/**
 * @brief Gets the count of items in the managed pointer.
 * @param ptr Managed pointer
 * @return Count of items in the pointer.
 */
static inline int MC_ADD_PREFIX(countof)(void *nonnull ptr)
{
    let mdata = MC_ADD_PREFIX(metadataof)(ptr);
    if (mdata == NULL)
        return 0;
    return (int)mdata->count;
}

/**
 * @brief Gets a reference to the managed pointer, incrementing the reference counter.
 * @param ptr Managed pointer.
 * @return ptr
 */
static inline void *nullable MC_ADD_PREFIX(reference)(void *nonnull ptr)
{
    var mdata = MC_ADD_PREFIX(metadataof)(ptr);
    if (mdata == NULL)
        return NULL;
    mdata->reference_count++;
    return mdata->data;
}

/**
 * @brief Frees a managed pointer.
 * @param ref Pointer to the managed pointer (double pointer).
 */

static inline void MC_ADD_PREFIX(free_managed)(const void *nonnull ref)
{
    void *ptr = *((void **)ref);
    var mdata = MC_ADD_PREFIX(metadataof)(ptr);

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

static inline void MC_ADD_PREFIX(release)(const void *nonnull ref)
{ MC_ADD_PREFIX(free_managed)(&ref); }


/**
 * @brief Allocates a managed pointer.
 * @param size Size of the type behind the data pointer.
 * @param count Number of elements in this pointer.
 * @param on_free Callback to be executed on free.
 * @return Managed pointer, or @c NULL if unable to allocate.
 */
ATTRIBUTE(warn_unused_result("This function returns a new allocated pointer on success, you must use the return value!"))
static inline void *nullable MC_ADD_PREFIX(managed_alloc)(unsigned int size, unsigned int count, MC_ADD_PREFIX(FreePointer_f) *nullable on_free)
{
    let total_size = count * size;

    struct MC_ADD_PREFIX(PointerMetadata) *ptr = calloc(1, sizeof(struct MC_ADD_PREFIX(PointerMetadata)) + total_size);
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

#pragma region Clang specific
///**
// * @copydoc managed_alloc(unsigned int, unsigned int, FreePointer_f *nullable)
// */
//
//overloadable static inline void *nullable managed_alloc(unsigned int size, unsigned int count)
//{
//    return managed_alloc(size, count, NULL);
//}
//
///**
// * @copydoc managed_alloc(unsigned int, unsigned int, FreePointer_f *nullable)
// */
//
//overloadable static inline void *nullable managed_alloc(unsigned int size, FreePointer_f *nullable on_free)
//{
//    return managed_alloc(size, 1, on_free);
//}
//
///**
// * @copydoc managed_alloc(unsigned int, unsigned int, FreePointer_f *nullable)
// */
//
//overloadable static inline void *nullable managed_alloc(unsigned int size)
//{
//    return managed_alloc(size, 1, NULL);
//}
#pragma endregion

/**
 * @brief Reallocates a managed pointer.
 * @param ptr Managed pointer
 * @param count New count (multiplied with the managed pointer's @c typesize).
 * @return Pointer to the resized block of memory, or NULL if it could not reallocate.
 * @remarks This function works just as the realloc function, on success, the parametre @c ptr is freed.
 */
ATTRIBUTE(warn_unused_result("This function returns the new reallocated pointer on success, you must use the return value!"))
static inline void *nullable MC_ADD_PREFIX(realloc_managed)(void *nonnull ptr, unsigned int count)
{
    var mdata = MC_ADD_PREFIX(metadataof)(ptr);
    let size     = mdata->typesize,
        newsize  = size * count;

    struct MC_ADD_PREFIX(PointerMetadata) *newptr = realloc(mdata, sizeof(struct MC_ADD_PREFIX(PointerMetadata)) + newsize);
    if (newptr == NULL) {
//        LOG_ERROR("Could not realloc pointer at %p with size %d (%d * %d)!", ptr, newsize, size, count);
        return NULL;
    }

    newptr->count   = count;
    newptr->data    = newptr + 1;

    return newptr->data;
}

#if defined(MC_NO_AUTO)
#   undef auto
#   pragma pop_macro("auto")
#endif

#if !defined(MC_VAR_KEYWORDS)
#   undef var
#   undef let

#   pragma pop_macro("var")
#   pragma pop_macro("let")
#endif

//#pragma pop_macro("MC_ADD_PREFIX")

#if defined (__clang__)
#   pragma clang assume_nonnull end
#endif
