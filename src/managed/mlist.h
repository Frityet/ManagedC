#if !defined(MANAGEDC_MLIST)
#define MANAGEDC_MLIST

#include "managed.h"

#define mlist_t(T) T *const


#define _mcinternal_ptrinfo(ptr) ((struct managed_PointerInfo *)managed_info_of(ptr))

#if defined(MC_MUTEX)
#	define _mcinternal_lock(ptr) 	(MC_MUTEX_LOCK(&_mcinternal_ptrinfo(ptr)->lock))
#	define _mcinternal_unlock(ptr) 	(MC_MUTEX_UNLOCK(&_mcinternal_ptrinfo(ptr)->lock))
#else
#	define _mcinternal_lock(ptr) 	
#	define _mcinternal_unlock(ptr) 	
#endif

#define MC_ASSERT_IS_MLIST(list) (({ mlist_t(mc_typeof(**list)) *_list_test_t_ = list; _list_test_t_; }))
#define MC_ASSERT_DATA_TYPE(list, obj) (({ mc_typeof(**list) *_obj_test_t_ = (obj); (obj); }))

static void managed_list_free(mlist_t(void) *list)
{
    managed_release(*list);
    /* Because we set the metadata for the list beforehand, we must now reset it to 0 to avoid the free function being called in memory we do not own */

    _mcinternal_ptrinfo(list)->count = 0;
    _mcinternal_ptrinfo(list)->capacity = 0;
}

#define mlist_new(type, free) (mlist_t(type) *)managed_list(sizeof(type), 2, free, NULL)
static mlist_t(void) *managed_list(size_t typesize, size_t count, managed_Free_f *free, const void *data)
{
    /* If we try and reallocate a pointer by itself, all existing references will break. */
    /* To fix this for our list, we must allocate a pointer which will hold a pointer to the actual array */
    void **list = managed_allocate(1, sizeof(void *), (managed_Free_f *)&managed_list_free, NULL);
    size_t cap = 0;
    if (list == NULL) return NULL;

    /* Make sure it's even */
    cap = count + (count & 1);
    *list = managed_allocate(cap, typesize, free, NULL);
    if (*list == NULL) {
        mc_free(list);
        return NULL;
    }

    _mcinternal_lock(*list);
    _mcinternal_lock(list);
    if (data != NULL)
        MC_MEMCPY(*list, data, typesize * count);

    /* Do the same for the actual array */
    _mcinternal_ptrinfo(*list)->count = 0;
    _mcinternal_ptrinfo(*list)->capacity = cap;

    /* So the semantics of mc_countof(list) work, we manually set the struct fields */
    _mcinternal_ptrinfo(list)->count = 0;
    _mcinternal_ptrinfo(list)->capacity = cap;

    _mcinternal_unlock(list);
    _mcinternal_unlock(*list);

    return list;
}

#if MC_ANSI
#	define mlist_add(list, data) managed_list_add(list, data)
#else
# 	define mlist_add(list, data) managed_list_add(MC_ASSERT_IS_MLIST(list), MC_ASSERT_DATA_TYPE(list, data))
#endif
static int managed_list_add(const void *ptr, const void *data)
{ 	/* The const in the arg is a complete lie, we must keep it or else the compiler complains though */
    void **list = (void **)ptr;

    struct managed_PointerInfo *listinfo = _mcinternal_ptrinfo(list), *list_data_info = NULL, arrayinfo;  
    if (listinfo == NULL) return 1;

    /*We must copy the data because the allocation is freed later in the function*/
    list_data_info = _mcinternal_ptrinfo(*list);
    if (list_data_info == NULL) return 2; 
    arrayinfo = *list_data_info;

    MC_MUTEX_LOCK(&listinfo->lock);

    if (arrayinfo.count >= arrayinfo.capacity) {
        /*1.5, being the closest to the golden ratio, is the most efficent cap size multiplier*/
        /*Why? I have 0 fuckin clue*/
        size_t newsiz = (size_t)((double)arrayinfo.capacity * 1.5);
        void *alloc = managed_allocate(newsiz, arrayinfo.typesize, arrayinfo.free, NULL);
        struct managed_PointerInfo *allocinfo = NULL;

        if (alloc == NULL) return 3;
        allocinfo = _mcinternal_ptrinfo(alloc);

        MC_MEMCPY(alloc, *list, arrayinfo.typesize * arrayinfo.count);
        allocinfo->count = arrayinfo.count;

        managed_release(*list);

        *list = alloc;
    }

    list_data_info = _mcinternal_ptrinfo(*list);
    if (list_data_info == NULL) return 2;

    MC_MUTEX_LOCK(&list_data_info->lock);
    MC_MEMCPY(((unsigned char *)*list) + arrayinfo.count * arrayinfo.typesize, data, arrayinfo.typesize);

    list_data_info->count++;
    listinfo->count = list_data_info->count;

    MC_MUTEX_UNLOCK(&list_data_info->lock);
    MC_MUTEX_UNLOCK(&listinfo->lock);
    return 0;
}

#if MC_ANSI
#	define mlist_rm(list, index) managed_list_remove(list, index)
#else
#	define mlist_rm(list, index) managed_list_remove(MC_ASSERT_IS_MLIST(list), index)
#endif
static int managed_list_remove(const void *ptr, size_t index)
{
    mlist_t(void) *list = (void *)ptr;
    struct managed_PointerInfo *datainfo = _mcinternal_ptrinfo(*list), *listinfo = _mcinternal_ptrinfo(list);
    if (datainfo == NULL) return 1;
    
    MC_MUTEX_LOCK(&listinfo->lock);
    MC_MUTEX_LOCK(&datainfo->lock);
    if (index >= datainfo->count) return 2;

    MC_MEMMOVE(((unsigned char *)*list) + index * datainfo->typesize, ((unsigned char *)*list) + (index + 1) * datainfo->typesize, (datainfo->count - index - 1) * datainfo->typesize);
    listinfo->count = --datainfo->count;

    MC_MUTEX_UNLOCK(&datainfo->lock);
    MC_MUTEX_UNLOCK(&listinfo->lock);
    return 0;
}

#if MC_ANSI
#	define mlist_get(list, index) managed_list_get(list, index)
#else
#	define mlist_get(list, index) (mc_typeof(*list))managed_list_get(MC_ASSERT_IS_MLIST(list), index)
#endif
static void *managed_list_get(const void *ptr, size_t index)
{
    mlist_t(void) *list = (void *)ptr;
    struct managed_PointerInfo *listinfo = _mcinternal_ptrinfo(*list);
    void *res = NULL;
    if (listinfo == NULL) return NULL;
    MC_MUTEX_LOCK(&listinfo->lock);
    if (index >= listinfo->count) return NULL;
    res = ((unsigned char *)*list) + index * listinfo->typesize;
    MC_MUTEX_UNLOCK(&listinfo->lock);
    return res;
}

#if MC_ANSI
#	define mlist_set(list, index, data) managed_list_set(list, index, data)
#else
#	define mlist_set(list, index, data) managed_list_set(MC_ASSERT_IS_MLIST(list), index, MC_ASSERT_DATA_TYPE(list, data))
#endif
static int managed_list_set(const void *ptr, size_t index, const void *data)
{
    mlist_t(void) *list = (void *)ptr;
    struct managed_PointerInfo *listinfo = _mcinternal_ptrinfo(*list);
    if (listinfo == NULL) return 1;
    MC_MUTEX_LOCK(&listinfo->lock);
    if (index >= listinfo->count) return 2;
    MC_MEMCPY(((unsigned char *)*list) + index * listinfo->typesize, data, listinfo->typesize);
    MC_MUTEX_UNLOCK(&listinfo->lock);
    return 0;
}

#if MC_ANSI
#	define mlist_copy(list) managed_list_copy(list)
#else
#	define mlist_copy(list) (mlist_t(mc_typeof(**list)) *)managed_list_copy(MC_ASSERT_IS_MLIST(list))
#endif
static mlist_t(void) *managed_list_copy(const void *ptr)
{
    mlist_t(void) *list = (void *)ptr, *const *ret;
    struct managed_PointerInfo *listinfo = _mcinternal_ptrinfo(*list);
    if (listinfo == NULL) return NULL;
    MC_MUTEX_LOCK(&listinfo->lock);
    ret = managed_list(listinfo->typesize, listinfo->count, listinfo->free, *list);
    MC_MUTEX_UNLOCK(&listinfo->lock);
    return ret;
}

#undef _mcinternal_ptrinfo

#endif 
