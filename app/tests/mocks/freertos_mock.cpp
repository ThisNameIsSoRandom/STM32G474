/**
 * @file freertos_mock.cpp
 * @brief Mock implementation of FreeRTOS functions for testing
 */

#include "freertos_types.h"
#include "SEGGER_RTT.h"

extern "C" {

/**
 * Mock implementation of vTaskDelay
 * In tests, we don't want real delays, so this is a no-op with logging
 */
void vTaskDelay(TickType_t xTicksToDelay) {
    SEGGER_RTT_printf(0, "vTaskDelay: %lu ticks (mocked)\n", xTicksToDelay);
}

/**
 * Mock implementation of vTaskDelete  
 * Note: This mock is only used when running tests off-target.
 * On-target, the real FreeRTOS vTaskDelete is used.
 */
#ifndef USE_FREERTOS
void vTaskDelete(TaskHandle_t xTaskToDelete) {
    SEGGER_RTT_printf(0, "vTaskDelete: %p (mocked)\n", xTaskToDelete);
}
#endif

} // extern "C"