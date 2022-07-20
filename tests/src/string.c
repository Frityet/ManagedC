#include "managed.h"
#include "test.h"

TEST(string)
{
	mstring *str = mstr("Hello, "), *full = NULL;
	ASSERT(str != NULL, "Could not allocate string!");

	{
		mstring *str2 = mstrcat(str, "World!");
		ASSERT(str2 != NULL, "Could not allocate string!");
		full = mc_ref(str2);
		mc_release(str2);
	}

	

	mc_release(str);
	return success;
}
