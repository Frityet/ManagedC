#include "managed/mlist.h"
#include "test.h"

TEST(list)
{
	mlist(unsigned long int) *list = mlist_create(unsigned long int, NULL);
	ASSERT(list != NULL, "Could not allocate list!");
	unsigned long i = 0;

	for (i = 0; i < (1 << 16); i++) 
	{
		unsigned long *alloc = managed_allocate(i, sizeof(unsigned long), NULL, NULL);
		ASSERT(alloc != NULL, "Could not allocate ints!");
		mlist_push(list, &i);
	}

	ASSERT(mc_countof(*list) == (1 << 16), "Count did not match!");

	return success;
}
