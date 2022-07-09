/**
 * @file mstring.h
 * @brief Implements a "managed" string, meaning that it is garbage collected.
 */

#pragma once

#include <string.h>

#include "managed.h"

#if !defined(MC_NO_MSTR_MACRO) && !defined(mstr)
#   define mstr(str) MC_ADD_PREFIX(managed_string)(str, strlen(str))
#endif

/**
 * @brief Creates a managed string.
 * @param str Base string to copy.
 * @param len Length of the string.
 * @return Managed pointer to the new string.
 */

static inline char *nullable MC_ADD_PREFIX(managed_string)(const char *nonnull str, size_t len)
{
    //+1 for the null term
    char *s = MC_ADD_PREFIX(alloc_managed)(sizeof(char), len + 1, NULL);
    if (s == NULL)
        return NULL;

    memcpy(s, str, len);
    s[len] = '\0';

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

static inline char *MC_ADD_PREFIX(mstrcat)(char **dst, char *src, size_t len)
{
    int oldlen = MC_ADD_PREFIX(countof)(*dst);
    char *new = MC_ADD_PREFIX(realloc_managed)(*dst, oldlen + len + 1);
    if (new == NULL)
        return NULL;

    memcpy(new + oldlen, src, len);
    new[oldlen + len] = '\0';

    *dst = new;
    return *dst;
}

/**
 * @brief Copies the @c src string to the @c dst string
 * @param dst
 * @param src
 * @param len
 * @remark This will free the dst string, and return a new, copied, string.
 * @return
 */

static inline char *MC_ADD_PREFIX(mstrcpy)(char **dst, char *src, size_t len)
{
    char *new = mc_realloc_managed(*dst, len + 1);
    if (new == NULL) return NULL;

    memcpy(new, src, len);
    new[len] = '\0';

    *dst = new;
    return new;
}


static inline bool MC_ADD_PREFIX(mstrcmp)(char *s1, char *s2, size_t len)
{
    bool eq = true;
    size_t c = MC_ADD_PREFIX(countof)(s1);
    for (size_t i = 0; i < c && i < len; i++) {
        eq = s1[i] == s2[i];
        if (!eq) break;
    }
    return eq;
}
