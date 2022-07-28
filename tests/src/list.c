#include "managed.h"
#include "managed/mlist.h"
#include "test.h"
#include <stdio.h>

declaretest(list)
{
	mlist(size_t) *list = mlist_new(size_t, NULL);
	size_t i = 0, listlen = 0;

	ASSERT(list != NULL, "Could not allocate list!");
	for (i = 0; i < (1 << 16); i++) 
		mlist_push(list, &i);

	listlen = mlist_length(list);

	ASSERT(mlist_length(list) == (1 << 16), "Count did not match!");


	for (i = 0; i < (1 << 16); i++) 
	{
		size_t val = *(size_t *)mlist_get(list, i);
		ASSERT(val == i, "Value did not match!");
	}

	mc_free(list);
	return success;
}
