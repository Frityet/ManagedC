//
// Created by Frityet on 2021-12-23.
//

#include <stdio.h>

//#define MC_NO_INLINE
//#define MC_NO_MSTRING
#include "managed.h"


int main(void)
{
    managed int *array = new(int, 100);
    //use array
    //Automatically deallocated!
}
