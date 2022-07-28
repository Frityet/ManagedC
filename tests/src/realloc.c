#include "test.h"

declaretest(realloc)
{
	int arr[] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
	int *alloc = managed_allocate(10, sizeof(int), NULL, arr), *new = NULL;
	ASSERT(alloc != NULL, "Could not allocate 10 ints!");
	ASSERT(alloc[0] == 10, "Value did not set!");
	new = managed_copy(alloc, 20);
	ASSERT(new != NULL, "Could not allocate 20 ints!");
	ASSERT(new[0] == 10, "Value did not set!");

	memcpy(new + 10, arr, sizeof(arr));
	ASSERT(new[10] == 10, "Value did not set!");

	return success; 
}
