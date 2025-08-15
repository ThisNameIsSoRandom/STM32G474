/**
 * @file hal_types.h
 * @brief Type definitions for HAL mock compatibility
 * 
 * Defines the minimal HAL types needed for mocking I2C functions.
 * These types match the STM32 HAL structure without including the full HAL.
 */

#ifndef HAL_TYPES_H
#define HAL_TYPES_H

#include <stdint.h>

/**
 * @brief HAL Status structures definition
 */
typedef enum
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

/**
 * @brief I2C handle structure (minimal for mocking)
 */
typedef struct
{
    void* Instance;  // Pointer to I2C peripheral (not used in mock)
    uint32_t Init;   // Configuration (not used in mock) 
    uint8_t State;   // Current state (not used in mock)
} I2C_HandleTypeDef;

#endif // HAL_TYPES_H