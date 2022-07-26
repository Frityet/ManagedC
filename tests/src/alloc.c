#include <stdint.h>
#include <stdio.h>

#include "managed.h"
#include "test.h"

declaretest(alloc)
{
	int *val = NULL;

	{
		int *i = mc_new(int, NULL);
		ASSERT(i != NULL, "Could not allocate 4 bytes!");

		*i = 512;
		val = mc_reference(i);
		mc_release(i);
	}

	ASSERT(*val == 512, "Value did not set!");

	return success;
}
