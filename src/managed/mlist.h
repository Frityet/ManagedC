#if !defined(MANAGEDC_MLIST)
#define MANAGEDC_MLIST

#include "managed.h"

#define mlist(T) T *const

#define _mcinternal_ptrinfo(ptr) ((struct managed_PointerInfo *)managed_info_of(ptr))

static void managed_list_free(mlist(void) *list)
{
	managed_free(*list);
}

static mlist(void) *managed_list(unsigned long int typesize, managed_Free_f *free)
{
	mlist(void) *base = managed_allocate(1, sizeof(void *), (managed_Free_f *)managed_list_free, NULL);
	if (base == NULL) return NULL;

	*((void **)base) = managed_allocate(2, typesize, free, NULL);
	if (*base == NULL) {
		managed_free((void *)base);
		return NULL;
	}

	_mcinternal_ptrinfo(*base)->count = 0;

	return base;
}

static int managed_list_push(void *ptr, void *data)
{
	mlist(void) *list = ptr;
	struct managed_PointerInfo ldata = *_mcinternal_ptrinfo(*list);

	if (ldata.count >= ldata.capacity) {
		void *new = managed_copy(*list, ldata.count * 1.5);
		if (new == NULL) return 1; /*Failed allocation*/
		managed_free(*list);
		*(void **)list = new;
	}

	_mcinternal_ptrinfo(*list)->count = ldata.count + 1;

	MC_MEMCPY(*list + ldata.count, data, ldata.typesize);

	return 0;
}

static int managed_list_pop(void *ptr, unsigned long int index)
{
	mlist(void) *list = ptr;
	struct managed_PointerInfo ldata = *_mcinternal_ptrinfo(*list);
	unsigned long int i = 0, c = 0;

	if (index >= ldata.count) return 2; /*Index out of bounds*/

	void *newalloc = managed_allocate(ldata.capacity, ldata.typesize, ldata.free, NULL);
	if (newalloc == NULL) return 1; /*Failed allocation*/
	_mcinternal_ptrinfo(newalloc)->count = ldata.count - 1;

	for (i = 0; i < ldata.count; i++) {
		if (i == index) continue;

		MC_MEMCPY(newalloc + i * ldata.typesize, *list + i * ldata.typesize, ldata.typesize);
		c++; /* lol */
	}

	managed_free(*list);
	*(void **)list = newalloc;

	return 0;
}

#undef _mcinternal_ptrinfo

#endif 
