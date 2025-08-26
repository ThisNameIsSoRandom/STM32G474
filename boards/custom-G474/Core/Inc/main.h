/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RELAY_Pin GPIO_PIN_1
#define RELAY_GPIO_Port GPIOA
#define DRIVE_STOP_Pin GPIO_PIN_2
#define DRIVE_STOP_GPIO_Port GPIOA
#define LED_CAN_OK_Pin GPIO_PIN_5
#define LED_CAN_OK_GPIO_Port GPIOA
#define LED_CAN_ERROR_Pin GPIO_PIN_6
#define LED_CAN_ERROR_GPIO_Port GPIOA
#define SMB3C_Pin GPIO_PIN_6
#define SMB3C_GPIO_Port GPIOC
#define SMB3D_Pin GPIO_PIN_7
#define SMB3D_GPIO_Port GPIOC
#define SMB1C_Pin GPIO_PIN_8
#define SMB1C_GPIO_Port GPIOC
#define SMB1D_Pin GPIO_PIN_9
#define SMB1D_GPIO_Port GPIOC
#define SMB2D_Pin GPIO_PIN_8
#define SMB2D_GPIO_Port GPIOA
#define SMB2C_Pin GPIO_PIN_9
#define SMB2C_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
