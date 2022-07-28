#include "test.h"

declaretest(linkedlist)
{
	struct managed_LinkedList *list = mllist_new(managed_release);
	size_t i = 0;

	mllist_add(list, mstr("Hello, "), 7);
	mllist_add(list, mstr("World!"), 7);
	mllist_add(list, mstr("!"), 1);

	for (i = 0; i < *list->count; i++) {
		mstring *str = mllist_get(list, i);
		printf("%lull: %s\n", i, str);
	}

	mc_free(list);
	return success;
}
