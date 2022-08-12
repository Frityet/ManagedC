#include "managed.h"

#include "test.h"

#define auto mc_auto

declaretest(mc_auto)
{
	#if defined (__STRICT_ANSI__)
	return success;
	#else

	auto int *i = mc_array(int, 10, NULL);
	ASSERT(i != NULL, "Allocation failed!");

	{
		mc_auto int *i2 = mc_ref(i);
		*i2 = 4;	
	}

	ASSERT(*i == 4, "Invaid value!");

	return success;
	#endif
}


