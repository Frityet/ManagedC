#include "managed.h"
#include "test.h"

declaretest(string)
{
	mstring *str = mstr("Hello, "), *full = NULL;
	ASSERT(str != NULL, "Could not allocate string!");

	{
		mstring *str2 = mstrcat(str, "World!");
		ASSERT(str2 != NULL, "Could not allocate string!");
		full = mc_ref(str2);
		mc_free(str);
		mc_free(str2);
	}

	ASSERT(full != NULL, "Could not allocate string!");
	ASSERT(mstrlen(full) == strlen("Hello, World!"), "Length did not match!");
	ASSERT(mstreq(full, "Hello, World!"), "String did not match!");

	mc_free(full);
	return success;
}
