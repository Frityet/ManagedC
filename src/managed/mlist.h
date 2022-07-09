#pragma once

#include "managed.h"
#include <stddef.h>

static void *nullable MC_ADD_PREFIX(new_array)(size_t size, size_t count, MC_ADD_PREFIX(FreePointer_f) *nullable free_func)
{
	void *ptr = MC_ADD_PREFIX(alloc_managed)(size, count, free_func);
	if (ptr == NULL)
		return NULL;
	
	memset(ptr, 0, size * count);
	return ptr;

}
