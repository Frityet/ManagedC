#if !defined(MANAGEDC_MAIN)
#define MANAGEDC_MAIN

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
extern int errno;

#if !defined(MC_GUARDPAGE_MAX)
#	define MC_GUARDPAGE_MAX ((uintptr_t)0x1000)
#endif

#if !defined(MC_ALLOCATOR)
# 	define MC_ALLOCATOR(c, s) calloc(c, s)
#endif

#if !defined(MC_MEMCPY)
#	define MC_MEMCPY(dst, src, nmemb) memcpy(dst, src, nmemb)
#endif

#if !defined(MC_MEMMOVE)
#	define MC_MEMMOVE(dst, src, nmemb) memmove(dst, src, nmemb)
#endif

#if !defined(MC_FREE)
# 	define MC_FREE(ptr) free(ptr)
#endif

#if defined(__STRICT_ANSI__)
#	define mc_nullable
# 	define mc_nonnull 
#	define mc_attribute(t)
#	define mc_defer
#	define mc_typeof(T) 
#	define mc_inline static
# 	define MC_EXPAND(t)
#else
# 	define MC_EXPAND(...) __VA_ARGS__
#	define mc_typeof(T) typeof(T)
#	define mc_attribute(...) __attribute__((__VA_ARGS__))
#	define mc_inline inline __attribute__((always_inline))
#	if defined(__clang__) && defined(__llvm__)
#		define mc_nullable _Nullable
# 		define mc_nonnull _Nonnull
# 		define mc_nocapture __block
# 		define mc_defer mc_attribute(cleanup(_mc_rundefer)) void (^_mc_##__LINE__##_deferexpr)(void) = ^ 
mc_inline void _mc_rundefer(void (^cb)(void))
{
	cb();
}

#	else
#		define mc_nullable 	
# 		define mc_nonnull 	
#		define mc_defer
#		define mc_nocapture
#	endif
#endif

#if defined(__STRICT_ANSI__)
#	define mc_auto "Running in ANSI standard mode (no extensions). This macro does not automatically release the pointer!";
#else 
#	define mc_auto mc_attribute(cleanup(managed_release_ptr))
# 	define MANAGED_HAS_AUTO
#endif

typedef void managed_Free_f(void *mc_nonnull alloc);

struct managed_PointerInfo {
	/**
	 * count: Number of used array elements.
	 * capacity: Number of allocated array elements. (Used for managed_Vector)
	 * typesize: Size of the type.
	 * reference_count: Number of references to this pointer.
	 */
	size_t count, capacity, typesize, reference_count;

	/**
	* Function to call on 0 references.
	*/
	managed_Free_f *mc_nonnull free;

	/**
	* Pointer to the data, should be just in front of data.
	*/
	void *mc_nonnull data;
};

#define mc_countof(ptr) (managed_info_of(ptr)->count)
mc_inline const struct managed_PointerInfo *mc_nullable managed_info_of(const void *mc_nonnull ptr)
{
	struct managed_PointerInfo *info = NULL;
	
	if (ptr == NULL || (uintptr_t)ptr < MC_GUARDPAGE_MAX) return NULL;
	info = (struct managed_PointerInfo *)ptr - 1;
	if (info->data != ptr)
		return NULL;

	return info;
}

#define mc_new(T, free) (T *)managed_allocate(1, sizeof(T), (managed_Free_f *)free, NULL)
static void *mc_nullable managed_allocate(size_t count, size_t typesize, managed_Free_f *mc_nullable free, const void *mc_nullable data)
{
	size_t total = sizeof(struct managed_PointerInfo) + count * typesize;

	struct managed_PointerInfo *info = MC_ALLOCATOR(1, total);
	if (info == NULL) return NULL;
	
	info->count 			= count;
	info->capacity 			= count;
	info->typesize 			= typesize;
	info->free 				= free;
	info->reference_count 	= 1;
	info->data 				= info + 1;

	if (data != NULL) {
		MC_MEMCPY(info->data, data, count * typesize);
	}

	return info->data;
}

#define mc_dup(ptr) managed_dup(ptr, managed_info_of(ptr)->count)
static void *mc_nullable managed_copy(const void *ptr, long int count)
{
	struct managed_PointerInfo 	*info = (void *)managed_info_of(ptr),
								*allocinfo = NULL;
								
	void *alloc = NULL; 
	if (count < 1) count = info->count;
	alloc = managed_allocate(count, info->typesize, info->free, NULL);
	if (alloc == NULL) return NULL;
	allocinfo = (void *)managed_info_of(ptr);

	allocinfo->count = info->count;
	MC_MEMCPY(alloc, ptr, info->count * info->typesize);

	return alloc;
}

#define mc_ref(ptr) MC_EXPAND((mc_typeof(ptr)))managed_reference(ptr)
static void *managed_reference(const void *mc_nonnull ptr)
{
	struct managed_PointerInfo *info = (void *)managed_info_of(ptr);
	info->reference_count++;
	return (void *)ptr;
}

#define mc_free(ptr) managed_release(ptr)
static void managed_release(const void *mc_nonnull ptr)
{
	struct managed_PointerInfo *info = (void *)managed_info_of(ptr);
	size_t i = 0;
	
	info->reference_count--;
	if (info->reference_count < 1) {
		if (info->free != NULL) {
			for (i = 0; i < info->count; i++) {
				info->free(((unsigned char *)ptr) + i * info->typesize);
			}
		}
		
		MC_FREE(info);
	}
}

#endif
