#include "app_tasks.h"
#include "cmsis_os.h"
#include "main.h"  // For HAL includes

// Board-specific LED definitions
#ifdef STM32G474xx
    #define LED_GPIO_PORT   GPIOA
    #define LED_GPIO_PIN    GPIO_PIN_5
#elif defined(STM32H755xx) && defined(USE_NUCLEO_64)
    // Nucleo H755 uses BSP for LEDs
    #include "stm32h7xx_nucleo.h"
    #define USE_BSP_LEDS
#endif

// Task handles and attributes definitions
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .attr_bits = 0,
  .cb_mem = NULL,
  .cb_size = 0,
  .stack_mem = NULL,
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
  .tz_module = 0,
  .reserved = 0
};

osThreadId_t BlinkErrorCodeHandle;
const osThreadAttr_t BlinkErrorCode_attributes = {
  .name = "BlinkErrorCode",
  .attr_bits = 0,
  .cb_mem = NULL,
  .cb_size = 0,
  .stack_mem = NULL,
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
  .tz_module = 0,
  .reserved = 0
};

// Helper functions for LED control
static void LED_On(void)
{
#ifdef USE_BSP_LEDS
    BSP_LED_On(LED_GREEN);
#else
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_SET);
#endif
}

static void LED_Off(void)
{
#ifdef USE_BSP_LEDS
    BSP_LED_Off(LED_GREEN);
#else
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_RESET);
#endif
}

static void LED_Toggle(void)
{
#ifdef USE_BSP_LEDS
    BSP_LED_Toggle(LED_GREEN);
#else
    HAL_GPIO_TogglePin(LED_GPIO_PORT, LED_GPIO_PIN);
#endif
}

// Default task implementation
void StartDefaultTask(void *argument)
{
  (void)argument;  // Mark parameter as intentionally unused
  
  // Infinite loop
  for(;;)
  {
    osDelay(1);
  }
}

// Error code blinking task implementation
void StartBlinkErrorCode(void *argument)
{
  (void)argument;  // Mark parameter as intentionally unused
  
  // Infinite loop
  for(;;)
  {
    // Pattern: 3 short blinks, pause, 2 long blinks, pause, repeat
    
    // 3 short blinks
    for(int i = 0; i < 3; i++) {
      LED_On();
      osDelay(200);  // 200ms on
      LED_Off();
      osDelay(200);  // 200ms off
    }
    
    osDelay(500);  // Pause between patterns
    
    // 2 long blinks
    for(int i = 0; i < 2; i++) {
      LED_On();
      osDelay(800);  // 800ms on
      LED_Off();
      osDelay(200);  // 200ms off
    }
    
    osDelay(1000);  // Long pause before repeating
  }
}

// Nucleo H755 specific task
void StartLedTask(void *argument)
{
  (void)argument;  // Mark parameter as intentionally unused
  
  // Initialize BSP LEDs if needed
#ifdef USE_BSP_LEDS
  BSP_LED_Init(LED_GREEN);
#endif
  
  // Infinite loop
  for(;;)
  {
    LED_Toggle();
    osDelay(500);
  }
}