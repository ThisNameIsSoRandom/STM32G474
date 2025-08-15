/**
 * @file rtt_test_listener.h
 * @brief Google Test event listener for SEGGER RTT output
 * 
 * This custom test event listener redirects all Google Test output
 * to SEGGER RTT, enabling test result visualization on embedded
 * systems without a serial console.
 */

#ifndef RTT_TEST_LISTENER_H
#define RTT_TEST_LISTENER_H

#include "../runner/gtest_minimal.h"
#include "SEGGER_RTT.h"

/**
 * @class RTTTestListener
 * @brief Custom Google Test event listener for RTT output
 * 
 * This listener implements the TestEventListener interface to
 * capture test events and format them for RTT output. It provides
 * colored output (if terminal supports it) and detailed test results.
 * 
 * Output Format:
 * - Green [PASS] for successful tests
 * - Red [FAIL] for failed tests  
 * - Yellow [SKIP] for disabled tests
 * - Timing information for each test
 * - Failure messages with file and line information
 */
class RTTTestListener : public testing::TestEventListener {
public:
    RTTTestListener() : test_count_(0), passed_count_(0), failed_count_(0) {}
    
    // Test program lifecycle events
    void OnTestProgramStart(const testing::UnitTest& unit_test) override;
    void OnTestProgramEnd(const testing::UnitTest& unit_test) override;
    
    // Test suite (formerly test case) events
    void OnTestSuiteStart(const testing::TestSuite& test_suite) override;
    void OnTestSuiteEnd(const testing::TestSuite& test_suite) override;
    
    // Individual test events
    void OnTestStart(const testing::TestInfo& test_info) override;
    void OnTestEnd(const testing::TestInfo& test_info) override;
    void OnTestPartResult(const testing::TestPartResult& result) override;
    
    // Test iteration events (for repeated tests)
    void OnTestIterationStart(const testing::UnitTest& unit_test, int iteration) override;
    void OnTestIterationEnd(const testing::UnitTest& unit_test, int iteration) override;
    
    // Environment setup/teardown events
    void OnEnvironmentsSetUpStart(const testing::UnitTest& unit_test) override;
    void OnEnvironmentsSetUpEnd(const testing::UnitTest& unit_test) override;
    void OnEnvironmentsTearDownStart(const testing::UnitTest& unit_test) override;
    void OnEnvironmentsTearDownEnd(const testing::UnitTest& unit_test) override;
    
private:
    int test_count_;
    int passed_count_;
    int failed_count_;
    
    // Helper function to print colored output
    void PrintColored(const char* color_code, const char* format, ...);
};

#endif // RTT_TEST_LISTENER_H