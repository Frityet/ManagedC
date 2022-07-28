#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, const char *argv[])
{
	size_t i = 0;
	struct Test tests[5];

	extern struct Test TESTNAME(alloc), TESTNAME(realloc), TESTNAME(list), TESTNAME(string), TESTNAME(linkedlist);
	tests[0] = TESTNAME(alloc);
	tests[1] = TESTNAME(realloc);
	tests[2] = TESTNAME(list);
	tests[3] = TESTNAME(string);
	tests[4] = TESTNAME(linkedlist);

	if (strcmp(argv[1], "all") == 0 || argc < 1) {
		for (i = 0; i < (sizeof(tests) / sizeof(tests[0])); i++) {
			printf("Running test %s...", tests[i].name);
			if (tests[i].test() == failure) {
				fprintf(stderr, "\x1b[31m[Fail]\x1b[0m\n");
				return EXIT_FAILURE;
			}
			printf("\x1b[32m[Success]\x1b[0m\n");
		}
		puts("Success!");
		return EXIT_SUCCESS;
	}

	for (i = 0; i < (sizeof(tests) / sizeof(tests[0])); i++) {
		if (strcmp(tests[i].name, argv[1]) == 0) {
			if (tests[i].test() == failure) {
				fprintf(stderr, "Failed test \"%s\"!\n", tests[i].name);
				return EXIT_FAILURE;
			}
			puts("Success!");
			return EXIT_SUCCESS;
		}
	}

	puts("Success!");
	return EXIT_SUCCESS;
}
