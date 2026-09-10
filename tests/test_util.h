#ifndef SN_TEST_UTIL_H
#define SN_TEST_UTIL_H

/* 轻量断言工具：无需第三方依赖。
 * 每个 test_*.c 单独成可执行文件，include 本头文件后使用 CHECK 系列宏，
 * 最后调用 test_summary("模块名") 汇总并作为退出码。 */

#include <stdio.h>
#include <math.h>

static int g_tests_run = 0;
static int g_tests_failed = 0;

#define CHECK(cond) \
    do { g_tests_run++; \
         if (!(cond)) { g_tests_failed++; \
             printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); } \
    } while (0)

#define CHECK_EQ_INT(a, b) \
    do { long long _a = (long long)(a), _b = (long long)(b); g_tests_run++; \
         if (_a != _b) { g_tests_failed++; \
             printf("  FAIL %s:%d: %s == %s  (%lld != %lld)\n", \
                    __FILE__, __LINE__, #a, #b, _a, _b); } \
    } while (0)

#define CHECK_CLOSE(a, b, eps) \
    do { double _a = (double)(a), _b = (double)(b), _e = (double)(eps); g_tests_run++; \
         if (fabs(_a - _b) > _e) { g_tests_failed++; \
             printf("  FAIL %s:%d: %s ≈ %s  (%.6f vs %.6f, eps %.3g)\n", \
                    __FILE__, __LINE__, #a, #b, _a, _b, _e); } \
    } while (0)

static int test_summary(const char *name) {
    printf("[%s] %d checks, %d failed\n", name, g_tests_run, g_tests_failed);
    return g_tests_failed == 0 ? 0 : 1;
}

#endif /* SN_TEST_UTIL_H */