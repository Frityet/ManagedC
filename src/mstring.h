/**
 * @file mstring.h
 * @brief Implements a "managed" string, meaning that it is garbage collected.
 */

#pragma once

#include <string.h>

#include "managed.h"

#if !defined(MC_NO_STRING)
    typedef char *
#   if defined(MC_PREFIX_STRING)
       MC_ADD_PREFIX(string);
#      define string MC_ADD_PREFIX(string)
#   else
       string;
#   endif
#endif

#if !defined(MC_NO_MSTR_MACRO)
//#   define mstr(str, ...) managed_string(str, __VA_OPT__(!)true ? strlen(str) : __VA_OPT__(1) __VA_ARGS__)
#endif

/**
 * @brief Creates a managed string.
 * @param str Base string to copy.
 * @param len Length of the string.
 * @return Managed pointer to the new string.
 */
static inline string MC_ADD_PREFIX(managed_string)(const string str, int len)
{
    //+1 for the null term
    string s = MC_ADD_PREFIX(alloc_managed)(len + 1, sizeof(char), NULL);

    strncpy(s, str, len);

    return s;
}

/**
 * @brief Concatenates a string onto a managed string.
 * @param dst Destination
 * @param src Source
 * @param len Length of the source.
 * @return Managed pointer to the new string.
 * @remarks This function does not function as base @c strncat. Instead of writing through the buffer of @c dst, this function
 * allocates a new object, and returns it.
 */
static inline string MC_ADD_PREFIX(mstrcat)(string dst, string src, int len)
{
    int oldlen = MC_ADD_PREFIX(metadataof)(dst)->count;
    string new = MC_ADD_PREFIX(realloc_managed)(dst, oldlen + len + 1);

    if (new == NULL)
        return NULL;

    //-1 because new has a NULL terminator
    strncpy(new + oldlen - 1, src, len);

    return new;
}

/**
 * @brief Copies the @c src string to the @c dst string
 * @param dst
 * @param src
 * @param len
 * @return
 */
static inline string MC_ADD_PREFIX(mstrcpy)(string dst, string src, int len)
{
    string new = MC_ADD_PREFIX(realloc_managed)(dst, len);
    //Already logged
    if (new == NULL)
        return NULL;

    memcpy(new, src, len);

    return new;
}

#if defined(MC_PREFIX_STRING)
#   undef string
#endif
