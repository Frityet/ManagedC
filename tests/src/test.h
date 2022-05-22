/**
 * @file test.h
 * @brief Utilities for testing and logging expressions
 */

#pragma once

#include <stdarg.h>

#include "logger.h"
#include "mstring.h"
// #include "mlist.h"

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

ATTRIBUTE(used)
static inline bool test_expr(bool expr, string strexpr, int line, string file, string func, string err, ...)
{
    if (expr) { 
        char _LOG_buf[LOG_BUFFER_SIZE]; 
        snprintf(_LOG_buf, LOG_BUFFER_SIZE, "Expression \"%s\" Suceeded!\n\t- ", strexpr);
        va_list l;
        va_start(l, err);
        vsnprintf(_LOG_buf, LOG_BUFFER_SIZE, err, l);
        va_end(l);
        logbase(LOG_TYPE_SUCCESS, _LOG_buf, line, file, func);

        return true;
    } else {
        char _LOG_buf[LOG_BUFFER_SIZE]; 
        snprintf(_LOG_buf, LOG_BUFFER_SIZE, "Expression \"%s\" failed!\n\t- ", strexpr);
        va_list l;
        va_start(l, err);
        vsnprintf(_LOG_buf, LOG_BUFFER_SIZE, err, l);
        va_end(l);
        logbase(LOG_TYPE_FATAL, _LOG_buf, line, file, func);

        return false;
    }
}

#define TEST_EXPR(expr, err, ...) test_expr((expr), STRMAC(expr), __LINE__, __FILE__, (char *)__PRETTY_FUNCTION__, err, __VA_ARGS__)\
        

#define EXTERN_TEST(_name) ({ (void)__COUNTER__; extern Test_t _name##_info; _name##_info; })
