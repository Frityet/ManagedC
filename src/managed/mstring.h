#if !defined(MANAGEDC_MSTRING)
#define MANAGEDC_MSTRING

#include <string.h>

#include "managed.h"

#if !defined(MC_STRLEN)
#	define MC_STRLEN(s) strlen(s)
#endif

typedef const char mstring;

#define mstr(str) managed_string(str, strlen(str))
static mstring *mc_nullable managed_string(const char *mc_nonnull str, size_t len)
{
	mstring *s = managed_allocate(len + 1, sizeof(char), NULL, str);

	/*Just in case str is not null terminated*/
	((char *)s)[len] = '\0';

	return s;
}

#define mstrlen(str) managed_string_length(str)
static long int managed_string_length(mstring *mc_nonnull str)
{
	const struct managed_PointerInfo *info = managed_info_of(str);
	if (info == NULL) return -1;
	
	return info->count - 1; /*No null terminator*/
}

#define mstrdup(str) managed_string_duplicate(str)
static mstring *mc_nullable managed_string_duplicate(mstring *mc_nonnull str)
{
	long length = managed_string_length(str);
	if (length == -1) return NULL;
	return managed_string(str, length);
}

#define mstrcat(str1, str2) managed_string_concatenate(str1, str2, managed_string_length(str2) < 0 ? strlen(str2) : managed_string_length(str2))
static mstring *mc_nullable managed_string_concatenate(mstring *mc_nonnull s1, mstring *mc_nonnull s2, size_t s2len)
{
	mstring *s = NULL;
	long int s1len = managed_string_length(s1), total;
	total = s1len + s2len;

	s = managed_allocate(total + 1, sizeof(char), NULL, s1);
	if (s == NULL) return NULL;

	MC_MEMCPY((char *)(s + s1len), s2, s2len);
	((char *)s)[total] = '\0';

	return s;
}


#define mstreq(str1, str2) managed_string_equals(str1, str2, managed_string_length(str2) == -1 ? strlen(str2) : managed_string_length(str2))
static int managed_string_equals(mstring *mc_nonnull s1, const char *mc_nonnull s2, long int s2len)
{
	long int s1len = managed_string_length(s1), i = 0;
	if (s1len == -1) s1len = strlen(s1);
	if (s1len != s2len) return 0;

	for (i = 0; i < s1len; i++) {
		if (((char *)s1)[i] != ((char *)s2)[i]) return 0;
	}

	return 1;
}

#endif
