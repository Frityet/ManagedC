#pragma once

#include <string.h>

#include "managed.h"

/**
 * @brief Char pointer
 */
typedef char *string;

/**
 * @brief Creates a managed string.
 * @param str Base string to copy.
 * @param len Length of the string.
 * @return Managed pointer to the new string.
 */
overloadable static inline string mstr(const string str, int len)
{
    //+1 for the null term
    string s = managed_alloc(len + 1, sizeof(char));
    let mdata = metadataof(s);

    strncpy(s, str, len);

    return s;
}

/**
 * @copydoc mstr(const string, int)
 */
USED_MOTHERFUCKER
overloadable static inline string mstr(const string str)
{
    return mstr(str, strlen(str));
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
overloadable static inline string mstrcat(string dst, string src, int len)
{
    let dst_mdata = metadataof(dst);
    let oldlen = dst_mdata->count;
    string new = realloc_managed(dst, oldlen + len + 1);
    //Already logged
    if (new == NULL)
        return NULL;

    //-1 because new has a NULL terminator
    strncpy(new + oldlen - 1, src, len);

    return new;
}

/**
 * @copydoc mstrcat(string, string, int)
 */
USED_MOTHERFUCKER
overloadable static inline string mstrcat(string dst, string src)
{
    let dst_mdata = metadataof(dst),
        src_mdata = metadataof(src);

    if (dst_mdata == NULL) {
        return NULL;
    }

    return mstrcat(dst, src, src_mdata == NULL ? strlen(src) : src_mdata->count);
}

/**
 * @brief Copies the @c src string to the @c dst string
 * @param dst
 * @param src
 * @param len
 * @return
 */
overloadable static inline string mstrcpy(string dst, string src, int len)
{
    string new = realloc_managed(dst, len);
    //Already logged
    if (new == NULL)
        return NULL;

    memcpy(new, src, len);

    return new;
}

/**
 * @copydoc mstrcpy(string, string int)
 */
USED_MOTHERFUCKER
overloadable static inline string mstrcpy(string dst, string src)
{
    let dst_mdata = metadataof(dst),
        src_mdata = metadataof(src);

    if (dst_mdata == NULL) {
//        LOG_ERROR("dst (%p) is not a managed string!", dst);
        return NULL;
    }

    return mstrcpy(dst, src, src_mdata == NULL ? strlen(src) : src_mdata->count);
}
