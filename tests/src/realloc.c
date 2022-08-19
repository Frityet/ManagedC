#include "test.h"

declaretest(realloc)
{
	int arr[] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
	int *alloc = managed_allocate(10, sizeof(int), NULL, arr), *new = NULL, i = 0;
	ASSERT(alloc != NULL, "Could not allocate 10 ints!");
	for (i = 0; i < 10; i++)
		ASSERT(alloc[i] == (i + 1) * 10, "Value did not set!");

	new = managed_copy(alloc, 20);
	ASSERT(new != NULL, "Could not allocate 20 ints!");
	for (i = 0; i < 10; i++)
		ASSERT(alloc[i] == (i + 1) * 10, "Value did not set!");

	memcpy(new + 10, arr, sizeof(arr));
	ASSERT(new[10] == 10, "Value did not set!");

    mc_free(alloc);
    mc_free(new);
	return success;
}
