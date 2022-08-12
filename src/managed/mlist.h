#if !defined(MANAGEDC_MLIST)
#define MANAGEDC_MLIST

#include "managed.h"

#define mlist(T) T *const

#define mlist_length(list) 		(mc_countof(*list))
#define mlist_capacity(list) 	(_mcinternal_ptrinfo(*list)->capacity)

#define _mcinternal_ptrinfo(ptr) ((struct managed_PointerInfo *)managed_info_of(ptr))

#define MC_ASSERT_IS_MLIST(obj) (mlist(mc_typeof(**obj)) *)obj

static void managed_list_free(const mlist(void) *list)
{
	managed_release(*list);
}

#define mlist_new(type, free) (mlist(type) *)managed_list(sizeof(type), 2, free, NULL)
static mlist(void) *managed_list(size_t typesize, size_t count, managed_Free_f *free, void *data)
{
	void **list = managed_allocate(1, sizeof(void *), (managed_Free_f *)&managed_list_free, NULL);
	size_t cap = 0;
	if (list == NULL) return NULL;

	cap = count < 2 ? 2 : count % 2 == 0 ? count : count + 1;
	*list = managed_allocate(cap, typesize, free, NULL);
	if (*list == NULL) {
		mc_free(list);
		return NULL;
	}

	if (data != NULL)
		MC_MEMCPY(*list, data, typesize * count);

	_mcinternal_ptrinfo(*list)->count = 0;
	_mcinternal_ptrinfo(*list)->capacity = cap;

	return list;
}

#if defined(__STRICT_ANSI__)
#	define mlist_push(list, data) managed_list_push(list, data)
#else
# 	define mlist_push(list, data) managed_list_push(MC_ASSERT_IS_MLIST(list), (mc_typeof(*list)) data)
#endif
static int managed_list_push(const void *ptr, const void *data)
{
	mlist(void) *list = ptr;
	struct managed_PointerInfo *listinfo_ptr = _mcinternal_ptrinfo(*list), listinfo;
	volatile size_t s = *(size_t *)data;
	if (listinfo_ptr == NULL) return 1;
	listinfo = *listinfo_ptr; /*We need the copy or else we would start accessing potentially freed memory*/

	if (listinfo.count >= listinfo.capacity) {
		size_t newcap = listinfo.capacity * 2, oldc = listinfo.count;
		void *newalloc = managed_allocate(newcap, listinfo.typesize, listinfo.free, NULL);
		struct managed_PointerInfo *newallocinfo = NULL; 
		if (newalloc == NULL) return 1;

		newallocinfo = _mcinternal_ptrinfo(newalloc);
		newallocinfo->count = oldc;

		MC_MEMCPY(newalloc, *list, listinfo.typesize * oldc);

		managed_release(*list);
		*((void **)list) = newalloc;
	}
	
	MC_MEMCPY(((unsigned char *)*list) + listinfo.count * listinfo.typesize, data, listinfo.typesize);
	_mcinternal_ptrinfo(*list)->count++;

	return 0;
}

#if defined (__STRICT_ANSI__)
#	define mlist_pop(list, index) managed_list_pop(list, index)
#else
#	define mlist_pop(list, index) managed_list_pop(MC_ASSERT_IS_MLIST(list), index)
#endif
static int managed_list_pop(const void *ptr, size_t index)
{
	mlist(void) *list = ptr;
	struct managed_PointerInfo *listinfo = _mcinternal_ptrinfo(*list);
	if (listinfo == NULL) return 2;
	if (index >= listinfo->count) return 1;

	MC_MEMMOVE(((unsigned char *)*list) + index * listinfo->typesize, ((unsigned char *)*list) + (index + 1) * listinfo->typesize, (listinfo->count - index - 1) * listinfo->typesize);
	listinfo->count--;

	return 0;
}

#if defined (__STRICT_ANSI__)
#	define mlist_get(list, index) managed_list_get(list, index)
#else
#	define mlist_get(list, index) (mc_typeof(*list))managed_list_get(MC_ASSERT_IS_MLIST(list), index)
#endif
static void *managed_list_get(const void *ptr, size_t index)
{
	mlist(void) *list = ptr;
	struct managed_PointerInfo *listinfo = _mcinternal_ptrinfo(*list);
	if (listinfo == NULL) return NULL;
	if (index >= listinfo->count) return NULL;
	return ((unsigned char *)*list) + index * listinfo->typesize;
}

#if defined(__STRICT_ANSI__)
#	define mlist_set(list, index, data) managed_list_set(list, index, data)
#else
#	define mlist_set(list, index, data) managed_list_set(MC_ASSERT_IS_MLIST(list), index, (mc_typeof(*list))data)
#endif
static int managed_list_set(void *ptr, size_t index, void *data)
{
	mlist(void) *list = ptr;
	struct managed_PointerInfo *listinfo = _mcinternal_ptrinfo(*list);
	if (listinfo == NULL) return 2;
	if (index >= listinfo->count) return 1;
	MC_MEMCPY(((unsigned char *)*list) + index * listinfo->typesize, data, listinfo->typesize);
	return 0;
}

#define mlist_copy(list) (mlist(mc_typeof(**list)) *)managed_list_copy(MC_ASSERT_IS_MLIST(list))
static mlist(void) *managed_list_copy(const void *ptr)
{
	mlist(void) *list = ptr;
	struct managed_PointerInfo *listinfo = _mcinternal_ptrinfo(*list);
	if (listinfo == NULL) return NULL;
	return managed_list(listinfo->typesize, listinfo->count, listinfo->free, *list);
}

#undef _mcinternal_ptrinfo

#endif 
