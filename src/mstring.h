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

#if !defined(MC_NO_MSTR_MACRO) && !defined(mstr)
#   define mstr(str) managed_string(str, strlen(str))
#endif

/**
 * @brief Creates a managed string.
 * @param str Base string to copy.
 * @param len Length of the string.
 * @return Managed pointer to the new string.
 */
ATTRIBUTE(used)
static inline string MC_ADD_PREFIX(managed_string)(const string str, int len)
{
    //+1 for the null term
    string s = MC_ADD_PREFIX(alloc_managed)(sizeof(char), len + 1, NULL);

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
ATTRIBUTE(used)
static inline string MC_ADD_PREFIX(mstrcat)(string dst, string src, int len)
{
    int oldlen = MC_ADD_PREFIX(countof)(dst);
    string new = MC_ADD_PREFIX(realloc_managed)(dst, oldlen + len);
    if (new == NULL)
        return NULL;
    dst = new;
    
    memcpy(dst + (oldlen - 1), src, len);
    dst[len + oldlen] = '\0';

    return dst;
}

/**
 * @brief Copies the @c src string to the @c dst string
 * @param dst
 * @param src
 * @param len
 * @remark This will free the dst string, and return a new, copied, string.
 * @return
 */
ATTRIBUTE(used)
static inline string MC_ADD_PREFIX(mstrcpy)(string dst, string src, int len)
{
    string new = mc_realloc_managed(dst, len);
    if (new == NULL) return NULL;

    memcpy(new, src, len);
    new[len] = '\0';

    return new;
}

ATTRIBUTE(used)
static inline bool MC_ADD_PREFIX(mstrcmp)(string s1, string s2, int len)
{
    bool eq = true;
    size_t c = MC_ADD_PREFIX(countof)(s1);
    for (size_t i = 0; i < c && i < (size_t)len; i++) 
        if (!(eq = s1[i] == s2[i]))
            break;
    return eq;
}

#if defined(MC_PREFIX_STRING)
#   undef string
#endif
