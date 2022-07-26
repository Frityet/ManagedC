#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, const char *argv[])
{
	unsigned long int i = 0;
	struct Test tests[4];

	extern struct Test TESTNAME(alloc), TESTNAME(realloc), TESTNAME(list), TESTNAME(string);
	tests[0] = TESTNAME(alloc);
	tests[1] = TESTNAME(realloc);
	tests[2] = TESTNAME(list);
	tests[3] = TESTNAME(string);

	if (strcmp(argv[1], "all") == 0 || argc < 1) {
		for (i = 0; i < (sizeof(tests) / sizeof(tests[0])); i++) {
			printf("Running test %s...\n", tests[i].name);
			if (tests[i].test() == failure) {
				fprintf(stderr, "Failed test \"%s\"!\n", tests[i].name);
				return EXIT_FAILURE;
			}
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
