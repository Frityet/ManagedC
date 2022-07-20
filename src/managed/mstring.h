#if !defined(MANAGEDC_MSTRING)
#define MANAGEDC_MSTRING

#include <string.h>

#include "managed.h"

typedef char mstring;

#define mstr(str) managed_string(str, strlen(str))
static mstring *mc_nullable managed_string(const char *mc_nonnull str, unsigned long int len)
{
	mstring *s = managed_allocate(len + 1, sizeof(char), NULL, str);

	/*Just in case str is not null terminated*/
	((char *)s)[len] = '\0';

	return s;
}

#define mstrlen(str) managed_string_length(str)
static long int managed_string_length(const mstring *mc_nonnull str)
{
	const struct managed_PointerInfo *info = managed_info_of(str);
	if (info == NULL) return -1;
	
	return info->count - 1; /*No null terminator*/
}

#define mstrdup(str) managed_string_duplicate(str)
static mstring *mc_nullable managed_string_duplicate(const mstring *mc_nonnull str)
{
	long length = managed_string_length(str);
	if (length == -1) return NULL;
	return managed_string(str, length);
}

#define mstrcat(str1, str2) managed_string_concatenate(str1, str2)
static mstring *mc_nullable managed_string_concatenate(const mstring *mc_nonnull s1, const mstring *mc_nonnull s2)
{
	long int s1len = managed_string_length(s1), s2len = managed_string_length(s2), total;
	if (s1len == -1) s1len = strlen(s1);
	if (s2len == -1) s2len = strlen(s2);
	total = s1len + s2len;

	mstring *s = managed_allocate(total + 1, sizeof(char), NULL, s1);
	if (s == NULL) return NULL;

	memcpy((char *)(s + s1len), s2, s2len);
	((char *)s)[total] = '\0';

	return s;
}

static int managed_string_compare(const mstring *mc_nonnull s1, const mstring *mc_nonnull s2)
{
	long int s1len = managed_string_length(s1), s2len = managed_string_length(s2), total;
	if (s1len == -1) s1len = strlen(s1);
	if (s2len == -1) s2len = strlen(s2);
	if (s1len != s2len) return 0;

	for (total = 0; total < s1len; total++) {
		if (((char *)s1)[total] != ((char *)s2)[total]) return 0;
	}

	return 1;
}

#endif
