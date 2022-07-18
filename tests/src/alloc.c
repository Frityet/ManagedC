#include <stdint.h>
#include <stdio.h>

#include "managed.h"

int main()
{
	mptr(int) ptr;
	
	{
		mptr(int) i = mc_new(int, NULL);
		**i = 5;
		ptr = mc_ref(i); 
		mc_free(i);
	}

	mc_free(ptr);
}
