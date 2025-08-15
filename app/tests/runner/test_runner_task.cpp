/**
 * @file test_runner_task.cpp
 * @brief FreeRTOS task for running Google Test suite on embedded target
 * 
 * This task initializes Google Test, configures output for SEGGER RTT,
 * and executes all registered unit tests. Results are displayed via RTT
 * for debugging without requiring a serial console.
 * 
 * ## Integration:
 * Platform code creates this task similar to other application tasks:
 * ```cpp
 * #ifdef ENABLE_UNIT_TESTS
 * TaskHandle_t testTaskHandle = NULL;
 * xTaskCreate(runTestsTask, "Tests", 2048, NULL, 
 *             tskIDLE_PRIORITY + 1, &testTaskHandle);
 * #endif
 * ```
 */

#include "freertos_types.h"
#include "SEGGER_RTT.h"
#include "gtest_minimal.h"  // Minimal GTest implementation for embedded
#include "../listeners/rtt_test_listener.h"

/**
 * @brief Main test runner task for FreeRTOS
 * @param pvParameters Task parameters (unused)
 * 
 * This task performs the following sequence:
 * 1. Initialize Google Test framework
 * 2. Register custom RTT output listener
 * 3. Execute all registered tests
 * 4. Report results via RTT
 * 5. Self-delete after completion
 * 
 * Stack requirements: 2048 words minimum (GTest overhead + test execution)
 */
extern "C" void runTestsTask(void* pvParameters) {
    (void)pvParameters;
    
    // Announce test execution start
    SEGGER_RTT_printf(0, "\n");
    SEGGER_RTT_printf(0, "========================================\n");
    SEGGER_RTT_printf(0, "    UNIT TEST SUITE - GOOGLE TEST\n");
    SEGGER_RTT_printf(0, "========================================\n");
    SEGGER_RTT_printf(0, "Platform: STM32 with FreeRTOS\n");
    SEGGER_RTT_printf(0, "Output: SEGGER RTT\n");
    SEGGER_RTT_printf(0, "\n");
    
    // Initialize Google Test without command line arguments
    int argc = 1;
    char* argv[] = {(char*)"embedded_tests"};
    testing::InitGoogleTest(&argc, argv);
    
    // Get the default test event listener
    testing::TestEventListeners* listeners = 
        testing::UnitTest::GetInstance()->listeners();
    
    // Remove default console output (not available on embedded)
    delete listeners->Release(listeners->default_result_printer());
    
    // Add custom RTT listener for test output
    listeners->Append(new RTTTestListener());
    
    // Run all tests
    SEGGER_RTT_printf(0, "Starting test execution...\n");
    SEGGER_RTT_printf(0, "----------------------------------------\n");
    
    int result = RUN_ALL_TESTS();
    
    // Report final results
    SEGGER_RTT_printf(0, "----------------------------------------\n");
    SEGGER_RTT_printf(0, "Test execution complete\n");
    
    if (result == 0) {
        SEGGER_RTT_printf(0, "Result: ALL TESTS PASSED\n");
    } else {
        SEGGER_RTT_printf(0, "Result: %d TEST(S) FAILED\n", result);
    }
    
    SEGGER_RTT_printf(0, "========================================\n");
    SEGGER_RTT_printf(0, "\n");
    
    // Optional: Keep task alive for debugging
    #ifdef KEEP_TEST_TASK_ALIVE
    while(1) {
        vTaskDelay(10000);  // Sleep forever
    }
    #else
    // Self-delete after test completion
    vTaskDelete(NULL);
    #endif
}

/**
 * @brief C-compatible entry point for test task
 * 
 * This is a simplified interface - the task is created directly
 * by main() which calls runTestsTask().
 */
extern "C" void startUnitTests(void) {
    // This function is no longer needed since main() creates the task directly
    SEGGER_RTT_printf(0, "Unit tests will be started by main task creation\n");
}