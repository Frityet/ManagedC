#include "managed/mstring.h"
#include "test.h"

declaretest(linkedlist)
{
	struct managed_LinkedList *list = mllist_new(mstring_t *, managed_release);
    mstring_t *s1 = mstr("Hello, "), *s2 = mstr("World"), *s3 = mstr("!");

	mllist_add(list, &s1);
	mllist_add(list, &s2);
	mllist_add(list, &s3);

	ASSERT(*list->length == 3, "Count did not match!");
	{	
		mstring_t 	**str1 = mllist_get(list, 0),
					**str2 = mllist_get(list, 1),
					**str3 = mllist_get(list, 2);
        size_t s1len = 0, s2len = 0, s3len = 0;

        ASSERT(str1 != NULL, "Could not get string!");
        ASSERT(str2 != NULL, "Could not get string!");
        ASSERT(str3 != NULL, "Could not get string!");

		ASSERT(*str1 != NULL, "Could not allocate string!");
		ASSERT(*str2 != NULL, "Could not allocate string!");
		ASSERT(*str3 != NULL, "Could not allocate string!");
        s1len = (size_t)mstrlen(*str1), s2len = (size_t)mstrlen(*str2), s3len = (size_t)mstrlen(*str3);

        ASSERT(s1len == strlen("Hello, "), "Length did not match!");
		ASSERT(s2len == strlen("World"), "Length did not match!");
		ASSERT(s3len == strlen("!"), "Length did not match!");
	}

	mc_free(list);
	return success;
}
