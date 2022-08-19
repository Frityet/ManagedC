#include "managed.h"

#include "test.h"

declaretest(mcauto)
{
#	if MC_ANSI
	return success;
	#else
#	define auto mc_auto

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


