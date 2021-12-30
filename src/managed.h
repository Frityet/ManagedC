/**
 * @file    managed.h
 * @author  Amrit Bhogal (ambhogal01@gmail.com)
 * @brief   Management for allocated pointers, just use the `new(type)` macro and mark your pointer as `managed`
 * @version 1.0
 * @date    2021-12-29
 *
 */
#ifndef MANAGED_C_HEADER_
#define MANAGED_C_HEADER_

#ifdef __cplusplus
    #error Use std::shared_ptr
#endif

#ifdef _MSC_VER
    #error MSVC does not support __attribute__((cleanup))
#endif

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
extern int errno;
#include <string.h>

#ifndef MC_ATTRIBUTE
    #define MC_ATTRIBUTE(_name) __attribute__((_name))
#endif

#ifndef MC_FORCE_INLINE
    #define MC_FORCE_INLINE MC_ATTRIBUTE(always_inline) static inline
#endif

#ifdef MC_NO_INLINE
    #define MC_FORCE_INLINE
#endif

/**
 * @brief Allows for a pointer allocated with managed_alloc be deallocated at the end of the scope
 */
#define managed MC_ATTRIBUTE(cleanup(free_managed_alloc))

//May god forgive me for this
#define _NEW_1(type, ...)                                           managed_alloc(sizeof(type), 1, NULL)
#define _NEW_2(type, count)                                         managed_alloc(sizeof(type), count, NULL)
#define _NEW_3(type, count, dtor)                                   managed_alloc(sizeof(type), count, dtor)
#define _NEW_REDIRECTOR(n, type, dtor_or_count, func, FUNC, ...)    FUNC

/**
 * @brief           Allocates a managed pointer
 * @param type      Type to allocate
 * @param count     Number of entries to allocate
 * @param dtor      Deconstructor to use for the allocation
 */
#define new(...) _NEW_REDIRECTOR(, ##__VA_ARGS__, _NEW_3(__VA_ARGS__), _NEW_2(__VA_ARGS__), _NEW_1(__VA_ARGS__))

struct managed_pointer {
    void    (*free)(void *);
    size_t  total_size, type_size, count;
    void    *data;
};

MC_FORCE_INLINE struct managed_pointer *metadataof(void *ptr)
{
    struct managed_pointer *mptr = ptr - sizeof(struct managed_pointer);
    if (mptr->data != ptr) {
        return NULL;
    }
    return mptr;
}

MC_FORCE_INLINE size_t managed_countof(void *ptr)
{
    return metadataof(ptr)->count;
}

MC_FORCE_INLINE size_t managed_sizeof(void *ptr)
{
    return metadataof(ptr)->total_size;
}

void *managed_alloc(size_t type_size, size_t count, void (*free)(void *))
{
    size_t full_size = type_size * count;

    struct managed_pointer *mptr = malloc(full_size + sizeof(struct managed_pointer));
    if (mptr == NULL)
        return NULL;

    mptr->total_size    = full_size;
    mptr->count         = count;
    mptr->type_size     = type_size;
    mptr->free          = free;
    mptr->data          = mptr + 1;

    return mptr->data;
}

void free_managed_alloc(void *ptr)
{
    void *realptr = *(void **)ptr;
    struct managed_pointer *mptr = metadataof(realptr);

    if (mptr->data != realptr) {
        errno = 1;
        return;
    }

    if (mptr->free != NULL)
        mptr->free(realptr);
    free(mptr);
}

void *increase_managed_alloc_count(void *alloc, size_t count_to_add)
{
    struct managed_pointer *mptr = metadataof(alloc);
}

MC_FORCE_INLINE char *mstr(const char *str)
{
    size_t len = strlen(str);
    char *mstr = new(char, len);
    strncpy(mstr, str, len);
    return mstr;
}

MC_FORCE_INLINE char *mstrcpy(char *mstr)
{
    size_t len = managed_countof(mstr);
    char *newstr = new(char, len);
    strncpy(newstr, mstr, len);
    return newstr;
}

MC_FORCE_INLINE char *mstrcat(char *str1, char *str2)
{

}

//#undef MC_ATTRIBUTE
#undef MC_FORCE_INLINE

#endif //MANAGED_C_HEADER_
