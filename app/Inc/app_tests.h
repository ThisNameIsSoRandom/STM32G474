#ifndef APP_TESTS_H
#define APP_TESTS_H

#ifdef __cplusplus
extern "C" {
#endif

// Test function declarations
void test_bq40z80_basic(void);
void test_bq40z80_advanced(void);

// Main test runner function
void runAllTests(void);

#ifdef __cplusplus
}
#endif

#endif // APP_TESTS_H