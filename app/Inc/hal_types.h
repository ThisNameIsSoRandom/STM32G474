#ifndef HAL_TYPES_H
#define HAL_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// ===============================
// HAL Types - Only define if real HAL not present
// ===============================

#ifndef HAL_MODULE_ENABLED

typedef enum
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

typedef enum
{
  HAL_UART_STATE_RESET             = 0x00U,
  HAL_UART_STATE_READY             = 0x20U,
  HAL_UART_STATE_BUSY              = 0x24U,
  HAL_UART_STATE_BUSY_TX           = 0x21U,
  HAL_UART_STATE_BUSY_RX           = 0x22U,
  HAL_UART_STATE_BUSY_TX_RX        = 0x23U,
  HAL_UART_STATE_TIMEOUT           = 0xA0U,
  HAL_UART_STATE_ERROR             = 0xE0U
} HAL_UART_StateTypeDef;

// Handle typedefs (opaque)
typedef struct I2C_HandleTypeDef I2C_HandleTypeDef;
typedef struct UART_HandleTypeDef UART_HandleTypeDef;

// Constants
#define SMBUS_FIRST_AND_LAST_FRAME_NO_PEC  0x00020000U

// Additional SMBUS frame options
#define SMBUS_FIRST_FRAME                   0x00002000U
#define SMBUS_LAST_FRAME_NO_PEC            0x00001000U
#define SMBUS_LAST_FRAME_WITH_PEC          0x00009000U
#define SMBUS_FIRST_AND_LAST_FRAME_WITH_PEC 0x0000B000U

#endif /* HAL_MODULE_ENABLED */

// I2C states - Always define these for function declarations
typedef enum
{
  HAL_I2C_STATE_RESET             = 0x00U,
  HAL_I2C_STATE_READY             = 0x20U,
  HAL_I2C_STATE_BUSY              = 0x24U,
  HAL_I2C_STATE_BUSY_TX           = 0x21U,
  HAL_I2C_STATE_BUSY_RX           = 0x22U,
  HAL_I2C_STATE_BUSY_TX_RX        = 0x23U,
  HAL_I2C_STATE_TIMEOUT           = 0xA0U,
  HAL_I2C_STATE_ERROR             = 0xE0U
} HAL_I2C_StateTypeDef;

// ===============================
// HAL Function Declarations - Always declare
// ===============================

// External handles - platform provides these
extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart2;

// HAL functions - platform will provide implementations
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c);
void MX_I2C2_Init(void);
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_UART_StateTypeDef HAL_UART_GetState(UART_HandleTypeDef *huart);

// FreeRTOS functions - platform will provide implementations
void HAL_Delay_MS(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* HAL_TYPES_H */