#include "test.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char *argv[])
{
	unsigned long i = 0;
	declare_test(alloc);
	declare_test(realloc);

	struct Test tests[] = {
		alloc,
		realloc
	};

	puts(argv[1]);
	if (strcmp(argv[1], "all") == 0) {
		for (i = 0; i < (sizeof(tests) / sizeof(tests[0])); i++) {
			printf("Running test %s...\n", tests[i].name);
			if (!tests[i].test()) {
				fprintf(stderr, "Failed test \"%s\"!\n", tests[i].name);
				return EXIT_FAILURE;
			}
		}
		puts("Success!");
		return EXIT_SUCCESS;
	}

	for (i = 0; i < (sizeof(tests) / sizeof(tests[0])); i++) {
		if (strcmp(tests[i].name, argv[1]) == 0) {
			if (!tests[i].test()) {
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
