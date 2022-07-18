#ifndef MANAGEDC_MAIN
#define MANAGEDC_MAIN

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#if defined (__STRICT_ANSI__)
#	define MC_AN_C true
#else
#	define MC_AN_C false
#endif

#if defined (__GNUC__)
#	define MC_GNU_C true 
#else
#	define MC_GNU_C false
#endif

#if defined (__clang__) && defined (__llvm__)
#	define MC_CLANG_C true
#else
#	define MC_CLANG_C false
#endif

#if MC_AN_C == true
#	define mc_nullable
# 	define mc_nonnull
#	define mc_attribute(...)
#	if !defined(defer)
#		define defer
#	endif
#	define mc_inline
#else
#	define mc_nullable _Nullable
# 	define mc_nonnull _Nonnull
#	define mc_attribute(...) __attribute__((__VA_ARGS__))
#	define mc_inline inline __attribute__((always_inline))
#	if MC_CLANG_C == true && !defined(defer) && !defined(nocapture)
#		define MC_CCATLN_1(x, y) x##y 
#		define MC_CCATLN_2(x, y) MC_CCATLN_1(x, y)
#		define MC_CONCAT_LINE(x) MC_CCATLN_2(x, __LINE__)
#		define defer __attribute__((cleanup(mc_defer_cleanup))) void (^MC_CONCAT_LINE(_deferfn_))(void) = ^
#		define nocapture __block

mc_inline void mc_defer_cleanup(void *mc_nonnull ptr)
{
	void (^*fn)(void) = (void (^*)(void))ptr;
	(*fn)();
}

#	else
#		define defer
#		define nocapture
#	endif
#endif

#if !defined(auto)
#	define auto 		"Running in ANSI standard mode (no extensions). This macro does not automatically release the pointer!"
#else 
#	define managed_auto mc_attribute(cleanup(managed_release_ptr))
#endif

typedef void managed_Free_f(void *mc_nonnull alloc);

struct managed_PointerInformation {
	/**
	 * count: Number of used array elements.
	 * capacity: Number of allocated array elements. (Used for managed_Vector)
	 * typesize: Size of the type.
	 * reference_count: Number of references to this pointer.
	 */
	unsigned long count, capacity, typesize, reference_count;

	/**
	* Function to call on 0 references.
	*/
	managed_Free_f *mc_nonnull free;

	/**
	* Pointer to the data. 
	*/
	void *mc_nonnull data;
};

typedef void *mc_nonnull const *mc_nullable managed_Pointer_tpcp; 
typedef managed_Pointer_tpcp mptr;

/* Using a double pointer system gives the big advantage that the pointer the managed pointer represents can be changed without a complete break of the system. */
/* The downside is that the semantics are a bit weird. You must do (*ptr)[index] to access an element. */
#define mptr(T) T *mc_nonnull const *mc_nullable

/* The const will be discarded in this file. YOU can't modify the data, I will! */
static const struct managed_PointerInformation *mc_nonnull managed_info_of(managed_Pointer_tpcp ptr)
{
	/* We need to get the address of the end of the struct, then subtract it by one */
	return ((struct managed_PointerInformation *)(ptr + 1)) - 1;
}

#define mc_new(T, free) (mptr(T))managed_allocate(1, sizeof(T), free, NULL)
#define mc_array(T, i, free) (mptr(T))managed_allocate(i, sizeof(T), free, NULL)
static managed_Pointer_tpcp managed_allocate(unsigned long count, unsigned long size, managed_Free_f *mc_nullable mfree, const void *data)
{
	struct managed_PointerInformation *alloc = malloc(sizeof(struct managed_PointerInformation));
	if (alloc == NULL) return NULL;

	alloc->capacity 		= count;
	alloc->count		 	= count;
	alloc->typesize 		= size;
	alloc->reference_count 	= 1;
	alloc->free 			= mfree;

	alloc->data = calloc(count, size);
	if (alloc->data == NULL) { 
		free(alloc);
		return NULL;
	}
	if (data != NULL) memcpy(alloc->data, data, count * size);

	return &alloc->data;
}

#define mc_ref(ptr) (void *)managed_reference((managed_Pointer_tpcp)ptr)
static managed_Pointer_tpcp managed_reference(managed_Pointer_tpcp ptr)
{
	struct managed_PointerInformation *info = (struct managed_PointerInformation *)managed_info_of(ptr);

	info->reference_count++;
	return ptr;
}

#define mc_free(ptr) managed_release((managed_Pointer_tpcp)ptr)
static void managed_release(managed_Pointer_tpcp ptr)
{
	struct managed_PointerInformation *info = (struct managed_PointerInformation *)managed_info_of(ptr);
	unsigned long i;

	info->reference_count--;
	if (info->reference_count == 0) {
		if (info->free != NULL) {
			for (i = 0; i < info->count; i++) {
				info->free(&info->data[i * info->typesize]);
			}
		}
		
		free(info->data);
		free(info);
	}
}

static void managed_reallocate(managed_Pointer_tpcp ptr, size_t newc)
{
	struct managed_PointerInformation *info = (struct managed_PointerInformation *)managed_info_of(ptr);

	void *tmp = realloc;
}

/* for __attribute__((cleanup())) */
static void managed_release_ptr(void *ptr)
{ 
	managed_Pointer_tpcp *m = ptr;
	managed_release(*m); 
}

#endif
