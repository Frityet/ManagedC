/**
 * @file test.h
 * @brief Utilities for testing and logging expressions
 */

#pragma once

#include "logger.h"
#include "mstring.h"
#include "mlist.h"

#include <errno.h>
extern int errno;

#define EVAL(x) x 
#define STRMAC(x) EVAL(#x)

typedef bool Test_f(void);
typedef const struct Test {
    const string    name;
    Test_f          *on_test;
} Test_t;

#define TEST(_name) static bool test_##_name(void); Test_t _name##_info = { .name = STRMAC(_name), .on_test = test_##_name }; static bool test_##_name(void)


#define TEST_COUNT __COUNTER__

#define TEST_EXPR(expr, err, ...) ({\
                                extern void exit(int);\
                                if (expr) LOG_SUCCESS("Expression \"" STRMAC(expr) "\" succeeded!");\
                                else { LOG_FATAL("Expression \"" STRMAC(expr) " failed!\n\t- " err __VA_OPT__(,) __VA_ARGS__); return false; }\
                            })

#define EXTERN_TEST(_name) ({ __COUNTER__; extern Test_t _name##_info; _name##_info; })
