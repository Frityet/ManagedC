#if !defined(MANAGED_TEST)
#define MANAGED_TEST

#include <stdio.h>
#include <stdlib.h>

#include "managed/mstring.h"
#include "managed/mlist.h"
#include "managed/mlinkedlist.h"

#undef bool
#undef true
#undef false

typedef enum Boolean { true = 1, false = 0, success = 1, failure = 0 } bool;

typedef bool Test_f(void);

struct Test {
	const char *name;
	Test_f *test;
};

#define declaretest(t) static bool _##t##_test(void); const struct Test __test_##t = { #t, _##t##_test }; static bool _##t##_test(void)
#define TESTNAME(t) __test_##t

static bool assert(bool expr, const char *err, const char *strexpr, const char *file, int line)
{
	if (!expr) {
		fprintf(stderr, "Assertion { %s } failed located at [%s:%d]\nError: %s\n", strexpr, file, line, err);
		return false;
	}
	return true;
}


#define ASSERT(expr, msg) do { if (!assert((expr), msg, #expr, __FILE__, __LINE__)) return 0; } while (0)

#endif
