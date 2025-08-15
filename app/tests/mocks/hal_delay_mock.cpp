/**
 * @file hal_delay_mock.cpp
 * @brief Mock implementation of HAL delay functions for testing
 */

#include "hal_types.h"
#include "SEGGER_RTT.h"

extern "C" {

/**
 * Mock implementation of HAL_Delay_MS for testing
 * In tests, we don't want real delays, so this is a no-op with logging
 */
void HAL_Delay_MS(uint32_t Delay) {
    // Log the delay request but don't actually delay
    SEGGER_RTT_printf(0, "HAL_Delay_MS: %lu ms (mocked)\n", Delay);
}

/**
 * Mock implementation of HAL_Delay (if used)
 */
void HAL_Delay(uint32_t Delay) {
    SEGGER_RTT_printf(0, "HAL_Delay: %lu ms (mocked)\n", Delay);
}

} // extern "C"