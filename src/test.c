//
// Created by Frityet on 2021-12-23.
//

#include <stdio.h>

#include "mstring.h"

void print_array(string strs[])
{
    for (int i = 0; i < lengthof(strs); i++) {
        let auto str = ref(strs[i]);
        puts(strs[i]);
    }
}

int main(int argc, string argv[])
{
    auto var args = new(string, argc, free_managed);

    for (int i = 0; i < argc; i++) {
        args[i] = mstr(argv[i]);
    }

    for (int i = 0; i < lengthof(args); i++) {
        puts(args[i]);
    }
}
