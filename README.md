# ManagedC

Library to add a reference-counter GC to C. See `tests/src/` for examples

> |ℹ️ Note ℹ️        |
> |-----------------|
> | This library is exclusively available on GCC and Clang due to the "`__attribute__((cleanup))`" extension! |    


## Installation

Using [xmake](https://xmake.io) you can easily use this library by including this package declaration in your `xmake.lua`: [package.lua](package.lua)

An entry into `xrepo` will be available soon.

If you do not use `xmake` then you can just copy the file `managed.h` from `src/`. `mstring.h` adds helper functions for string usage.

## Usage

### Allocation and deallocation

> |ℹ️ Note ℹ️        |
> |-----------------|
> | to change the prefix used on each function in ManagedC, define the `MC_PREFIX` macro before including!|    

To get a "managed" type, you must allocate your type with 
```c
void *mc_alloc_managed(unsigned int size, unsigned int count, void (*on_free)(void *))
````

Just as the orignal use of `auto` in BCPL was to declare that a stack allocated variable was to be deallocated once outside of the scope, `auto` in this library declares that a **heap allocated variable allocated with `alloc_managed` will be freed once outside the scope**.

For example

```c
#include "managed.h"

void func_that_allocs(void)
{
    auto int *mem = mc_alloc_managed(sizeof(int), 128, NULL);
    //...usage
   
    //Automatically deallocated!
}

int main()
{
    func_that_allocs();
    func_that_allocs();
    func_that_allocs();
    func_that_allocs();
    func_that_allocs();
    //no leaks!
}
```

You can specify a custom "deconstructor", which is a function which will run on each element of the array on deallocation

```c
#include <stdio.h>

#include "managed.h"

//We get a pointer to the value that we can free
//It is a double pointer because we have an array 
//of pointers in main
void free_int_ref(int **ref)
{
    printf("Value: %d\n", **ref);
    
    //mc_free_managed is used for deallocating "managed" types.
    //It must take in a pointer to the reference, not just the reference
    mc_free_managed(ref);
}

int main()
{
    auto int **alloc_list = mc_alloc_managed(sizeof(int *), 5, free_int_ref); 
    
    alloc_list[0] = mc_alloc_managed(sizeof(int), 1, NULL);
    *alloc_list[0] = 11;
    
    alloc_list[1] = mc_alloc_managed(sizeof(int), 1, NULL);
    *alloc_list[1] = 10;

    alloc_list[2] = mc_alloc_managed(sizeof(int), 1, NULL);
    *alloc_list[2] = 9;
    
    alloc_list[3] = mc_alloc_managed(sizeof(int), 1, NULL);
    *alloc_list[3] = 8;
    
    alloc_list[4] = mc_alloc_managed(sizeof(int), 1, NULL);
    *alloc_list[4] = 7;
    
    //All allocated at the end of the scope!
}
```

### Referencing

For the reference counter to work, assigning a reference through the regular `=` will result in no additional references being counted. To properly reference a "managed" pointer, use
```c
void *mc_reference(void *nonnull ptr)
```
or the
```c
#define ref(x) (typeof(x) *)mc_reference(x)
```
macro.

The pointer will not be freed until all the references are not in use.

```c
#include "managed.h"

int *func_that_allocs(void)
{
    auto int *mem = mc_alloc_managed(sizeof(int), 128, NULL);
    
    //Because we declared mem as auto, the memory would be deallocated at the end of the scope, so we must get a reference
    return mc_reference(mem);
    
    //equivalent to
    int *mem = mc_alloc_managed(sizeof(int), 128, NULL);
    return mem;
}

int main()
{
    auto int **refarray = mc_alloc_managed(sizeof(int *), 4, free_managed);
    int len = 0;
    {
        auto int *list = func_that_allocs();
        *list = 10;
        refarray[len++] = mc_reference(list); //Will not be deallocateed because we got a reference
    }
    //all deallocated at the end of the list
}
```

### Metadata

ManagedC keeps metadata about the pointer, meaning that you do not have to keep track of this data yourself.
The metadata is stored in this struct
```c
struct mc_PointerMetadata {
    unsigned int total_size, count, typesize, reference_count;
    void (*on_free)(void *);
    void *data;
}
```
It can be accessed by using
```c
struct mc_PointerMetadata *mc_metadataof(void *ptr);
```

As `count` is the most common field you will probably access, you can call
```c
unsigned int mc_countof(void *ptr);
```
to quickly get the count of elements in the pointer.

### TODO
- [x] Safer and better thread support   - [issue](https://github.com/Frityet/ManagedC/issues/1)
- [X] Fix for reallocation              - [issue](https://github.com/Frityet/ManagedC/issues/3)
- [X] Solution to circular references   - [issue](https://github.com/Frityet/ManagedC/issues/2)
- [ ] Solution for MSVC/ISO-C           - [issue](https://github.com/Frityet/ManagedC/issues/4)
- [X] Easier/standardized referencing   - [issue](https://github.com/Frityet/ManagedC/issues/5)
- [ ] Datastructures                    - [issue](https://github.com/Frityet/ManagedC/issues/8)
