# ManagedC

Library to add a reference counter to C. See [`tests/src/`](tests/src) for examples

> |ℹ️ Note ℹ️        |
> |-----------------|
> | If you have GCC extensions enabled, you get access to more features, i.e the `auto` macro for automatic releasing! |    


# Installation

Using [xmake](https://xmake.io) you can easily use this library by adding `managedc` to your packages

If you do not use `xmake` then you can just copy the file [`managed.h`](src/managed.h) from [`src/`](src/). [`mstring.h`](src/managed/mstring.h) adds helper functions for string usage.

# Usage

## Allocation and deallocation

To get a "managed" type, you must allocate your type with 
```c
static void *mc_nullable managed_allocate(size_t count, size_t typesize, void (*mc_nullable)(void *mc_nonnull) free, const void *mc_nullable data)
````
or
```c
#define mc_alloc(T, free) (T *)managed_allocate(1, sizeof(T), (managed_Free_f *)free, NULL)
```
or, for arrays
```c
#define mc_array(T, count, free) (T *)managed_allocate(count, sizeof(T), (managed_Free_f *)(free), NULL)
```

and you can release your reference using
```c
static void managed_release(const void *mc_nonnull ptr)
```
or
```c
#define mc_free(ptr) managed_release(ptr)
```

You can specify a custom "deconstructor", which is a function that will run on each element of the array on deallocation

```c
#include <stdio.h>

#include "managed.h"

//We get a pointer to the value that we can free
//It is a double pointer because we have an array 
//of pointers in main
void free_int_ref(int **ref)
{
    //This function will be called for each item in the array
    printf("Value: %d\n", **ref);
    
    //mc_free is used for deallocating "managed" types.
    mc_free(ref);
}

int main(void)
{
    int **alloc_list = mc_array(int *, 5, &free_int_ref); 
    
    alloc_list[0] = mc_alloc(int, NULL);
    *alloc_list[0] = 11;
    
    alloc_list[1] = mc_alloc(int, NULL);
    *alloc_list[1] = 10;

    alloc_list[2] = mc_alloc(int, NULL);
    *alloc_list[2] = 9;
    
    alloc_list[3] = mc_alloc(int, NULL);
    *alloc_list[3] = 8;
    
    alloc_list[4] = mc_alloc(int, NULL);
    *alloc_list[4] = 7;
    
    mc_free(alloc_list);
}
```

## Referencing

For the reference counter to work, assigning a reference through the regular `=` will result in no additional references being counted, which is useful for "weak" references. To make a strong reference to a "managed" pointer, use
```c
static void *managed_reference(const void *mc_nonnull ptr)
```
or the
```c
#define mc_ref(ptr) MC_EXPAND((mc_typeof(ptr)))managed_reference(ptr)
```
macro.

The pointer will not be freed until all the references are not in use.

```c
#include "managed.h"

int *func_that_allocs(void)
{
    int *mem = managed_alloc(sizeof(int), 128, NULL, NULL);

    return mem;
}

int main(void)
{
    //You can pass in managed_release (not mc_free) as a param, and it will run on the value of the pointer, which must also be a managed pointer
    int **refarray = mc_alloc(int *, managed_release);

    {
        int *array = func_that_allocs();
        *array = 10;
        refarray[0] = mc_reference(array); //Will not be deallocateed because we got a reference
        mc_free(array);
	}
	
	mc_free(refarray);
}
```

## Metadata

ManagedC keeps metadata about the pointer, meaning that you do not have to keep track of this data yourself.
The metadata is stored in this struct
```c
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
    managed_Free_f *mc_nullable free;

    /**
    * Pointer to the data, should be just in front of data.
    */
    void *mc_nonnull data;

#if MC_MUTEX
    /**
    * Lock on the metadata, maps to an OS specific object.
    */
    mc_Mutex_t lock;
#endif
};
```
It can be accessed by using
```c
const struct managed_PointerInfo *mc_nullable managed_info_of(const void *mc_nonnull ptr)
```

As `count` is the most common field you will probably access, you can call
```c
#define mc_countof(ptr) (managed_info_of(ptr)->count)
```
to quickly get the count of elements in the allocations.

### Extensions - GCC/Clang

Just as the original use of `auto` in BCPL was to declare that a stack-allocated variable was to be deallocated once outside of the scope, `mc_auto` in this library declares that a **heap allocated variable allocated with `managed_allocate` will be freed once outside the scope**.

For example

```c
#include "managed.h"

void func_that_allocs(void)
{
    mc_auto int *mem = mc_alloc(int, NULL);
    //...usage
   
    //Automatically deallocated!
}

int main(void)
{
    func_that_allocs();
    func_that_allocs();
    func_that_allocs();
    func_that_allocs();
    func_that_allocs();
    //no leaks!
}
```

### Extensions - Clang

If you use clang, you can use `mc_defer` to make code run once out of scope. This requires `-fBlocks` and you may have to link with `libBlocksRuntime`.
```c
int main(void)
{
    FILE *fp = fopen("hi.txt", "rb");
    mc_defer { fclose(fp); };
    //...usage
    //Automatically closed!
}
```

## Datastructures

ManagedC also comes with reference counted common data structures, with the list still growing
You can see them [here](src/managed/)

### TODO
- [x] Safer and better thread support   - [issue](https://github.com/Frityet/ManagedC/issues/1)
- [X] Fix for reallocation              - [issue](https://github.com/Frityet/ManagedC/issues/3)
- [X] A solution to circular references - [issue](https://github.com/Frityet/ManagedC/issues/2)
- [X] Solution for MSVC/ISO-C           - [issue](https://github.com/Frityet/ManagedC/issues/4)
- [X] Easier/standardized referencing   - [issue](https://github.com/Frityet/ManagedC/issues/5)
- [X] Data structures                   - [issue](https://github.com/Frityet/ManagedC/issues/8)

- [ ] Even more data structures!        - [issue](https://github.com/Frityet/ManagedC/issues/10)
