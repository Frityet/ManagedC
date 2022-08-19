#include "managed.h"
#include "managed/mlist.h"
#include "test.h"

declaretest(list)
{
	mlist(size_t) *list = mlist_new(size_t, NULL);
	size_t i = 0, listlen = 0;

	ASSERT(list != NULL, "Could not allocate list!");
	for (i = 0; i < (1 << 16); i++)
		mlist_add(list, &i);

	listlen = (size_t)mc_countof(list);
	ASSERT(listlen == (1 << 16), "Count did not match!");

	for (i = 0; i < listlen; i++) {
		size_t val = *(size_t *)mlist_get(list, i);
		ASSERT(val == i, "Value did not match!");
	}

	mlist_set(list, 0, &i);
	ASSERT(*(size_t *)mlist_get(list, 0) == (1 << 16), "Value did not set!");

	mlist_rm(list, 0);
	ASSERT(*(size_t *)mlist_get(list, 0) != (1 << 16), "Value did not remove!");
	ASSERT(mc_countof(list) == (1 << 16) - 1, "List length did not lower!");

	mc_free(list);
	return success;
}
