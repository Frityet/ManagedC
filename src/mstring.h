#pragma once

#include <string.h>

#include "managed.h"

typedef char *string;

overloadable static inline string mstr(const string str, int len)
{
    //+1 for the null term
    string s = managed_alloc(len + 1, sizeof(char));

    strncpy(s, str, len);

    return s;
}

USED_MOTHERFUCKER
overloadable static inline string mstr(const string str)
{
    return mstr(str, strlen(str));
}

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

overloadable static inline string mstrcpy(string dst, string src, int len)
{
    string new = realloc_managed(dst, len);
    //Already logged
    if (new == NULL)
        return NULL;

    memcpy(new, src, len);

    return new;
}

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


