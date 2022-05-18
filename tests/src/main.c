/**
 * @file main.c
 * @brief Entry point for testing the library.
 */

#include "test.h"

int main()
{
    const Test_t TESTS[] = {
        EXTERN_TEST(variable),
        EXTERN_TEST(customdtor),
        EXTERN_TEST(refcount),
        EXTERN_TEST(list),
        EXTERN_TEST(ptrlist),
        EXTERN_TEST(thread),
        // EXTERN_TEST(circularreference)
    };
    const int TESTC = TEST_COUNT;

    LOG_INFO("%d tests to execute!", TESTC);

    for (int i = 0; i < TESTC; i++) {
        Test_t test = TESTS[i];

        LOG_INFO("Executing test \"%s\" (%d/%d)", test.name, i, TESTC);
        if (test.on_test()) 
            LOG_SUCCESS("{Test: \"%s\"} - Success!", test.name);
        else
            LOG_ERROR("{Test: \"%s\"} - Error", test.name);
    }

    LOG_SUCCESS("\x1b[32;5;88mDone!");
    return EXIT_SUCCESS;
}
