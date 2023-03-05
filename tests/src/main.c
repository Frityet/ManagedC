#include "test.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



int main(void)
{
    size_t i = 0, failc = 0;
    struct Test tests[16];

    extern struct Test TESTNAME(alloc), TESTNAME(realloc), TESTNAME(list), TESTNAME(string), TESTNAME(linkedlist), TESTNAME(mcauto), TESTNAME(mcdefer), TESTNAME(destructor), TESTNAME(threads);
    tests[__COUNTER__] = TESTNAME(alloc);
    tests[__COUNTER__] = TESTNAME(realloc);
    tests[__COUNTER__] = TESTNAME(string);
    /*tests[__COUNTER__] = TESTNAME(linkedlist);*/
    tests[__COUNTER__] = TESTNAME(mcauto);
    tests[__COUNTER__] = TESTNAME(mcdefer);
    tests[__COUNTER__] = TESTNAME(list);
    tests[__COUNTER__] = TESTNAME(destructor);
    tests[__COUNTER__] = TESTNAME(threads);

    printf("Running %d tests\n", __COUNTER__);

    for (i = 0; i < __COUNTER__ - 1; i++) {
        size_t charc = (size_t)printf("Running test %s...", tests[i].name);
        putchar('\t');
        if (charc <= sizeof("Running test destructor...") - 2) putchar('\t');
        bool res = tests[i].test();
        if (res == failure) {
            failc++;
            fprintf(stderr, "\x1b[31m[Fail]\x1b[0m\n");
        } else printf("\x1b[32m[Success]\x1b[0m\n");
    }

    if (failc > 0) {
        fprintf(stderr, "\x1b[31mFailed %ld tests!\x1b[0m\n", failc);
        return EXIT_FAILURE;
    }
    printf("\x1b[32mSuccessfully ran all tests!\x1b[0m\n");
}
