/**
 ******************************************************************************
 * @file           : hal_implementations.cpp  
 * @brief          : HAL function weak implementations for Tasks
 ******************************************************************************
 */

#include "hal_types.h"
#include "freertos_types.h"
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
 * @brief Weak implementation of I2C Master Transmit
 */
__weak HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    (void)hi2c;
    (void)DevAddress;
    (void)pData;
    (void)Size;
    (void)Timeout;
    SEGGER_RTT_printf(0, "WARNING: HAL_I2C_Master_Transmit not implemented by platform\n\r");
    return HAL_OK;
}

/**
 * @brief Weak implementation of I2C Master Receive
 */
__weak HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    (void)hi2c;
    (void)DevAddress;
    (void)pData;
    (void)Size;
    (void)Timeout;
    SEGGER_RTT_printf(0, "WARNING: HAL_I2C_Master_Receive not implemented by platform\n\r");
    return HAL_OK;
}

/**
 * @brief Weak implementation of I2C Get State
 */
__weak HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef *hi2c)
{
    (void)hi2c;
    SEGGER_RTT_printf(0, "WARNING: HAL_I2C_GetState not implemented by platform\n\r");
    return HAL_I2C_STATE_READY;
}

/**
 * @brief Weak implementation of I2C DeInit
 */
__weak HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c)
{
    (void)hi2c;
    SEGGER_RTT_printf(0, "WARNING: HAL_I2C_DeInit not implemented by platform\n\r");
    return HAL_OK;
}


/**
 * @brief Weak implementation of I2C Init
 */
__weak void MX_I2C2_Init(void)
{
    SEGGER_RTT_printf(0, "WARNING: MX_I2C2_Init not implemented by platform\n\r");
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

// ===============================
// Weak FreeRTOS implementations for Tasks library
// ===============================

/**
 * @brief Weak implementation of vTaskDelete
 */
__weak void vTaskDelete(TaskHandle_t xTaskToDelete)
{
    (void)xTaskToDelete;
    SEGGER_RTT_printf(0, "WARNING: vTaskDelete not implemented by platform - cannot delete task\n\r");
    // Cannot actually delete task without FreeRTOS
}

/**
 * @brief Weak implementation of vTaskDelay
 */
__weak void vTaskDelay(const TickType_t xTicksToDelay)
{
    (void)xTicksToDelay;
    SEGGER_RTT_printf(0, "WARNING: vTaskDelay not implemented by platform - no delay applied\n\r");
    // No delay in hollow implementation
}

}