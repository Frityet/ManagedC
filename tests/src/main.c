#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, const char *argv[])
{
	size_t i = 0, failc = 0;
	struct Test tests[128];
	const char *opt = NULL;

	extern struct Test TESTNAME(alloc), TESTNAME(realloc), TESTNAME(list), TESTNAME(string), TESTNAME(linkedlist), TESTNAME(mcauto), TESTNAME(mcdefer);
	tests[__COUNTER__] = TESTNAME(alloc);
    tests[__COUNTER__] = TESTNAME(realloc);
    tests[__COUNTER__] = TESTNAME(string);
    tests[__COUNTER__] = TESTNAME(linkedlist);
    tests[__COUNTER__] = TESTNAME(mcauto);
    tests[__COUNTER__] = TESTNAME(mcdefer);
    tests[__COUNTER__] = TESTNAME(list);

    opt = argc < 1 ? "all" : argv[1];
	if (strcmp(opt, "all") == 0) {
		for (i = 0; i < __COUNTER__; i++) {
			printf("Running test %s...", tests[i].name);
			if (tests[i].test() == failure) failc++, fprintf(stderr, "\t\t\x1b[31m[Fail]\x1b[0m\n");
			else printf("\t\t\x1b[32m[Success]\x1b[0m\n");
		}
	}

	if (failc > 0) {
        fprintf(stderr, "\x1b[31mFailed %zu tests!\x1b[0m\n", failc);
        return EXIT_FAILURE;
    }
    printf("\x1b[32mSuccessfully ran all tests!\x1b[0m\n");
}
