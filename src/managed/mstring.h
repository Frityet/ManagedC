#if !defined(MANAGEDC_MSTRING)
#define MANAGEDC_MSTRING

#include <string.h>

#include "managed.h"

typedef const char mstring;

#define _mcinternal_ptrinfo(ptr) ((struct managed_PointerInfo *)managed_info_of(ptr))

#define mstrlen(str) mc_countof(str)

#define mstr(str) managed_string(str, strlen(str))
static mstring *mc_nullable managed_string(const char *mc_nonnull str, size_t len)
{
	mstring *s = managed_allocate(len + 1, sizeof(char), NULL, str);
	if (s == NULL) return NULL;

	_mcinternal_ptrinfo(s)->count--; /* Don't count the null terminator */

	/*Just in case str is not null terminated*/
	((char *)s)[len] = '\0';

	return s;
}

#define mstrdup(str) managed_string_duplicate(str)
static mstring *mc_nullable managed_string_duplicate(mstring *mc_nonnull str)
{
	long length = mstrlen(str);
	if (length == -1) return NULL;
	return managed_string(str, length);
}

#define mstrcat_unsafe(str1, str2) managed_string_concatenate(str1, str2, mstrlen(str2) < 0 ? strlen(str2) : mstrlen(str2))
#define mstrcat(str1, str2) managed_string_concatenate(str1, str2, strlen(str2))
#define mstrncat(str1, str2, len) managed_string_concatenate(str1, str2, len)
#define mstrmcat(str1, str2) managed_string_concatenate(str1, mstrlen(s2))
static mstring *mc_nullable managed_string_concatenate(mstring *mc_nonnull s1, mstring *mc_nonnull s2, size_t s2len)
{
	char *s = NULL;
	size_t s1len = mstrlen(s1), total;
	total = s1len + s2len;

	s = managed_allocate(total + 1, sizeof(char), NULL, NULL);
	if (s == NULL) return NULL;
	MC_MEMCPY(s, s1, s1len); /* It cannot be a param for data because that assumes that sizeof(data) == count * typesize */
	MC_MEMCPY((char *)(s + s1len), s2, s2len);
	((char *)s)[total] = '\0';

	_mcinternal_ptrinfo(s)->count--; /* Don't count the null terminator */

	return s;
}

#define mstreq_unsafe(str1, str2) managed_string_equals(str1, str2, managed_string_length(str2) == -1 ? strlen(str2) : managed_string_length(str2))
#define mstreq(str1, str2) managed_string_equals(str1, str2, strlen(str2))
#define mstrneq(str1, str2, len) managed_string_equals(str1, str2, len)
#define mstrmeq(str1, str2) managed_string_equals(str1, str2, managed_string_length(str2))
static int managed_string_equals(mstring *mc_nonnull s1, const char *mc_nonnull s2, long int s2len)
{
	long int s1len = mstrlen(s1), i = 0;
	if (s1len == -1) s1len = strlen(s1);
	if (s1len != s2len) return 0;

	for (i = 0; i < s1len; i++) {
		if (((char *)s1)[i] != ((char *)s2)[i]) return 0;
	}

	return 1;
}



#endif
