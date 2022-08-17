#include "managed/mstring.h"
#include "test.h"

declaretest(linkedlist)
{
	struct managed_LinkedList *list = mllist_new(mstring *, managed_release);
	size_t i = 0;
    mstring *s1 = mstr("Hello, "), *s2 = mstr("World"), *s3 = mstr("!");

	mllist_add(list, &s1);
	mllist_add(list, &s2);
	mllist_add(list, &s3);

	ASSERT(*list->count == 3, "Count did not match!");

	{	
		mstring **str1 = mllist_get(list, 0),
				**str2 = mllist_get(list, 1),
				**str3 = mllist_get(list, 2);
        size_t s1len, s2len, s3len;

        ASSERT(str1 != NULL, "Could not get string!");
        ASSERT(str2 != NULL, "Could not get string!");
        ASSERT(str3 != NULL, "Could not get string!");

		ASSERT(*str1 != NULL, "Could not allocate string!");
		ASSERT(*str2 != NULL, "Could not allocate string!");
		ASSERT(*str3 != NULL, "Could not allocate string!");
        s1len = mstrlen(*str1), s2len = mstrlen(*str2), s3len = mstrlen(*str3);

        ASSERT(s1len == strlen("Hello, "), "Length did not match!");
		ASSERT(s2len == strlen("World"), "Length did not match!");
		ASSERT(s3len == strlen("!"), "Length did not match!");
	}

	mc_free(list);
	return success;
}
