/**
 * @file test_runner_task.h
 * @brief Header for FreeRTOS unit test task integration
 * 
 * Provides C-compatible interface for integrating the Google Test
 * runner with platform code. The test task can be conditionally
 * compiled based on ENABLE_UNIT_TESTS definition.
 */

#ifndef TEST_RUNNER_TASK_H
#define TEST_RUNNER_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ENABLE_UNIT_TESTS

/**
 * @brief FreeRTOS task function for running unit tests
 * @param pvParameters Task parameters (unused)
 * 
 * This function runs all registered Google Test cases and outputs
 * results via SEGGER RTT. The task self-deletes after completion.
 * 
 * Task requirements:
 * - Stack: 2048 words minimum
 * - Priority: tskIDLE_PRIORITY + 1 (run before application tasks)
 */
void runTestsTask(void* pvParameters);

/**
 * @brief Create and start the unit test task
 * 
 * Convenience function that creates the test task with appropriate
 * parameters. Call this from main() when ENABLE_UNIT_TESTS is defined.
 * 
 * Example usage:
 * ```c
 * #ifdef ENABLE_UNIT_TESTS
 * startUnitTests();
 * #endif
 * ```
 */
void startUnitTests(void);

#else

// Empty implementations when tests are disabled
static inline void startUnitTests(void) { }

#endif // ENABLE_UNIT_TESTS

#ifdef __cplusplus
}
#endif

#endif // TEST_RUNNER_TASK_H