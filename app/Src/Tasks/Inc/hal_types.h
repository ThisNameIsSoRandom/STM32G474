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
typedef struct CAN_HandleTypeDef CAN_HandleTypeDef;

// Constants
#define SMBUS_FIRST_AND_LAST_FRAME_NO_PEC  0x00020000U

// Additional SMBUS frame options
#define SMBUS_FIRST_FRAME                   0x00002000U
#define SMBUS_LAST_FRAME_NO_PEC            0x00001000U
#define SMBUS_LAST_FRAME_WITH_PEC          0x00009000U
#define SMBUS_FIRST_AND_LAST_FRAME_WITH_PEC 0x0000B000U

#endif /* HAL_MODULE_ENABLED */

// I2C states - Only define if real HAL not present
#ifndef HAL_I2C_MODULE_ENABLED
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
#endif /* HAL_I2C_MODULE_ENABLED */

// CAN states and types - Only define if real HAL not present
#ifndef HAL_CAN_MODULE_ENABLED
typedef enum
{
  HAL_CAN_STATE_RESET             = 0x00U,
  HAL_CAN_STATE_READY             = 0x20U,
  HAL_CAN_STATE_LISTENING         = 0x21U,
  HAL_CAN_STATE_SLEEP_PENDING     = 0x22U,
  HAL_CAN_STATE_SLEEP_ACTIVE      = 0x23U,
  HAL_CAN_STATE_ERROR             = 0xE0U
} HAL_CAN_StateTypeDef;

typedef struct
{
  uint32_t StdId;    // Standard Identifier (11-bit)
  uint32_t ExtId;    // Extended Identifier (29-bit)
  uint32_t IDE;      // Identifier Extension
  uint32_t RTR;      // Remote Transmission Request
  uint32_t DLC;      // Data Length Code
  uint32_t Timestamp; // Time stamp counter value
  uint32_t FilterMatchIndex; // Filter Match Index
} CAN_RxHeaderTypeDef;

typedef struct
{
  uint32_t StdId;    // Standard Identifier (11-bit)
  uint32_t ExtId;    // Extended Identifier (29-bit)
  uint32_t IDE;      // Identifier Extension
  uint32_t RTR;      // Remote Transmission Request
  uint32_t DLC;      // Data Length Code
  uint32_t TransmitGlobalTime; // Transmit Global Time
} CAN_TxHeaderTypeDef;

// CAN Constants
#define CAN_ID_STD                     0x00000000U  // Standard Id
#define CAN_ID_EXT                     0x00000004U  // Extended Id
#define CAN_RTR_DATA                   0x00000000U  // Data frame
#define CAN_RTR_REMOTE                 0x00000002U  // Remote frame
#define CAN_RX_FIFO0                   0x00000000U  // CAN receive FIFO 0
#define CAN_RX_FIFO1                   0x00000001U  // CAN receive FIFO 1

#endif /* HAL_CAN_MODULE_ENABLED */

// ===============================
// HAL Function Declarations - Only declare if real HAL not present
// ===============================

#ifndef HAL_MODULE_ENABLED
// External handles - platform provides these
extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart2;
extern CAN_HandleTypeDef hcan1;

// HAL functions - platform will provide implementations
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c);
void MX_I2C2_Init(void);
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_UART_StateTypeDef HAL_UART_GetState(UART_HandleTypeDef *huart);

// CAN functions - platform will provide implementations
HAL_StatusTypeDef HAL_CAN_Start(CAN_HandleTypeDef *hcan);
HAL_StatusTypeDef HAL_CAN_Stop(CAN_HandleTypeDef *hcan);
HAL_StatusTypeDef HAL_CAN_AddTxMessage(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *pHeader, uint8_t aData[], uint32_t *pTxMailbox);
HAL_StatusTypeDef HAL_CAN_GetRxMessage(CAN_HandleTypeDef *hcan, uint32_t RxFifo, CAN_RxHeaderTypeDef *pHeader, uint8_t aData[]);
uint32_t HAL_CAN_GetRxFifoFillLevel(CAN_HandleTypeDef *hcan, uint32_t RxFifo);
HAL_StatusTypeDef HAL_CAN_ConfigFilter(CAN_HandleTypeDef *hcan, void *sFilterConfig);
HAL_CAN_StateTypeDef HAL_CAN_GetState(CAN_HandleTypeDef *hcan);
void MX_CAN1_Init(void);

// FDCAN types and functions - platform will provide implementations
typedef struct FDCAN_HandleTypeDef FDCAN_HandleTypeDef;

typedef struct
{
    uint32_t Identifier;
    uint32_t IdType;
    uint32_t TxFrameType;
    uint32_t DataLength;
    uint32_t ErrorStateIndicator;
    uint32_t BitRateSwitch;
    uint32_t FDFormat;
    uint32_t TxEventFifoControl;
    uint32_t MessageMarker;
} FDCAN_TxHeaderTypeDef;

// FDCAN Constants for compatibility
#define FDCAN_EXTENDED_ID                  0x00000004U
#define FDCAN_DATA_FRAME                   0x00000000U
#define FDCAN_ESI_ACTIVE                   0x00000000U
#define FDCAN_BRS_OFF                      0x00000000U
#define FDCAN_CLASSIC_CAN                  0x00000000U
#define FDCAN_NO_TX_EVENTS                 0x00000000U

#define FDCAN_DLC_BYTES_0                  0x00000000U
#define FDCAN_DLC_BYTES_1                  0x00010000U
#define FDCAN_DLC_BYTES_2                  0x00020000U
#define FDCAN_DLC_BYTES_3                  0x00030000U
#define FDCAN_DLC_BYTES_4                  0x00040000U
#define FDCAN_DLC_BYTES_5                  0x00050000U
#define FDCAN_DLC_BYTES_6                  0x00060000U
#define FDCAN_DLC_BYTES_7                  0x00070000U
#define FDCAN_DLC_BYTES_8                  0x00080000U

// FDCAN functions - platform will provide implementations
HAL_StatusTypeDef HAL_FDCAN_Start(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_Stop(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_AddMessageToTxFifoQ(FDCAN_HandleTypeDef *hfdcan, FDCAN_TxHeaderTypeDef *pTxHeader, uint8_t *pTxData);
void MX_FDCAN1_Init(void);
#endif /* HAL_MODULE_ENABLED */

// FreeRTOS functions - platform will provide implementations
void HAL_Delay_MS(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* HAL_TYPES_H */