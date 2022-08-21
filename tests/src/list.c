#include "test.h"

#include <stdint.h>

#define LIST_SIZE (1 << 24)

declaretest(list)
{
	mlist_t(size_t) *list = mlist_new(size_t, NULL);
	size_t i = 0, listlen = 0;
	
	ASSERT(list != NULL, "Could not allocate list!");
	for (i = 0; i < LIST_SIZE; i++)
		ASSERT(mlist_add(list, &i) == 0, "Could not add item!");

	listlen = (size_t)mc_countof(list);
	ASSERT(listlen == LIST_SIZE, "Count did not match!");

	for (i = 0; i < listlen; i++) {
		size_t *val = mlist_get(list, i);
		ASSERT(val != NULL, "Could not get value!");

		ASSERT(*val == i, "Value did not match!");
	}

	mlist_set(list, 0, &i);
	ASSERT(*(size_t *)mlist_get(list, 0) == LIST_SIZE, "Value did not set!");

	mlist_rm(list, 0);
	ASSERT(*(size_t *)mlist_get(list, 0) != LIST_SIZE, "Value did not remove!");
	ASSERT((size_t)mc_countof(list) == LIST_SIZE - 1, "List length did not lower!");

	mc_free(list);
	
	return success;
}
