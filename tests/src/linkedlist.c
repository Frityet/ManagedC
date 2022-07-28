#include "test.h"

declaretest(linkedlist)
{
	struct managed_LinkedList *list = mllist_new(managed_release);
	size_t i = 0;

	mllist_add(list, mstr("Hello, "), 7);
	mllist_add(list, mstr("World!"), 7);
	mllist_add(list, mstr("!"), 1);

	ASSERT(*list->count == 3, "Count did not match!");

	mc_free(list);
	return success;
}
