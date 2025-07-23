#include "test_runner.h"
#include "SEGGER_RTT.h"

namespace UnitTests {

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

void TestRunner::begin(const char* suite_name) {
    SEGGER_RTT_printf(0, "\n=== STARTING TEST SUITE: %s ===\n", suite_name);
    tests_run = 0;
    tests_passed = 0;
    tests_failed = 0;
}

void TestRunner::assert_true(bool condition, const char* test_name, const char* message) {
    tests_run++;
    if (condition) {
        tests_passed++;
        SEGGER_RTT_printf(0, "[PASS] %s\n", test_name);
    } else {
        tests_failed++;
        SEGGER_RTT_printf(0, "[FAIL] %s: %s\n", test_name, message);
    }
}

void TestRunner::assert_equal(int expected, int actual, const char* test_name) {
    tests_run++;
    if (expected == actual) {
        tests_passed++;
        SEGGER_RTT_printf(0, "[PASS] %s\n", test_name);
    } else {
        tests_failed++;
        SEGGER_RTT_printf(0, "[FAIL] %s: expected %d, got %d\n", test_name, expected, actual);
    }
}

void TestRunner::assert_equal(uint16_t expected, uint16_t actual, const char* test_name) {
    tests_run++;
    if (expected == actual) {
        tests_passed++;
        SEGGER_RTT_printf(0, "[PASS] %s\n", test_name);
    } else {
        tests_failed++;
        SEGGER_RTT_printf(0, "[FAIL] %s: expected %u, got %u\n", test_name, expected, actual);
    }
}

void TestRunner::assert_not_null(const void* ptr, const char* test_name) {
    tests_run++;
    if (ptr != nullptr) {
        tests_passed++;
        SEGGER_RTT_printf(0, "[PASS] %s\n", test_name);
    } else {
        tests_failed++;
        SEGGER_RTT_printf(0, "[FAIL] %s: pointer is null\n", test_name);
    }
}

void TestRunner::end() {
    SEGGER_RTT_printf(0, "\n=== TEST RESULTS ===\n");
    SEGGER_RTT_printf(0, "Total: %d, Passed: %d, Failed: %d\n", tests_run, tests_passed, tests_failed);
    if (tests_failed == 0) {
        SEGGER_RTT_printf(0, "ALL TESTS PASSED!\n");
    } else {
        SEGGER_RTT_printf(0, "SOME TESTS FAILED!\n");
    }
    SEGGER_RTT_printf(0, "===================\n\n");
}

int TestRunner::getFailedCount() {
    return tests_failed;
}

} // namespace UnitTests

// C interface for main test runner
extern "C" {

// Forward declare test functions
void test_bq40z80_basic(void);
void test_bq40z80_advanced(void);

void runAllTests(void) {
    SEGGER_RTT_printf(0, "\n");
    SEGGER_RTT_printf(0, "************************************************\n");
    SEGGER_RTT_printf(0, "*          APP LIBRARY UNIT TESTS             *\n");
    SEGGER_RTT_printf(0, "************************************************\n");
    
    int totalFailures = 0;
    
    // Run BQ40Z80 tests
    test_bq40z80_basic();
    totalFailures += UnitTests::TestRunner::getFailedCount();
    
    test_bq40z80_advanced();
    totalFailures += UnitTests::TestRunner::getFailedCount();
    
    // Final summary
    SEGGER_RTT_printf(0, "\n");
    SEGGER_RTT_printf(0, "************************************************\n");
    SEGGER_RTT_printf(0, "*              FINAL RESULTS                   *\n");
    SEGGER_RTT_printf(0, "************************************************\n");
    
    if (totalFailures == 0) {
        SEGGER_RTT_printf(0, "🎉 ALL TESTS PASSED! 🎉\n");
    } else {
        SEGGER_RTT_printf(0, "❌ %d TEST(S) FAILED!\n", totalFailures);
    }
    
    SEGGER_RTT_printf(0, "************************************************\n");
}

}