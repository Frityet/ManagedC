#if !defined(MANAGEDC_MAIN)
#define MANAGEDC_MAIN

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
extern int errno;

#if defined(__STRICT_ANSI__)
#	define mc_nullable
# 	define mc_nonnull 
#	define mc_attribute(...)
#	if !defined(defer)
#		define defer
#	endif
#	define mc_typeof(T) 
#	define mc_inline static
# 	define MC_EXPAND(...)
#else
# 	define MC_EXPAND(...) __VA_ARGS__
#	define mc_typeof(T) typeof(T)
#	define mc_attribute(...) __attribute__((__VA_ARGS__))
#	define mc_inline inline __attribute__((always_inline))
#	if defined(__clang__) && defined(__llvm__)
#		define mc_nullable _Nullable
# 		define mc_nonnull _Nonnull
#	else
#		define mc_nullable 	
# 		define mc_nonnull 	
#		define defer
#		define nocapture
#	endif
#endif

#if defined(__STRICT_ANSI__)
#	define auto 		"Running in ANSI standard mode (no extensions). This macro does not automatically release the pointer!"
#else 
#	define managed_auto mc_attribute(cleanup(managed_release_ptr))
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
	unsigned long int count, typesize, reference_count;

	/**
	* Function to call on 0 references.
	*/
	managed_Free_f *mc_nonnull free;

	/**
	* Pointer to the data, should be just in front of data.
	*/
	void *mc_nonnull data;
};

mc_inline const struct managed_PointerInfo *mc_nullable managed_info_of(const void *ptr)
{
	struct managed_PointerInfo *info = (struct managed_PointerInfo *)ptr - 1;
	if (info->data != ptr)
		return NULL;

	return info;
}

#define mc_new(T, free) (T *)managed_allocate(1, sizeof(T), (managed_Free_f *)free, NULL)
static void *mc_nullable managed_allocate(unsigned long int count, unsigned long int typesize, managed_Free_f *mc_nullable free, const void *mc_nullable data)
{
	unsigned long int total = sizeof(struct managed_PointerInfo) + count * typesize;

	struct managed_PointerInfo *info = calloc(1, total);
	if (info == NULL) return NULL;
	
	info->count 			= count;
	info->typesize 			= typesize;
	info->free 				= free;
	info->reference_count 	= 1;
	info->data 				= info + 1;

	if (data != NULL) {
		memcpy(info->data, data, count * typesize);
	}

	return info->data;
}

static void *mc_nullable managed_copy(const void *ptr, unsigned long int count)
{
	struct managed_PointerInfo 	*info = (void *)managed_info_of(ptr),
								*allocinfo = NULL;
	void *alloc = managed_allocate(count, info->typesize, info->free, NULL);
	if (alloc == NULL) return NULL;
	allocinfo = (void *)managed_info_of(ptr);

	allocinfo->count = info->count;
	memcpy(alloc, ptr, info->count * info->typesize);

	return alloc;
}

#define mc_ref(ptr) MC_EXPAND((mc_typeof(ptr)))managed_reference(ptr)
static void *managed_reference(void *mc_nonnull ptr)
{
	struct managed_PointerInfo *info = (void *)managed_info_of(ptr);
	info->reference_count++;
	return ptr;
}

#define mc_release(ptr) managed_free(ptr)
static void managed_free(void *mc_nonnull ptr)
{
	struct managed_PointerInfo *info = (void *)managed_info_of(ptr);
	unsigned long int i = 0;
	
	info->reference_count--;
	if (info->reference_count < 1) {
		if (info->free != NULL) {
			for (i = 0; i < info->count; i++) {
				info->free(ptr + i * info->typesize);
			}
		}
		
		free(info);
	}
}

#endif