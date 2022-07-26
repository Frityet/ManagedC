#include "managed.h"
#include "managed/mlist.h"
#include "test.h"
#include <stdio.h>

declaretest(list)
{
	mlist(unsigned long int) *list = mlist_create(unsigned long int, NULL);
	unsigned long int i = 0;

	ASSERT(list != NULL, "Could not allocate list!");
	for (i = 0; i < (1 << 16); i++) 
	{
		unsigned long *alloc = managed_allocate(i, sizeof(unsigned long), NULL, NULL);
		ASSERT(alloc != NULL, "Could not allocate ints!");
		mlist_push(list, &i);
	}

	ASSERT(mc_countof(*list) == (1 << 16), "Count did not match!");

	for (i = 0; i < (1 << 16); i++) 
	{
		unsigned long int val = *(unsigned long int *)mlist_get(list, i);
		printf("%lu: %lu\n", i, val);
		ASSERT(val == i, "Value did not match!");
	}

	mc_ref(list);
	return success;
}
