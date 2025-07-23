/**
  ******************************************************************************
  * @file           : uart_task.cpp
  * @brief          : UART task implementation
  ******************************************************************************
  */

#include "hal_types.h"
#include "freertos_types.h"
#include "SEGGER_RTT.h"

extern "C" {

/**
 * @brief UART task for serial communication
 * @param pvParameters Task parameters
 */
void uartTask(void *pvParameters)
{
    (void)pvParameters;
    
    SEGGER_RTT_printf(0, "UART task started!\n\r");
    
    // Wait a bit for system to stabilize
    vTaskDelay(100);
    
    uint8_t tx_buffer[] = "Hello from UART task!\r\n";
    uint8_t rx_buffer[64];
    HAL_StatusTypeDef status;
    
    for(;;)
    {
        /* UART operations - Send hello message */
        
        SEGGER_RTT_printf(0, "Sending UART message...\n\r");
        
        // Send message via UART using HAL function - platform will implement
        status = HAL_UART_Transmit_IT(&huart2, tx_buffer, sizeof(tx_buffer) - 1);
        
        if(status == HAL_OK)
        {
            SEGGER_RTT_printf(0, "UART transmit initiated successfully\n\r");
            
            // Wait for transmission to complete
            vTaskDelay(50);
            
            // Check transmission state using HAL function
            if(HAL_UART_GetState(&huart2) == HAL_UART_STATE_READY)
            {
                SEGGER_RTT_printf(0, "UART transmission completed\n\r");
            }
            else
            {
                SEGGER_RTT_printf(0, "UART transmission still in progress\n\r");
            }
        }
        else
        {
            SEGGER_RTT_printf(0, "UART transmit failed with status: %d\n\r", status);
        }
        
        // Try to receive data (non-blocking check) using HAL function
        status = HAL_UART_Receive_IT(&huart2, rx_buffer, sizeof(rx_buffer) - 1);
        if(status == HAL_OK)
        {
            SEGGER_RTT_printf(0, "UART receive started\n\r");
            
            // Wait a bit to see if data arrives
            vTaskDelay(100);
            
            if(HAL_UART_GetState(&huart2) == HAL_UART_STATE_READY)
            {
                // Null terminate and print received data
                rx_buffer[sizeof(rx_buffer) - 1] = '\0';
                SEGGER_RTT_printf(0, "UART received: %s\n\r", rx_buffer);
            }
        }
        
        // Wait before next iteration
        vTaskDelay(3000);
    }
}

}