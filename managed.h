/**
 * @file    managed.h
 * @author  Amrit Bhogal (ambhogal01@gmail.com)
 * @brief   Management for allocated pointers, just use the `new(type)` macro and mark your pointer as `managed`
 * @version 1.0 
 * @date    2021-11-22
 * 
 */

#ifndef MC_SERVER_MANAGER_MANAGED_
#define MC_SERVER_MANAGER_MANAGED_

#include <stddef.h>
#include <string.h>

#ifndef ATTRIBUTE
#define ATTRIBUTE(func) __attribute__((func))
#endif

/**
 * @brief Allows for a pointer allocated with managed_alloc be deallocated at the end of the scope
 */
#define managed ATTRIBUTE(cleanup(free_managed))

//May god forgive me for this
#define _NEW_1(type, ...)                                   managed_alloc(sizeof(type), 1, NULL)
#define _NEW_2(type, count)                                 managed_alloc(sizeof(type), count, NULL)
#define _NEW_3(type, count, dtor)                           managed_alloc(sizeof(type), count, dtor)
#define _NEW_REDIRECTOR(n, type, dtor_or_count, func, FUNC, ...)  FUNC
/**
 * @brief           Allocates a managed pointer
 * @param type      Type to allocate
 * @param count     Number of entries to allocate
 * @param dtor      Deconstructor to use for the allocation
 */
#define new(...)                                            _NEW_REDIRECTOR(, ##__VA_ARGS__, _NEW_3(__VA_ARGS__), _NEW_2(__VA_ARGS__), _NEW_1(__VA_ARGS__))

/**
 * @brief Represents a pointer that is automatically deallocated
 */
struct managed_pointer {
    /**
     * @brief   Callback to be run on the managed pointer before deallocation
     */
    void    (*deconstructor)(void *);

    /**
     * @brief   Total size of the allocation
     */
    size_t  size;

    /**
     * @brief   Size of one element in the pointer
     */
    size_t  type_size;

    /**
     * @brief   Count of the elements in the pointer
     */
    size_t  count;

    /**
     * @brief   Allocated data which can be used
     */
    void    *data;
};

/**
 * @brief Gets the metadata of a managed pointer
 * 
 * @param ptr Pointer allocated with @link managed_alloc @endlink
 * @return struct managed_pointer* Metadata of the point
 */
ATTRIBUTE(always_inline)
static inline struct managed_pointer *metadataof(void *ptr)
{
    return ptr - sizeof(struct managed_pointer);
}



/**
 * @brief Allocated a pointer which can be automatically freed at the end of the scope
 * 
 * @param size Size of the allocated memory block
 * @param free Function to run on scope exit
 * @return Allocated memory, or null if failed
 */
void *managed_alloc(size_t size, size_t count, void (*free)(void *));

/**
 * @brief Resizes a managed allocation. Use increase_managed_array to increase the size of an array
 * @param alloc Allocation to resize
 * @param size  New size
 * @return      Resized allocation
 */
void *resize_managed_alloc(void *alloc, size_t size);

/**
 * @brief Increases the size of a managed array
 * @param array Array to modify
 * @param count How much bigger to make it
 * @return      Resized array
 */
void *increase_managed_array(void *array, int count);

/**
 * @brief Frees a managed pointer
 * 
 * @param ptr Pointer (allocated with @link managed_alloc @endlink) to free
 */
void free_managed(void *ptr);

/**
 * @brief Gets the count of elements in the managed pointer
 * @param ptr Managed allocation
 * @return
 */
static inline size_t lengthof(void *ptr)
{
    return metadataof(ptr)->count;
}

/**
 * @brief       Copies the specified managed pointer
 * @param mptr  Copy of the managed pointer
 * @return      Copy of the managed pointer
 */
static inline void *mptrcpy(void *mptr)
{
    return ({
        struct managed_pointer *mdata = metadataof(mptr);
        void *ptr = new(*ptr, mdata->size);
        ptr = memcpy(ptr, mdata->data, mdata->size);
        ptr;
    });
}

/**
 * @brief   Creates a managed string
 * @param   string String to wrap
 * @return  Managed string
 */
char *mstr(const char *string);

/**
 * @brief   Copies a managed string
 * @param   str Managed string to copy
 * @return  Copy of the managed string
 */
char *mstrcpy(char *str);

#endif //MC_SERVER_MANAGER_MANAGED_

