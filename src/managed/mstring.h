#if !defined(MANAGEDC_MSTRING)
#define MANAGEDC_MSTRING

#include <string.h>

#include "managed.h"

#if !defined(MSTRING_CHAR_T) && !defined(mc_char_t)
typedef char mc_char_t;
#endif

typedef mc_char_t mstring;

#define _mcinternal_ptrinfo(ptr) ((struct managed_PointerInfo *)managed_info_of(ptr))

#define mstrlen(str) mc_countof(str)

/**
* Creates metadata for the str pointer, does not copy any bytes
*/
static mstring *mc_nullable managed_string(const mc_char_t *mc_nonnull str, size_t len)
{
    /* This actually could be UB if the string has no null terminator, and strlen(str) is ACTUALLY == len */
	mstring *s = managed_allocate(len + 1, sizeof(mc_char_t), NULL, str);
	if (s == NULL) return NULL;
    s[len] = '\0';

	_mcinternal_ptrinfo(s)->count--; /* Don't count the null terminator */

	return s;
}
static mstring *mc_nullable mstr(const mc_char_t *mc_nonnull str)
{ return managed_string(str, strlen(str)); }


/**
* Duplicates the managed string str
*/
static mstring *mc_nullable managed_string_duplicate(const mstring *mc_nonnull str)
{
	long int length = mstrlen(str);
	if (length == -1) return NULL;
	return managed_string(str, length);
}
static mstring *mc_nullable mstrdup(const mstring *mc_nonnull str)
{ return managed_string_duplicate(str); }

#define mstrcat_unsafe(str1, str2) managed_string_concatenate(str1, str2, mstrlen(str2) < 0 ? strlen(str2) : mstrlen(str2))
#define mstrcat(str1, str2) managed_string_concatenate(str1, str2, strlen(str2))
#define mstrncat(str1, str2, len) managed_string_concatenate(str1, str2, len)
#define mstrmcat(str1, str2) managed_string_concatenate(str1, mstrlen(s2))
/**
* Allocates a new string of needed size, and concatenates the 2 strings into it
*/
static mstring *mc_nullable managed_string_concatenate(mstring *mc_nonnull s1, const mc_char_t *mc_nonnull s2, size_t s2len)
{
	mstring *s = NULL;
	size_t s1len = mstrlen(s1), total;
	total = s1len + s2len;

	s = managed_allocate(total + 1, sizeof(mc_char_t), NULL, NULL);
	if (s == NULL) return NULL;
	MC_MEMCPY(s, s1, s1len); /* It cannot be a param for data because that assumes that sizeof(data) == count * typesize */
	MC_MEMCPY((mstring *)(s + s1len), s2, s2len);
	((mstring *)s)[total] = '\0';

	_mcinternal_ptrinfo(s)->count--; /* Don't count the null terminator */

	return s;
}


/**
* Compares each character of both strings, returning 0 if not equal, and 1 if equal
*/
static int managed_string_equals(mstring *mc_nonnull s1, const mc_char_t *mc_nonnull s2, size_t s2len)
{
	long int s1len = mstrlen(s1), i = 0;
	if (s1len == -1) s1len = (long int)strlen(s1);
	if (s1len != (long int)s2len) return 0;

	for (i = 0; i < s1len; i++)
		if (((mc_char_t *)s1)[i] != ((mc_char_t *)s2)[i]) return 0;

	return 1;
}

static int mstreq_unsafe(mstring *mc_nonnull s1, const mc_char_t *mc_nonnull s2)
{
    long int s1len = mstrlen(s1), s2len = mstrlen(s2), i = 0;
    if (s1len == -1) s1len = (long int)strlen(s1);
    if (s2len == -1) s2len = (long int)strlen(s2);
    if (s1len != s2len) return 0;

    for (i = 0; i < s1len; i++)
        if (((mc_char_t *)s1)[i] != ((mc_char_t *)s2)[i]) return 0;

    return 1;
}

static int mstreq(mstring *mc_nonnull s1, const mc_char_t *mc_nonnull s2, size_t s2len)
{ return managed_string_equals(s1, s2, s2len); }
#define mstreq(str1, str2) mstreq(str1, str2, strlen(str2))

#endif
