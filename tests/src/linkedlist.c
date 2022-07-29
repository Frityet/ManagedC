#include "managed/mstring.h"
#include "test.h"

declaretest(linkedlist)
{
	struct managed_LinkedList *list = mllist_new(managed_release);
	size_t i = 0;

	mllist_add(list, mstr("Hello, "));
	mllist_add(list, mstr("World"));
	mllist_add(list, mstr("!"));

	ASSERT(*list->count == 3, "Count did not match!");

	{	
		mstring *str1 = mllist_get(list, 0), 
				*str2 = mllist_get(list, 1), 
				*str3 = mllist_get(list, 2);

		ASSERT(str1 != NULL, "Could not allocate string!");
		ASSERT(str2 != NULL, "Could not allocate string!");
		ASSERT(str3 != NULL, "Could not allocate string!");

		ASSERT(mstrlen(str1) == strlen("Hello, "), "Length did not match!");
		ASSERT(mstrlen(str2) == strlen("World"), "Length did not match!");
		ASSERT(mstrlen(str3) == strlen("!"), "Length did not match!");
	}

	mc_free(list);
	return success;
}
