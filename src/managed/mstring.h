#if !defined(MANAGEDC_MSTRING)
#define MANAGEDC_MSTRING

#include "managed.h"

typedef char *mc_nonnull const mstring;

static mstring *managed_string(const char *value, unsigned long len)
{ 
	mstring *new = (mstring *)managed_allocate(len + 1, sizeof(char), NULL, value);
	(*new)[len + 1] = '\0';

	return (mstring *)new;
}

static mstring *managed_string_concatenate(mstring *dst, const char *src, unsigned long len)
{
	unsigned long dstlen = mc_count(dst), total = dstlen + len + 1; 

	char **new = (char **)mc_array(char, total, NULL);

	memcpy(*(mptr)new, *dst, dstlen);
	(*new)[total] = '\0';

	return (mstring *)new;
}

#endif
