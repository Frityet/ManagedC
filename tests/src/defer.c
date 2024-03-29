#include "managed.h"
#include "test.h"

#include <setjmp.h>
#include <stdio.h>

declaretest(mcdefer)
{
	#if !MC_LLVM
	return success;
	#else

#	define auto mc_auto
#	define defer mc_defer
#	define nocapture mc_nocapture

	nocapture bool fail = false;
	auto int *buf = mc_array(int, _JBLEN, NULL);
	nocapture int jmp = setjmp(buf);
	if (fail) ASSERT(false, "Defer did not complete properly!");
	
	int i = 5;
	defer { 
		if (i != 5) {
			fail = true;
			longjmp(buf, true);
		}
	};
	i = 2;

	nocapture int i_nc = 5;
	defer {
		if (i == 5) {
			fail = true;
			longjmp(buf, true);
		}
	};

	return success;
	#endif
}
