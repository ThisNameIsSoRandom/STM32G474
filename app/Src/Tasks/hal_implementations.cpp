/**
 ******************************************************************************
 * @file           : hal_implementations.cpp  
 * @brief          : HAL function weak implementations for Tasks
 ******************************************************************************
 */

#include "hal_types.h"
#include "freertos_types.h"
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
    DEBUG_LOG("WARNING: HAL_Delay_MS(%u) not implemented by platform - no delay applied", (unsigned int)ms);
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
    DEBUG_LOG("WARNING: HAL_I2C_Master_Transmit not implemented by platform");
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
    DEBUG_LOG("WARNING: HAL_I2C_Master_Receive not implemented by platform");
    return HAL_OK;
}

/**
 * @brief Weak implementation of I2C Get State
 */
__weak HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef *hi2c)
{
    (void)hi2c;
    DEBUG_LOG("WARNING: HAL_I2C_GetState not implemented by platform");
    return HAL_I2C_STATE_READY;
}

/**
 * @brief Weak implementation of I2C DeInit
 */
__weak HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c)
{
    (void)hi2c;
    DEBUG_LOG("WARNING: HAL_I2C_DeInit not implemented by platform");
    return HAL_OK;
}


/**
 * @brief Weak implementation of I2C Init
 */
__weak void MX_I2C2_Init(void)
{
    DEBUG_LOG("WARNING: MX_I2C2_Init not implemented by platform");
}

// ===============================
// Weak CAN implementations for VESC CAN library
// ===============================

/**
 * @brief Weak implementation of CAN Start
 */
__weak HAL_StatusTypeDef HAL_CAN_Start(CAN_HandleTypeDef *hcan)
{
    (void)hcan;
    DEBUG_LOG("WARNING: HAL_CAN_Start not implemented by platform");
    return HAL_OK;
}

/**
 * @brief Weak implementation of CAN Stop
 */
__weak HAL_StatusTypeDef HAL_CAN_Stop(CAN_HandleTypeDef *hcan)
{
    (void)hcan;
    DEBUG_LOG("WARNING: HAL_CAN_Stop not implemented by platform");
    return HAL_OK;
}

/**
 * @brief Weak implementation of CAN Add Tx Message
 */
__weak HAL_StatusTypeDef HAL_CAN_AddTxMessage(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *pHeader, uint8_t aData[], uint32_t *pTxMailbox)
{
    (void)hcan;
    (void)pHeader;
    (void)aData;
    (void)pTxMailbox;
    DEBUG_LOG("WARNING: HAL_CAN_AddTxMessage not implemented by platform - VESC command not transmitted");
    return HAL_OK;
}

/**
 * @brief Weak implementation of CAN Get Rx Message
 */
__weak HAL_StatusTypeDef HAL_CAN_GetRxMessage(CAN_HandleTypeDef *hcan, uint32_t RxFifo, CAN_RxHeaderTypeDef *pHeader, uint8_t aData[])
{
    (void)hcan;
    (void)RxFifo;
    (void)pHeader;
    (void)aData;
    DEBUG_LOG("WARNING: HAL_CAN_GetRxMessage not implemented by platform - no VESC status received");
    return HAL_ERROR;
}

/**
 * @brief Weak implementation of CAN Get Rx FIFO Fill Level
 */
__weak uint32_t HAL_CAN_GetRxFifoFillLevel(CAN_HandleTypeDef *hcan, uint32_t RxFifo)
{
    (void)hcan;
    (void)RxFifo;
    DEBUG_LOG("WARNING: HAL_CAN_GetRxFifoFillLevel not implemented by platform");
    return 0;
}

/**
 * @brief Weak implementation of CAN Config Filter
 */
__weak HAL_StatusTypeDef HAL_CAN_ConfigFilter(CAN_HandleTypeDef *hcan, void *sFilterConfig)
{
    (void)hcan;
    (void)sFilterConfig;
    DEBUG_LOG("WARNING: HAL_CAN_ConfigFilter not implemented by platform");
    return HAL_OK;
}

/**
 * @brief Weak implementation of CAN Get State
 */
__weak HAL_CAN_StateTypeDef HAL_CAN_GetState(CAN_HandleTypeDef *hcan)
{
    (void)hcan;
    DEBUG_LOG("WARNING: HAL_CAN_GetState not implemented by platform");
    return HAL_CAN_STATE_READY;
}

/**
 * @brief Weak implementation of CAN Init
 */
__weak void MX_CAN1_Init(void)
{
    DEBUG_LOG("WARNING: MX_CAN1_Init not implemented by platform - VESC CAN communication not available");
}

/**
 * @brief Weak implementation of UART Transmit IT
 */
__weak HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    (void)huart;
    (void)pData;
    (void)Size;
    DEBUG_LOG("WARNING: HAL_UART_Transmit_IT not implemented by platform");
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
    DEBUG_LOG("WARNING: HAL_UART_Receive_IT not implemented by platform");
    return HAL_OK;
}

/**
 * @brief Weak implementation of UART Get State
 */
__weak HAL_UART_StateTypeDef HAL_UART_GetState(UART_HandleTypeDef *huart)
{
    (void)huart;
    DEBUG_LOG("WARNING: HAL_UART_GetState not implemented by platform");
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
    DEBUG_LOG("WARNING: vTaskDelete not implemented by platform - cannot delete task");
    // Cannot actually delete task without FreeRTOS
}

/**
 * @brief Weak implementation of vTaskDelay
 */
__weak void vTaskDelay(const TickType_t xTicksToDelay)
{
    (void)xTicksToDelay;
    DEBUG_LOG("WARNING: vTaskDelay not implemented by platform - no delay applied");
    // No delay in hollow implementation
}

/**
 * @brief Weak implementation of xQueueCreate
 */
__weak QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength, UBaseType_t uxItemSize)
{
    (void)uxQueueLength;
    (void)uxItemSize;
    DEBUG_LOG("WARNING: xQueueCreate not implemented by platform - returning null queue");
    return nullptr;
}

/**
 * @brief Weak implementation of xQueueSend
 */
__weak BaseType_t xQueueSend(QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait)
{
    (void)xQueue;
    (void)pvItemToQueue;
    (void)xTicksToWait;
    DEBUG_LOG("WARNING: xQueueSend not implemented by platform - message lost");
    return pdFAIL;
}

/**
 * @brief Weak implementation of xQueueReceive
 */
__weak BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait)
{
    (void)xQueue;
    (void)pvBuffer;
    (void)xTicksToWait;
    DEBUG_LOG("WARNING: xQueueReceive not implemented by platform - no data received");
    return pdFAIL;
}

/**
 * @brief Weak implementation of xTaskGetTickCount
 */
__weak TickType_t xTaskGetTickCount(void)
{
    DEBUG_LOG("WARNING: xTaskGetTickCount not implemented by platform - returning 0");
    return 0;
}

__weak HAL_StatusTypeDef HAL_FDCAN_GetRxMessage(FDCAN_HandleTypeDef *hfdcan, uint32_t RxLocation,
                                         FDCAN_RxHeaderTypeDef *pRxHeader, uint8_t *pRxData);

}
