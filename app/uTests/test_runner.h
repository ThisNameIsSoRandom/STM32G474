#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include <cstdint>

namespace UnitTests {

class TestRunner {
public:
    // Start a test suite
    static void begin(const char* suite_name);
    
    // Basic assertions
    static void assert_true(bool condition, const char* test_name, const char* message = "condition is false");
    static void assert_equal(int expected, int actual, const char* test_name);
    static void assert_equal(uint16_t expected, uint16_t actual, const char* test_name);
    static void assert_not_null(const void* ptr, const char* test_name);
    
    // End test suite and show results
    static void end();
    
    // Get test results
    static int getFailedCount();
};

// Convenience macros
#define TEST_ASSERT(condition, name) UnitTests::TestRunner::assert_true((condition), (name))
#define TEST_ASSERT_EQUAL(expected, actual, name) UnitTests::TestRunner::assert_equal((expected), (actual), (name))
#define TEST_ASSERT_NOT_NULL(ptr, name) UnitTests::TestRunner::assert_not_null((ptr), (name))

} // namespace UnitTests

#endif // TEST_RUNNER_H