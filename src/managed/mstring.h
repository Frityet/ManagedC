#if !defined(MANAGEDC_MSTRING)
#define MANAGEDC_MSTRING

#include <string.h>

#include "managed.h"

#if !defined(MSTRING_CHAR_T) && !defined(mc_char_t)
#define MSTRING_CHAR_T char
typedef MSTRING_CHAR_T mc_char_t;
#endif

typedef mc_char_t mstring_t;

#define _mcinternal_ptrinfo(ptr) ((struct managed_PointerInfo *)managed_info_of(ptr))
#if defined(MC_MUTEX)
#	define _mcinternal_lock(ptr) 	(MC_MUTEX_LOCK(&_mcinternal_ptrinfo(ptr)->lock))
#	define _mcinternal_unlock(ptr) 	(MC_MUTEX_UNLOCK(&_mcinternal_ptrinfo(ptr)->lock))
#else
#	define _mcinternal_lock(ptr) 	
#	define _mcinternal_unlock(ptr) 	
#endif


#define mstrlen(str) mc_countof(str)

/**
* Creates metadata for the str pointer, does not copy any bytes
*/
static mstring_t *mc_nullable managed_string(const mc_char_t *mc_nonnull str, size_t len)
{
	mstring_t *s = managed_allocate(len + 1, sizeof(mc_char_t), NULL, NULL);
	if (s == NULL) return NULL;
	_mcinternal_lock(s);
    MC_MEMCPY(s, str, len);
	s[len] = '\0';

	_mcinternal_ptrinfo(s)->count--; /* Don't count the null terminator */
	_mcinternal_unlock(s);
	return s;
}
static mstring_t *mc_nullable mstr(const mc_char_t *mc_nonnull str)
{ return managed_string(str, strlen(str)); }


/**
* Duplicates the managed string str
*/
static mstring_t *mc_nullable managed_string_duplicate(const mstring_t *mc_nonnull str)
{
	long int length = mstrlen(str); 
	if (length == -1) return NULL;
	return managed_string(str, (size_t)length);
}
static mstring_t *mc_nullable mstrdup(const mstring_t *mc_nonnull str)
{ return managed_string_duplicate(str); }

#define mstrcat_unsafe(str1, str2) managed_string_concatenate(str1, str2, mstrlen(str2) < 0 ? strlen(str2) : mstrlen(str2))
#define mstrcat(str1, str2) managed_string_concatenate(str1, str2, strlen(str2))
#define mstrncat(str1, str2, len) managed_string_concatenate(str1, str2, len)
#define mstrmcat(str1, str2) managed_string_concatenate(str1, mstrlen(s2))
/**
* Allocates a new string of needed size, and concatenates the 2 strings into it
*/
static mstring_t *mc_nullable managed_string_concatenate(mstring_t *mc_nonnull s1, const mc_char_t *mc_nonnull s2, size_t s2len)
{
	mstring_t *s = NULL;
	long int s1len = mstrlen(s1), total;
	if (s1len < 1) return NULL;
	total = (long int)s1len + (long int)s2len;

	s = managed_allocate((size_t)total + 1, sizeof(mc_char_t), NULL, NULL);
	if (s == NULL) return NULL;
	_mcinternal_lock(s);
	_mcinternal_lock(s1);
	
	MC_MEMCPY(s, s1, (size_t)s1len); /* It cannot be a param for data because that assumes that sizeof(data) == count * typesize */
	MC_MEMCPY((mstring_t *)(s + s1len), s2, s2len);
	((mstring_t *)s)[total] = '\0';

	_mcinternal_ptrinfo(s)->count--; /* Don't count the null terminator */


	_mcinternal_unlock(s1);
	_mcinternal_unlock(s);
	return s;
}


/**
* Compares each character of both strings, returning 0 if not equal, and 1 if equal
*/
static int managed_string_equals(mstring_t *mc_nonnull s1, const mc_char_t *mc_nonnull s2, size_t s2len)
{
	long int s1len = mstrlen(s1), i = 0, managed = 1;
	if (s1len == -1) {
		s1len = (long int)strlen(s1);
		_mcinternal_lock(s1);
		managed = 0;
	}

	if (s1len != (long int)s2len) return 0;

	for (i = 0; i < s1len; i++)
		if (((mc_char_t *)s1)[i] != ((const mc_char_t *)s2)[i]) return 0;

	if (managed)
		_mcinternal_unlock(s1);
	return 1;
}

static int mstreq_unsafe(mstring_t *mc_nonnull s1, const mc_char_t *mc_nonnull s2)
{
    long int s1len = mstrlen(s1), s2len = mstrlen(s2), i = 0, s1_managed = 1, s2_managed = 1;
    if (s1len == -1) {
		s1len = (long int)strlen(s1);
		s1_managed = 0;
	} else _mcinternal_lock(s1);

    if (s2len == -1) { 
		s2len = (long int)strlen(s2);
		s2_managed = 0;
	} else _mcinternal_lock(s2);

    if (s1len != s2len) return 0;

    for (i = 0; i < s1len; i++)
        if (((mc_char_t *)s1)[i] != ((const mc_char_t *)s2)[i]) return 0;

/*INTENTIONAL INDENTATION*/
if (s1_managed) _mcinternal_unlock(s1);
	if (s2_managed) _mcinternal_unlock(s2);

    return 1;
}

static int mstreq(mstring_t *mc_nonnull s1, const mc_char_t *mc_nonnull s2)
{ return managed_string_equals(s1, s2, strlen(s2)); }

static int mstrneq(mstring_t *mc_nonnull s1, const mc_char_t *mc_nonnull s2, size_t len)
{ return managed_string_equals(s1, s2, len); }

#endif
