#ifndef HAL_TYPES_H
#define HAL_TYPES_H

#include <stdint.h>

// ===============================
// Platform-aware Debug Logging
// ===============================
#ifdef STM32G474xx
    // STM32G474 uses UART console via printf
    #include <stdio.h>
    #define DEBUG_LOG(format, ...) printf(format "\n", ##__VA_ARGS__)
#else
    // Other platforms use SEGGER RTT
    #include "SEGGER_RTT.h"
    #define DEBUG_LOG(format, ...) SEGGER_RTT_printf(0, format "\n", ##__VA_ARGS__)
#endif

#ifdef __cplusplus
extern "C" {
#endif

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

// FDCAN states and types - Only define if real HAL not present  
#ifndef HAL_FDCAN_MODULE_ENABLED
typedef struct FDCAN_HandleTypeDef FDCAN_HandleTypeDef;

typedef struct
{
  uint32_t Identifier;              // FDCAN identifier
  uint32_t IdType;                  // FDCAN identifier type
  uint32_t TxFrameType;             // FDCAN frame type
  uint32_t DataLength;              // FDCAN data length
  uint32_t ErrorStateIndicator;     // FDCAN error state indicator
  uint32_t BitRateSwitch;           // FDCAN bit rate switch
  uint32_t FDFormat;                // FDCAN format
  uint32_t TxEventFifoControl;      // FDCAN Tx event FIFO control
  uint32_t MessageMarker;           // FDCAN message marker
} FDCAN_TxHeaderTypeDef;


/**
  * @brief  FDCAN Rx header structure definition
  */
typedef struct
{
  uint32_t Identifier;            /*!< Specifies the identifier.
                                       This parameter must be a number between:
                                        - 0 and 0x7FF, if IdType is FDCAN_STANDARD_ID
                                        - 0 and 0x1FFFFFFF, if IdType is FDCAN_EXTENDED_ID               */

  uint32_t IdType;                /*!< Specifies the identifier type of the received message.
                                       This parameter can be a value of @ref FDCAN_id_type               */

  uint32_t RxFrameType;           /*!< Specifies the the received message frame type.
                                       This parameter can be a value of @ref FDCAN_frame_type            */

  uint32_t DataLength;            /*!< Specifies the received frame length.
                                        This parameter can be a value of @ref FDCAN_data_length_code     */

  uint32_t ErrorStateIndicator;   /*!< Specifies the error state indicator.
                                       This parameter can be a value of @ref FDCAN_error_state_indicator */

  uint32_t BitRateSwitch;         /*!< Specifies whether the Rx frame is received with or without bit
                                       rate switching.
                                       This parameter can be a value of @ref FDCAN_bit_rate_switching    */

  uint32_t FDFormat;              /*!< Specifies whether the Rx frame is received in classic or FD
                                       format.
                                       This parameter can be a value of @ref FDCAN_format                */

  uint32_t RxTimestamp;           /*!< Specifies the timestamp counter value captured on start of frame
                                       reception.
                                       This parameter must be a number between 0 and 0xFFFF              */

  uint32_t FilterIndex;           /*!< Specifies the index of matching Rx acceptance filter element.
                                       This parameter must be a number between:
                                        - 0 and (SRAMCAN_FLS_NBR-1), if IdType is FDCAN_STANDARD_ID
                                        - 0 and (SRAMCAN_FLE_NBR-1), if IdType is FDCAN_EXTENDED_ID
                                       When the frame is a Non-Filter matching frame, this parameter
                                       is unused.                                                        */

  uint32_t IsFilterMatchingFrame; /*!< Specifies whether the accepted frame did not match any Rx filter.
                                       Acceptance of non-matching frames may be enabled via
                                       HAL_FDCAN_ConfigGlobalFilter().
                                       This parameter takes 0 if the frame matched an Rx filter or
                                       1 if it did not match any Rx filter                               */

} FDCAN_RxHeaderTypeDef;

#define FDCAN_RX_FIFO0    			   ((uint32_t)0x00000040U) /*!< Get received message from Rx FIFO 0    */

#define FDCAN_IE_RF0NE_Pos        		(0U)
#define FDCAN_IE_RF0NE_Msk        		(0x1UL << FDCAN_IE_RF0NE_Pos)               /*!< 0x00000001 */
#define FDCAN_IE_RF0NE            		FDCAN_IE_RF0NE_Msk                          /*!<Rx FIFO 0 New Message Enable   */
#define FDCAN_IT_RX_FIFO0_NEW_MESSAGE  	FDCAN_IE_RF0NE 								/*!< New message written to Rx FIFO 0       */

// FDCAN Constants
#define FDCAN_STANDARD_ID              ((uint32_t)0x00000000U)
#define FDCAN_EXTENDED_ID              ((uint32_t)0x00000004U)
#define FDCAN_DATA_FRAME               ((uint32_t)0x00000000U)

#define FDCAN_DLC_BYTES_0              ((uint32_t)0x00000000U) /*!< 0 bytes data field  */
#define FDCAN_DLC_BYTES_1              ((uint32_t)0x00000001U) /*!< 1 bytes data field  */
#define FDCAN_DLC_BYTES_2              ((uint32_t)0x00000002U) /*!< 2 bytes data field  */
#define FDCAN_DLC_BYTES_3              ((uint32_t)0x00000003U) /*!< 3 bytes data field  */
#define FDCAN_DLC_BYTES_4              ((uint32_t)0x00000004U) /*!< 4 bytes data field  */
#define FDCAN_DLC_BYTES_5              ((uint32_t)0x00000005U) /*!< 5 bytes data field  */
#define FDCAN_DLC_BYTES_6              ((uint32_t)0x00000006U) /*!< 6 bytes data field  */
#define FDCAN_DLC_BYTES_7              ((uint32_t)0x00000007U) /*!< 7 bytes data field  */
#define FDCAN_DLC_BYTES_8              ((uint32_t)0x00000008U) /*!< 8 bytes data field  */
#define FDCAN_DLC_BYTES_12             ((uint32_t)0x00000009U) /*!< 12 bytes data field */
#define FDCAN_DLC_BYTES_16             ((uint32_t)0x0000000AU) /*!< 16 bytes data field */
#define FDCAN_DLC_BYTES_20             ((uint32_t)0x0000000BU) /*!< 20 bytes data field */
#define FDCAN_DLC_BYTES_24             ((uint32_t)0x0000000CU) /*!< 24 bytes data field */
#define FDCAN_DLC_BYTES_32             ((uint32_t)0x0000000DU) /*!< 32 bytes data field */
#define FDCAN_DLC_BYTES_48             ((uint32_t)0x0000000EU) /*!< 48 bytes data field */
#define FDCAN_DLC_BYTES_64             ((uint32_t)0x0000000FU) /*!< 64 bytes data field */

#define FDCAN_ESI_ACTIVE               ((uint32_t)0x00000000U)
#define FDCAN_BRS_OFF                  ((uint32_t)0x00000000U)
#define FDCAN_CLASSIC_CAN              ((uint32_t)0x00000000U)
#define FDCAN_NO_TX_EVENTS             ((uint32_t)0x00000000U)



#endif /* HAL_FDCAN_MODULE_ENABLED */

// ===============================
// HAL Function Declarations - Only declare if real HAL not present
// ===============================

#ifndef HAL_MODULE_ENABLED
// External handles - platform provides these
extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart2;
extern CAN_HandleTypeDef hcan1;
extern FDCAN_HandleTypeDef hfdcan1;

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

// FDCAN functions - platform will provide implementations  
HAL_StatusTypeDef HAL_FDCAN_Start(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_Stop(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_AddMessageToTxFifoQ(FDCAN_HandleTypeDef *hfdcan, FDCAN_TxHeaderTypeDef *pTxHeader, uint8_t *pTxData);
#endif /* HAL_MODULE_ENABLED */

// FreeRTOS functions - platform will provide implementations
void HAL_Delay_MS(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* HAL_TYPES_H */
