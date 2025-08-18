/**
 * @file rtt_test_listener.cpp
 * @brief Implementation of SEGGER RTT test event listener
 */

#include "rtt_test_listener.h"
#include <cstdarg>
#include <cstdio>

// ANSI color codes for RTT output (if terminal supports them)
#define RTT_COLOR_GREEN   "\\033[32m"
#define RTT_COLOR_RED     "\\033[31m" 
#define RTT_COLOR_YELLOW  "\\033[33m"
#define RTT_COLOR_BLUE    "\\033[34m"
#define RTT_COLOR_RESET   "\\033[0m"

void RTTTestListener::OnTestProgramStart(const testing::UnitTest& unit_test) {
    SEGGER_RTT_printf(0, "Running %d test(s)\n", unit_test.total_test_count());
}

void RTTTestListener::OnTestProgramEnd(const testing::UnitTest& unit_test) {
    SEGGER_RTT_printf(0, "\n");
    SEGGER_RTT_printf(0, "========== FINAL RESULTS ==========\n");
    
    if (unit_test.failed_test_count() == 0) {
        PrintColored(RTT_COLOR_GREEN, "[PASSED] All %d test(s)\n", unit_test.total_test_count());
    } else {
        PrintColored(RTT_COLOR_RED, "[FAILED] %d of %d test(s)\n", 
                    unit_test.failed_test_count(), unit_test.total_test_count());
        PrintColored(RTT_COLOR_GREEN, "[PASSED] %d test(s)\n", unit_test.successful_test_count());
    }
    
    SEGGER_RTT_printf(0, "==================================\n");
}

void RTTTestListener::OnTestSuiteStart(const testing::TestSuite& test_suite) {
    SEGGER_RTT_printf(0, "\n");
    PrintColored(RTT_COLOR_BLUE, "[ RUN      ] %s\n", test_suite.name());
}

void RTTTestListener::OnTestSuiteEnd(const testing::TestSuite& test_suite) {
    SEGGER_RTT_printf(0, "[ SUITE    ] %s (%d/%d passed)\n", 
                     test_suite.name(),
                     test_suite.successful_test_count(),
                     test_suite.test_to_run_count());
}

void RTTTestListener::OnTestStart(const testing::TestInfo& test_info) {
    SEGGER_RTT_printf(0, "[ RUN      ] %s.%s\n", 
                     test_info.test_suite_name(), 
                     test_info.name());
}

void RTTTestListener::OnTestEnd(const testing::TestInfo& test_info) {
    if (test_info.result() && test_info.result()->failed()) {
        PrintColored(RTT_COLOR_RED, "[  FAILED  ] %s.%s\n",
                    test_info.test_suite_name(), test_info.name());
        failed_count_++;
    } else {
        PrintColored(RTT_COLOR_GREEN, "[       OK ] %s.%s\n",
                    test_info.test_suite_name(), test_info.name());
        passed_count_++;
    }
    test_count_++;
}

void RTTTestListener::OnTestPartResult(const testing::TestPartResult& result) {
    if (result.failed()) {
        PrintColored(RTT_COLOR_RED, "FAILURE: %s:%d\n", 
                    result.file_name(), result.line_number());
        SEGGER_RTT_printf(0, "  %s\n", result.message());
    }
}

void RTTTestListener::OnTestIterationStart(const testing::UnitTest& unit_test, int iteration) {
    if (iteration > 0) {
        SEGGER_RTT_printf(0, "\nRepeating all tests (iteration %d)\n", iteration + 1);
    }
}

void RTTTestListener::OnTestIterationEnd(const testing::UnitTest& unit_test, int iteration) {
    (void)unit_test; (void)iteration; // Unused
}

void RTTTestListener::OnEnvironmentsSetUpStart(const testing::UnitTest& unit_test) {
    (void)unit_test; // Unused
    SEGGER_RTT_printf(0, "Global test environment set-up\n");
}

void RTTTestListener::OnEnvironmentsSetUpEnd(const testing::UnitTest& unit_test) {
    (void)unit_test; // Unused
}

void RTTTestListener::OnEnvironmentsTearDownStart(const testing::UnitTest& unit_test) {
    (void)unit_test; // Unused
    SEGGER_RTT_printf(0, "Global test environment tear-down\n");
}

void RTTTestListener::OnEnvironmentsTearDownEnd(const testing::UnitTest& unit_test) {
    (void)unit_test; // Unused
}

void RTTTestListener::PrintColored(const char* color_code, const char* format, ...) {
    // Print color code
    SEGGER_RTT_WriteString(0, color_code);
    
    // Format and print message
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    SEGGER_RTT_WriteString(0, buffer);
    
    // Reset color
    SEGGER_RTT_WriteString(0, RTT_COLOR_RESET);
}