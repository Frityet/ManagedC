#if !defined(MANAGEDC_MLIST)
#define MANAGEDC_MLIST

#include "managed.h"

#if defined(__STRICT_ANSI__)
    #define mlist(T) T *const
#else
    #define mlist(T) mc_typeof(T *const)
#endif

#define _mcinternal_ptrinfo(ptr) ((struct managed_PointerInfo *)managed_info_of(ptr))

#if defined(__GNUC__)
    #define MC_ASSERT_IS_MLIST(list) (({ mlist(__typeof__(**list)) *_list_test_t_ = list; _list_test_t_; }))
    #define MC_ASSERT_DATA_TYPE(list, obj) (({ __typeof__(**list) _obj_test_t_ = *(obj); (obj); }))
#else
    #define MC_ASSERT_IS_MLIST(obj) (obj)
    #define MC_ASSERT_DATA_TYPE(list, obj) (obj)
#endif

static void managed_list_free(const mlist(void) *list)
{
	managed_release(*list);
	/* Because we set the metadata for the list beforehand, we must now reset it to 0 to avoid the free function being called in memory we do not own */
	_mcinternal_ptrinfo(list)->count = 0;
	_mcinternal_ptrinfo(list)->capacity = 0;
}

#define mlist_new(type, free) (mlist(type) *)managed_list(sizeof(type), 2, free, NULL)
static mlist(void) *managed_list(size_t typesize, size_t count, managed_Free_f *free, void *data)
{
    /* If we try and reallocate a pointer by itself, all existing references will break. */
    /* To fix this for our list, we must allocate a pointer which will hold a pointer to the actual array */
	void **list = managed_allocate(1, sizeof(void *), (managed_Free_f *)&managed_list_free, NULL);
	size_t cap = 0;
	if (list == NULL) return NULL;

    /* Make sure it's even */
	cap = count < 2 ? 2 : count % 2 == 0 ? count : count + 1;
	*list = managed_allocate(cap, typesize, free, NULL);
	if (*list == NULL) {
		mc_free(list);
		return NULL;
	}

	if (data != NULL)
		MC_MEMCPY(*list, data, typesize * count);

	/* Do the same for the actual array */
	_mcinternal_ptrinfo(*list)->count = 0;
	_mcinternal_ptrinfo(*list)->capacity = cap;

	/* So the semantics of mc_countof(list) work, we manually set the struct fields */
	_mcinternal_ptrinfo(list)->count = 0;
	_mcinternal_ptrinfo(list)->capacity = cap;

	return list;
}

#if defined(__STRICT_ANSI__)
#	define mlist_add(list, data) managed_list_add(list, data)
#else
# 	define mlist_add(list, data) managed_list_add(MC_ASSERT_IS_MLIST(list), MC_ASSERT_DATA_TYPE(list, data))
#endif
static int managed_list_add(const void *ptr, const void *data)
{ 	/* The const in the arg is a complete lie, we must keep it or else the compiler complains though */
	const void **list = (void *)ptr;
	struct managed_PointerInfo *listinfo_ptr = _mcinternal_ptrinfo(*list), listinfo;
	if (listinfo_ptr == NULL) return 1;
	listinfo = *listinfo_ptr; /*We need the copy or else we would start accessing potentially freed memory*/

	if (listinfo.count >= listinfo.capacity) {
		/* 1.5 is the most efficent cap size multiplier because of the golden ratio or something like that */
		/* TODO: restudy math 11 so ~~I dont fail~~ I understand why the golden ratio is effective */
        size_t newcap = (size_t)listinfo.capacity * 1.5, oldc = listinfo.count;
		void *newalloc = managed_allocate(newcap, listinfo.typesize, listinfo.free, NULL);
		struct managed_PointerInfo *newallocinfo = NULL; 
		if (newalloc == NULL) return 1;

		newallocinfo = _mcinternal_ptrinfo(newalloc);
		newallocinfo->count = oldc;

		MC_MEMCPY(newalloc, *list, listinfo.typesize * oldc);
		managed_release(*list);
		*list = newalloc;

		_mcinternal_ptrinfo(list)->capacity = newcap;
	}
	
	MC_MEMCPY(((unsigned char *)*list) + listinfo.count * listinfo.typesize, data, listinfo.typesize);
	
	/* Update the list fields So the semantics of mc_countof(list) work */
	_mcinternal_ptrinfo(list)->count = ++_mcinternal_ptrinfo(*list)->count;
	return 0;
}

#if defined (__STRICT_ANSI__)
#	define mlist_rm(list, index) managed_list_remove(list, index)
#else
#	define mlist_rm(list, index) managed_list_remove(MC_ASSERT_IS_MLIST(list), index)
#endif
static int managed_list_remove(const void *ptr, size_t index)
{
	mlist(void) *list = ptr;
	struct managed_PointerInfo *listinfo = _mcinternal_ptrinfo(*list);
	if (listinfo == NULL) return 2;
	if (index >= listinfo->count) return 1;

	MC_MEMMOVE(((unsigned char *)*list) + index * listinfo->typesize, ((unsigned char *)*list) + (index + 1) * listinfo->typesize, (listinfo->count - index - 1) * listinfo->typesize);
	_mcinternal_ptrinfo(list)->count = --listinfo->count;

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
#	define mlist_set(list, index, data) managed_list_set(MC_ASSERT_IS_MLIST(list), index, MC_ASSERT_DATA_TYPE(list, data))
#endif
static int managed_list_set(const void *ptr, size_t index, void *data)
{
	mlist(void) *list = ptr;
	struct managed_PointerInfo *listinfo = _mcinternal_ptrinfo(*list);
	if (listinfo == NULL) return 1;
	if (index >= listinfo->count) return 2;
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
