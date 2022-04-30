#pragma once

#include <string.h>

#include "managed.h"

typedef char *
#if defined(MC_PREFIX_STRING)
    MC_ADD_PREFIX(string);
#   define string MC_ADD_PREFIX(string)
#else
    string;
#endif

#if !defined(MC_NO_MSTR_MACRO)
#   define mstr(str, ...) managed_string(str, __VA_OPT__(strlen(str)) __VA_ARGS__)
#endif

/**
 * @brief Creates a managed string.
 * @param str Base string to copy.
 * @param len Length of the string.
 * @return Managed pointer to the new string.
 */
static inline string managed_string(const string str, int len)
{
    //+1 for the null term
    string s = MC_ADD_PREFIX(managed_alloc)(len + 1, sizeof(char), NULL);

    strncpy(s, str, len);

    return s;
}

///**
// * @copydoc managed_string(const string, int)
// */
//overloadable static inline string managed_string(const string str)
//{
//    return managed_string(str, strlen(str));
//}

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

///**
// * @copydoc mstrcat(string, string, int)
// */
//overloadable static inline string mstrcat(string dst, string src)
//{
//    let dst_mdata = MC_ADD_PREFIX(metadataof)(dst),
//        src_mdata = MC_ADD_PREFIX(metadataof)(src);
//
//    if (dst_mdata == NULL) {
//        return NULL;
//    }
//
//    return mstrcat(dst, src, src_mdata == NULL ? strlen(src) : src_mdata->count);
//}

/**
 * @brief Copies the @c src string to the @c dst string
 * @param dst
 * @param src
 * @param len
 * @return
 */
static inline string mstrcpy(string dst, string src, int len)
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
