/**
 * @file    managed.h
 * @author  Amrit Bhogal (ambhogal01@gmail.com)
 * @brief   Management for allocated pointers, just use the `new(type)` macro and mark your pointer as `managed`
 * @version 1.0 
 * @date    2021-11-22
 * 
 * @copyright Copyright Amrit Bhogal(c) 2021
 * 
 */

#ifndef _BUTCHER_MANAGED_
#define _BUTCHER_MANAGED_

#include <stddef.h>

#include <utilities.h>



/**
 * @brief Lets a pointer allocated with @link managed_alloc @endlink be deallocated at the end of the scope
 */
#define managed ATTRIBUTE(cleanup(free_managed))

#define string_t managed char *

//May god forgive me for this
#define _NEW_1(type, ...)                                   managed_alloc(sizeof(type), 1, NULL)
#define _NEW_2(type, count)                                 managed_alloc(sizeof(type), count, NULL)
#define _NEW_3(type, count, dtor)                           managed_alloc(sizeof(type), count, dtor)
#define _NEW_REDIRECTOR(n, type, dtor_or_count, func, FUNC, ...)  FUNC
#define new(...)                                            _NEW_REDIRECTOR(, ##__VA_ARGS__, _NEW_3(__VA_ARGS__), _NEW_2(__VA_ARGS__), _NEW_1(__VA_ARGS__))
/**
 * @brief Represents a pointer that is automatically deallocated
 */
struct managed_pointer {
    void    (*deconstructor)(void *);
    size_t  size;
    size_t  count;
    void    *data;
};

/**
 * @brief Gets the metadata of a managed pointer
 * 
 * @param ptr Pointer allocated with @link managed_alloc @endlink
 * @return struct managed_pointer* Metadata of the point
 */
ATTRIBUTE(always_inline)
struct managed_pointer *metadataof(void *ptr);



/**
 * @brief Allocated a pointer which can be automatically freed at the end of the scope
 * 
 * @param size Size of the allocated memory block
 * @param free Function to run on scope exit
 * @return void* Allocated memory, or null if failed
 */
void *managed_alloc(size_t size, size_t count, void (*free)(void *));

void *resize_managed_alloc(void *alloc, size_t size);
void *resize_managed_array(void *array, int count);

/**
 * @brief Frees a managed pointer
 * 
 * @param ptr Pointer (allocated with @link managed_alloc @endlink) to free
 */
void free_managed(void *ptr);

char *mstr(const char *string);

#endif

