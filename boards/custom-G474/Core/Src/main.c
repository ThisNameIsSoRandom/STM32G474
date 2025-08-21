/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fdcan.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_tasks.h"
#include "vescan_task.h"
#include "battery_monitor_task.h"
#include "hal_types.h"  // For unified DEBUG_LOG macro
#include <stdio.h>
#include <string.h>

// Unit test integration - declare as weak symbols so they can be overridden
extern "C" __attribute__((weak)) void startUnitTests(void) {
    // Weak implementation - does nothing if tests not linked
    DEBUG_LOG("Unit tests not linked - skipping");
}

extern "C" __attribute__((weak)) void runTestsTask(void* pvParameters) {
    (void)pvParameters;
    // Weak implementation - just delete task if tests not linked
    DEBUG_LOG("Test task not implemented - deleting task");
    vTaskDelete(NULL);  // Use NULL instead of nullptr for C compatibility
}
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// NOTE: Logging method selection and macro definitions are above, before function definitions
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FDCAN1_Init();
  MX_I2C2_Init();
  MX_I2C3_Init();
  MX_I2C4_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */
  // Test UART4 hardware directly first
  const char* test_msg = "UART4 HAL Test OK\n\r";
  HAL_UART_Transmit(&huart4, (uint8_t*)test_msg, strlen(test_msg), 1000);
  
  // Test UART4 console via printf/syscalls
  printf("=== UART4 Console Test ===\n\r");
  printf("Direct printf test successful!\n\r");
  
  // Display startup banner via UART console
  printf("mmmmm mmmmm mmmmm mmmmm mmmmm mmmmm \n\r");
  printf("MM                               MM \n\r");
  printf("MM     `7MM\"\"\"YMM   .M\"\"\"bgd     MM\tExisting solutions v 01.08.25\n\r");
  printf("MM       MM    `7  ,MI    \"Y     MM \n\r");
  printf("MM       MM   d    `MMb.         MM \n\r");
  printf("MM       MMmmMM      `YMMNq.     MM \n\r");
  printf("MM       MM   Y  , .     `MM     MM \n\r");
  printf("MM       MM     ,M Mb     dM     MM \n\r");
  printf("MM     .JMMmmmmMMM P\"Ybmmd\"      MM \n\r");
  printf("MM                               MM \n\r");
  printf("mmmmm mmmmm mmmmm mmmmm mmmmm mmmmm \n\r");

  DEBUG_LOG("\n=== STM32G474 Custom Board Startup ===");
  DEBUG_LOG("Platform: Custom-G474");
  DEBUG_LOG("MCU: STM32G474RE Cortex-M4F");
  DEBUG_LOG("FreeRTOS: Enabled");
  DEBUG_LOG("Architecture: SMBus + VESCAN tasks with queues");
  DEBUG_LOG("\n--- Creating Application Tasks ---");

  /* Initialize VESCAN queues (temporarily disabled for debugging) */
  // vescanInitQueues();
  DEBUG_LOG("VESCAN queues initialization skipped (debugging)");

  /* Configure battery monitor task for STM32G474 platform */
  static BatteryTaskConfig battery_config = {
      &hi2c3,                    // Use I2C3 peripheral on this platform
      BATTERY_DEFAULT_ADDRESS,   // Standard BQ40Z80 address (0x0B)
      3000,                      // Read battery every 3 seconds
      "BatteryG474"              // Platform-specific task name
  };

  /* Create battery monitor task for BQ40Z80 management */
  TaskHandle_t batteryTaskHandle = NULL;
  BaseType_t xReturned = xTaskCreate(
      batteryMonitorTask,           // Task function - battery monitoring with reports
      "Battery",                    // Task name
      BATTERY_MONITOR_TASK_STACK_SIZE, // Stack size from configuration
      &battery_config,              // Pass configuration as parameters
      BATTERY_MONITOR_TASK_PRIORITY,// Task priority from configuration
      &batteryTaskHandle            // Task handle
  );
  
  if(xReturned != pdPASS) {
    DEBUG_LOG("ERROR: Failed to create battery monitor task");
    Error_Handler();
  }
  DEBUG_LOG("Battery monitor task created successfully (I2C3, 3sec reports)");

  /* Create VESCAN task (temporarily disabled for debugging) */
  DEBUG_LOG("VESCAN task creation skipped (debugging - no queues initialized)");
  /*
  TaskHandle_t vescanTaskHandle = NULL;
  xReturned = xTaskCreate(
      vescanTask,                   // Task function
      "VESCAN",                     // Task name
      512,                          // Stack size
      NULL,                         // Parameters
      tskIDLE_PRIORITY + 2,         // Priority
      &vescanTaskHandle             // Task handle
  );
  
  if(xReturned != pdPASS) {
    DEBUG_LOG("ERROR: Failed to create VESCAN task");
    Error_Handler();
  }
  DEBUG_LOG("VESCAN task created successfully (FDCAN1)");
  */

  /* UART task is currently disabled */
  //TaskHandle_t uartTaskHandle = NULL;
  //xReturned = xTaskCreate(uartTask, "UART", 1024, NULL, tskIDLE_PRIORITY + 1, &uartTaskHandle);
  
  DEBUG_LOG("\n--- Starting FreeRTOS Scheduler ---");
  DEBUG_LOG("Application ready\n");

  /* Start FreeRTOS scheduler */
  vTaskStartScheduler();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

// FDCAN handle for G474 platform (G474 uses FDCAN, not classic CAN)
extern FDCAN_HandleTypeDef hfdcan1;

// HAL_Delay_MS implementation for app library
extern "C" void HAL_Delay_MS(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

// FreeRTOS static allocation hook functions
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize) {
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
  *ppxIdleTaskStackBuffer = uxIdleTaskStack;
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

#if (configUSE_TIMERS == 1)
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize) {
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
  *ppxTimerTaskStackBuffer = uxTimerTaskStack;
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
#endif

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
