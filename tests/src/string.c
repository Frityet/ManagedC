#include "managed.h"
#include "test.h"

declaretest(string)
{
	mstring *str = mstr("Hello, "), *full = NULL;
	ASSERT(str != NULL, "Could not allocate string!");

	{
		mstring *str2 = mstrcat(str, "World!");
		ASSERT(str2 != NULL, "Could not allocate string!");
		full = mc_reference(str2);
		mc_release(str);
		mc_release(str2);
	}

	ASSERT(full != NULL, "Could not allocate string!");
	ASSERT(mstrlen(full) == strlen("Hello, World!"), "Length did not match!");
	ASSERT(strcmp(full, "Hello, World!") == 0, "String did not match!");

	mc_release(full);
	return success;
}
