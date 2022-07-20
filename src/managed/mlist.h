#include "managed.h"
#if !defined(MANAGEDC_MLIST)
#define MANAGEDC_MLIST

#define mlist(T) T *const 

static void managed_list_free(mlist(void) *list)
{
	managed_free(*list);
}

static mlist(void) *managed_list(unsigned long int typesize, managed_Free_f *free)
{
	mlist(void) *base = managed_allocate(1, sizeof(void *), (managed_Free_f *)managed_list_free, NULL);

	return base;
}

#endif 
