/*
 * test_harness.h - about forty lines of test framework.
 *
 * Unit testing embedded C does not need a dependency. A macro that compares
 * two values, prints the file and line when they differ, and counts the
 * failures is enough to catch the bugs that matter, and it compiles anywhere a
 * C compiler exists.
 */
#ifndef TEST_HARNESS_H
#define TEST_HARNESS_H

#include <math.h>
#include <stdio.h>
#include <string.h>

static int tests_run = 0;
static int tests_failed = 0;
static const char *current_test = "";

#define TEST(name) \
    static void name(void); \
    static void run_##name(void) { current_test = #name; name(); } \
    static void name(void)

#define RUN(name) do { run_##name(); } while (0)

#define CHECK(cond) do {                                                      \
    tests_run++;                                                              \
    if (!(cond)) {                                                            \
        tests_failed++;                                                       \
        printf("  FAIL %s:%d  in %s:  %s\n",                                  \
               __FILE__, __LINE__, current_test, #cond);                      \
    }                                                                         \
} while (0)

#define CHECK_EQ_INT(a, b) do {                                               \
    tests_run++;                                                              \
    long _a = (long)(a), _b = (long)(b);                                      \
    if (_a != _b) {                                                           \
        tests_failed++;                                                       \
        printf("  FAIL %s:%d  in %s:  %s == %s  (%ld vs %ld)\n",              \
               __FILE__, __LINE__, current_test, #a, #b, _a, _b);             \
    }                                                                         \
} while (0)

#define CHECK_NEAR(a, b, tol) do {                                            \
    tests_run++;                                                              \
    double _a = (double)(a), _b = (double)(b);                                \
    if (fabs(_a - _b) > (tol)) {                                              \
        tests_failed++;                                                       \
        printf("  FAIL %s:%d  in %s:  %s ~= %s  (%g vs %g)\n",                \
               __FILE__, __LINE__, current_test, #a, #b, _a, _b);             \
    }                                                                         \
} while (0)

static int test_report(const char *suite)
{
    printf("%-22s %3d checks, %d failed\n", suite, tests_run, tests_failed);
    return tests_failed == 0 ? 0 : 1;
}

#endif /* TEST_HARNESS_H */
