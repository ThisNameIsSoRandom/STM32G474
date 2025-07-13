/**
 ******************************************************************************
 * @file           : hal_implementations.cpp  
 * @brief          : HAL function weak implementations for Tasks
 ******************************************************************************
 */

#include "hal_types.h"
#include "SEGGER_RTT.h"
#define __weak __attribute__((used))  __attribute__((weak))

extern "C" {

// ===============================
// Weak HAL implementations for Tasks library
// ===============================

/**
 * @brief Weak implementation of HAL Delay MS
 */
 __weak void HAL_Delay_MS(uint32_t ms)
{
    SEGGER_RTT_printf(0, "WARNING: HAL_Delay_MS(%u) not implemented by platform - no delay applied\n\r", (unsigned int)ms);
    // No delay in hollow implementation
}

/**
 * @brief Weak implementation of SMBus Master Transmit IT
 */
__weak HAL_StatusTypeDef HAL_SMBUS_Master_Transmit_IT(SMBUS_HandleTypeDef *hsmbus, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t XferOptions)
{
    (void)hsmbus;
    (void)DevAddress;
    (void)pData;
    (void)Size;
    (void)XferOptions;
    SEGGER_RTT_printf(0, "WARNING: HAL_SMBUS_Master_Transmit_IT not implemented by platform\n\r");
    return HAL_OK;
}

/**
 * @brief Weak implementation of UART Transmit IT
 */
__weak HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    (void)huart;
    (void)pData;
    (void)Size;
    SEGGER_RTT_printf(0, "WARNING: HAL_UART_Transmit_IT not implemented by platform\n\r");
    return HAL_OK;
}

/**
 * @brief Weak implementation of UART Receive IT
 */
__weak HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    (void)huart;
    (void)pData;
    (void)Size;
    SEGGER_RTT_printf(0, "WARNING: HAL_UART_Receive_IT not implemented by platform\n\r");
    return HAL_OK;
}

/**
 * @brief Weak implementation of UART Get State
 */
__weak HAL_UART_StateTypeDef HAL_UART_GetState(UART_HandleTypeDef *huart)
{
    (void)huart;
    SEGGER_RTT_printf(0, "WARNING: HAL_UART_GetState not implemented by platform\n\r");
    return HAL_UART_STATE_READY;
}

}