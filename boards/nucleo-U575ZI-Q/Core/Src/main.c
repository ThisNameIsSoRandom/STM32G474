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
  * Copyright (c) 2025 Raptors PŁ Poland.
  * All rights reserved.
  *
  * This modified software is proprietary. All rights reserved.
  * Original components licensed under SLA0048 terms in the LICENSE file.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "icache.h"
#include "memorymap.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "battery_monitor_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "logging.h"
#include "SEGGER_RTT.h"
#include "freertos_tasks.h"
#include "i2c.h"

// Unit test integration - declare as weak symbols so they can be overridden
extern "C" __attribute__((weak)) void startUnitTests(void) {
    // Weak implementation - does nothing if tests not linked
    SEGGER_RTT_printf(0, "Unit tests not linked - skipping\n");
}

extern "C" __attribute__((weak)) void runTestsTask(void* pvParameters) {
    (void)pvParameters;
    // Weak implementation - just delete task if tests not linked
    SEGGER_RTT_printf(0, "Test task not implemented - deleting task\n");
    vTaskDelete(NULL);  // Use NULL instead of nullptr for C compatibility
}

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void SystemPower_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
class moo {

};
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

  /* Configure the System Power */
  SystemPower_Config();

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C2_Init();
  MX_ICACHE_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  // Initialize logging system
  initLogging();

  SEGGER_RTT_printf(0, "\n=== STM32U575ZI-Q Platform Startup ===\n");
  SEGGER_RTT_printf(0, "Platform: NUCLEO-U575ZI-Q\n");
  SEGGER_RTT_printf(0, "MCU: STM32U575ZI Cortex-M33\n");
  SEGGER_RTT_printf(0, "FreeRTOS: Enabled\n");
  SEGGER_RTT_printf(0, "\n--- Creating Application Tasks ---\n");

  /* Configure battery monitor task for STM32U575ZI-Q platform */
  static BatteryTaskConfig battery_config = {
      .i2c_handle = &hi2c2,                    // Use I2C2 peripheral on this platform
      .device_address = BATTERY_DEFAULT_ADDRESS, // Standard BQ40Z80 address (0x0B)
      .update_interval_ms = 3000,              // Read battery every 3 seconds
      .task_name = "BatteryU575"               // Platform-specific task name
  };

  /* Create battery monitor task for BQ40Z80 management */
  TaskHandle_t batteryTaskHandle = NULL;
  BaseType_t xReturned = xTaskCreate(
      batteryMonitorTask,           // Task function - battery monitoring
      "Battery",                    // Task name
      BATTERY_MONITOR_TASK_STACK_SIZE, // Stack size from configuration
      &battery_config,              // Pass configuration as parameters
      BATTERY_MONITOR_TASK_PRIORITY,// Task priority from configuration
      &batteryTaskHandle            // Task handle
  );
  
  if(xReturned != pdPASS) {
    SEGGER_RTT_printf(0, "ERROR: Failed to create battery monitor task\n");
    Error_Handler();
  }
  SEGGER_RTT_printf(0, "Battery monitor task created successfully (I2C2, 3sec interval)\n");

  /* UART task is currently disabled */
  //TaskHandle_t uartTaskHandle = NULL;
  //xReturned = xTaskCreate(uartTask, "UART", 1024, NULL, tskIDLE_PRIORITY + 1, &uartTaskHandle);
  
  SEGGER_RTT_printf(0, "\n--- Starting FreeRTOS Scheduler ---\n");
  SEGGER_RTT_printf(0, "Application ready\n\n");

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
/* USER CODE BEGIN IGNORE C WARNINGS*/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
/* USER CODE END IGNORE C WARNINGS*/
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_4;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Power Configuration
  * @retval None
  */
static void SystemPower_Config(void)
{
  HAL_PWREx_EnableVddIO2();

  /*
   * Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
   */
  HAL_PWREx_DisableUCPDDeadBattery();

  /*
   * Switch to SMPS regulator instead of LDO
   */
  if (HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY) != HAL_OK)
  {
    Error_Handler();
  }
/* USER CODE BEGIN PWR */
/* USER CODE END PWR */
}

/* USER CODE BEGIN 4 */

// HAL_Delay_MS implementation for app library
extern "C" void HAL_Delay_MS(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM17 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM17)
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

#ifdef  USE_FULL_ASSERT
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
/* USER CODE BEGIN IGNORE C WARNINGS*/
#pragma GCC diagnostic pop
/* USER CODE END IGNORE C WARNINGS*/