#if !defined(MANAGEDC_MAIN)
#define MANAGEDC_MAIN

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#if defined(_POSIX_VERSION) || defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#   define MC_POSIX 1
#   include <unistd.h>
#   include <pthread.h>
#else
#   define MC_POSIX 0
#endif
// #pragma message("Including Windows.h")

#if defined(WIN32)
#   define MC_WIN32 1
#   include <windows.h>
#else
#   define MC_WIN32 0
#endif

/*Thread support start*/

#if !defined(__STDC_NO_ATOMICS__)
#   include <stdatomic.h>
#   define mc_atomic _Atomic
#else
#   define mc_atomic
#   define MC_ATOMIC_LOAD(var)
#endif

#if !defined(MC_MUTEX_T)
    #if MC_POSIX
        typedef pthread_mutex_t mc_mutex_t;
#       define MC_MUTEX 1
#   elif defined(MC_WIN32)
/* Don't ask, Windows fucking hates typedef for some reason */
        typedef HANDLE mc_mutex_t;
#       define MC_MUTEX 1
#   else
#       warning Mutexes disabled
#       define MC_MUTEX 0
#   endif
    
#endif

/*Thread support end*/

#if !defined(MC_UINTPTR)
    typedef unsigned long int mc_uintptr_t;
#endif

#if !defined(MC_GUARDPAGE_MAX)
#	define MC_GUARDPAGE_MAX ((uintptr_t)0x1000)
#endif

#if !defined(MC_ALLOCATOR)
# 	define MC_ALLOCATOR(c, s) calloc(c, s)
#endif

#if !defined(MC_MEMCPY)
#	define MC_MEMCPY(dst, src, nmemb) memcpy(dst, src, nmemb)
#endif

#if !defined(MC_MEMMOVE)
#	define MC_MEMMOVE(dst, src, nmemb) memmove(dst, src, nmemb)
#endif

#if !defined(MC_FREE)
# 	define MC_FREE(ptr) free(ptr)
#endif

#if !defined(__GNUC__) || defined(__STRICT_ANSI__)
#	define MC_ANSI 1
#else
#	define MC_ANSI 0
#endif

#if defined(__clang__) && defined(__llvm__) && !MC_ANSI
#	define MC_LLVM 1
#else
#	define MC_LLVM 0
#endif

#if MC_ANSI
#	define mc_nullable
# 	define mc_nonnull 
#	define mc_attribute(t)
#	define mc_defer
#	define mc_typeof(T)
# 	define MC_EXPAND(t)
#else
# 	define MC_EXPAND(...) __VA_ARGS__
#	define MC_concat2(x) _mc_##x##_deferepr
#	define MC_concat1(x) MC_concat2(x)

#	define mc_typeof(T) __typeof__(T)
#	define mc_attribute(...) __attribute__((__VA_ARGS__))
#	if MC_LLVM
#		define mc_nullable _Nullable
# 		define mc_nonnull _Nonnull
# 		define mc_nocapture __block
# 		define mc_defer mc_attribute(cleanup(_mc_rundefer   )) void (^mc_nonnull MC_concat1(__LINE__))(void) = ^ 
        static void _mc_rundefer(void (^mc_nonnull *mc_nonnull cb)(void)) { (*cb)(); }

#	else
#		define mc_nullable 	
# 		define mc_nonnull 	
#		define mc_defer
#		define mc_nocapture
#	endif
#endif

#if MC_ANSI
#	define mc_auto Running in ANSI standard mode (no extensions). This macro does not automatically release the pointer!
#else 
    static void managed_release(const void *mc_nonnull ptr);
    static void managed_release_ptr(void *mc_nonnull addr)
    {
        void **ptr = addr;
        if (*ptr)
        {
            managed_release(*ptr);
            *ptr = NULL;
        }
    }

#	define mc_auto mc_attribute(cleanup(managed_release_ptr))
#endif

#define _mcinternal_ptrinfo(ptr) ((struct managed_PointerInfo *)managed_info_of(ptr))

typedef void managed_Free_f(void *mc_nonnull alloc);

struct managed_PointerInfo {
    /**
     * count: Number of used array elements.
     * capacity: Number of allocated array elements. (Used for managed_Vector)
     * typesize: Size of the type.
     * reference_count: Number of references to this pointer.
     */
    mc_atomic size_t count, capacity, typesize, reference_count;

    /**
    * Function to call on 0 references.
    */
    managed_Free_f *mc_nullable free;

    /**
    * Pointer to the data, should be just in front of data.
    */
    void *mc_nonnull data;

#if MC_MUTEX
    /**
    * 
    */
    mc_mutex_t lock;
#endif
};

/**
* Gets the metadata about the pointer
*/
static const struct managed_PointerInfo *mc_nullable managed_info_of(const void *mc_nonnull ptr)
{
    struct managed_PointerInfo *info = NULL;

    /*Check if the pointer is lesser than the max page, which *probably* means that it is not on the heap*/
    if ((uintptr_t)ptr < MC_GUARDPAGE_MAX) return NULL;
    info = (struct managed_PointerInfo *)ptr - 1;
    if (info->data != ptr)
        return NULL;

    return info;
}

static long int mc_countof(const void *mc_nonnull ptr)
{
    const struct managed_PointerInfo *info = managed_info_of(ptr);
    if (info == NULL) return -1;
    else return (long int)info->count;
}

static long int mc_sizeof_type(const void *mc_nonnull ptr)
{
    const struct managed_PointerInfo *info = managed_info_of(ptr);
    if (info == NULL) return -1;
    else return (long int)info->typesize;
}

static long int mc_sizeof(const void *mc_nonnull ptr)
{
    long int c = mc_countof(ptr);
    if (c == -1) return -1;
    return (long int)c * mc_sizeof_type(ptr);
}

#define mc_new(T, free) (T *)managed_allocate(1, sizeof(T), (managed_Free_f *)(free), NULL)
#define mc_array(T, count, free) (T *)managed_allocate(count, sizeof(T), (managed_Free_f *)(free), NULL)
/**
* Creates a new allocation with the managed pointer metadata. Define MC_ALLOCATOR(c, nmemb) to change the allocation function
*/
static void *mc_nullable managed_allocate(size_t count, size_t typesize, managed_Free_f *mc_nullable free, const void *mc_nullable data)
{
    struct managed_PointerInfo *info = MC_ALLOCATOR(1, sizeof(struct managed_PointerInfo) + count * typesize);
    if (info == NULL) return NULL;
    
    info->capacity = info->count= count;
    info->typesize 			   	= typesize;
    info->free 				    = free;
    info->reference_count 	    = 1;
    /* The data must be right after the metadata */
    info->data 				    = info + 1;

    if (data != NULL)
        MC_MEMCPY(info->data, data, count * typesize);

    return info->data;
}


/**
* Creates a copy of the managed pointer. This copies every byte of data in the allocation
*/
static void *mc_nullable managed_copy(const void *mc_nonnull ptr, long int count)
{
    struct managed_PointerInfo *info = (void *)managed_info_of(ptr);
    void *alloc = NULL;
    if (info == NULL) return NULL;
    if (count < 1) count = (long int)info->count;
    alloc = managed_allocate((size_t)count, info->typesize, info->free,NULL);
    if (alloc == NULL) return NULL;

    /* Just in case count is larger than mc_countof(ptr) (sizing up an allocation), make sure you only copy the existing data */
    MC_MEMCPY(alloc, ptr, (size_t)(mc_sizeof_type(ptr) * mc_countof(ptr)));

    return alloc;
}

static void *mc_nullable mc_dup(const void *mc_nonnull ptr)
{ return managed_copy(ptr, mc_countof(ptr)); }

#define mc_ref(ptr) MC_EXPAND((mc_typeof(ptr)))managed_reference(ptr)
/**
* Gets a reference to the ptr, and incrememnts it's reference count
*/
static void *mc_nullable managed_reference(const void *mc_nonnull ptr)
{
    struct managed_PointerInfo *info = (void *)managed_info_of(ptr);
    if (info == NULL) return NULL;
    info->reference_count++;
    return (void *)ptr;
}

/**
* Releases a reference to the pointer, and if 0 references, frees the pointer
*/
static void managed_release(const void *mc_nonnull ptr)
{
    struct managed_PointerInfo *info = NULL;
    size_t i = 0;

    if (ptr == NULL) return;
    info = (void *)managed_info_of(ptr);
    if (info == NULL) return;

    info->reference_count--;
    if (info->reference_count < 1) {
        if (info->free != NULL)
            for (i = 0; i < info->count; i++) /* Free each item of the allocation individually */
                info->free(((unsigned char *)ptr) + i * info->typesize);

        MC_FREE(info);
        /* If we don't free the ptr, unlock, else we would invoke UB */
    }
}
static void mc_free(const void *mc_nonnull ptr)
{ managed_release(ptr); }

/**
* Creates a new, non-managed, allocation and copies all the data to it
*/
static void *mc_nullable managed_to_unmanaged(const void *mc_nonnull ptr)
{
    struct managed_PointerInfo *info = _mcinternal_ptrinfo(ptr);
    void *unmanaged = MC_ALLOCATOR(info->count + 1, info->typesize); /* +1 just in case it's a string */
    if (unmanaged == NULL) return NULL;

    MC_MEMCPY(unmanaged, ptr, info->count * info->typesize);
    return unmanaged;
}

#undef _mcinternal_ptrinfo
#undef MC_SOURCE_PRIVATE

#endif
