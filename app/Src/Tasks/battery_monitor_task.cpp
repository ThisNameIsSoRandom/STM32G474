/**
 * @file battery_monitor_task.cpp
 * @brief FreeRTOS task for continuous battery monitoring
 * 
 * This task provides a platform-independent battery monitoring service
 * that runs as a FreeRTOS task. It initializes the BQ40Z80 driver and
 * periodically reads battery parameters, outputting them via SEGGER RTT.
 * 
 * ## Task Requirements:
 * - Stack size: 1024 words minimum (handles BQ40Z80 driver overhead)
 * - Priority: tskIDLE_PRIORITY + 2 (moderate priority for monitoring)
 * - Platform must provide configured I2C peripheral
 * - SEGGER RTT must be initialized for debug output
 * 
 * ## Platform Integration:
 * The platform is responsible for:
 * 1. Configuring I2C hardware (100kHz or 400kHz)
 * 2. Creating the task with appropriate stack and priority
 * 3. Ensuring HAL_I2C functions are properly implemented
 * 
 * Example platform integration:
 * ```cpp
 * TaskHandle_t batteryTaskHandle = NULL;
 * xTaskCreate(batteryMonitorTask, "Battery", 1024, NULL, 
 *             tskIDLE_PRIORITY + 2, &batteryTaskHandle);
 * ```
 */

#include "battery_monitor_task.h"
#include "hal_types.h"
#include "freertos_types.h"
#include "BQ40Z80/bq40z80.h"
#include "SEGGER_RTT.h"

// Platform-aware logging
#ifdef STM32G474xx
#include <stdio.h>
#define BATTERY_LOG(format, ...) printf(format "\n", ##__VA_ARGS__)
#else
#define BATTERY_LOG(format, ...) SEGGER_RTT_printf(0, format "\n", ##__VA_ARGS__)
#endif

/**
 * @brief Battery monitoring task entry point
 * @param pvParameters Pointer to BatteryTaskConfig structure
 * 
 * This task runs indefinitely, monitoring battery status at the configured interval.
 * The task configuration is passed via pvParameters to enable platform independence
 * and multi-device support.
 * 
 * The task follows this sequence:
 * 1. Validate configuration parameters
 * 2. Initialize BQ40Z80 driver with injected I2C handle
 * 3. Verify communication with battery
 * 4. Enter monitoring loop with configurable update interval
 * 
 * @note Task will self-delete if configuration is invalid or initialization fails
 */
extern "C" void batteryMonitorTask(void *pvParameters) {
    // Cast parameters to configuration structure
    BatteryTaskConfig* config = static_cast<BatteryTaskConfig*>(pvParameters);
    
    // Validate configuration
    if (!config) {
        BATTERY_LOG("Battery Monitor Task: No configuration provided");
        vTaskDelete(NULL);
        return;
    }
    
    if (!config->i2c_handle) {
        BATTERY_LOG("Battery Monitor Task: Invalid I2C handle");
        vTaskDelete(NULL);
        return;
    }
    
    const char* task_name = config->task_name ? config->task_name : "Battery";
    BATTERY_LOG("%s: Starting with I2C handle 0x%08lX", task_name, (uint32_t)config->i2c_handle);
    
    // Create custom configuration for BQ40Z80 driver
    BQ40Z80::Config driver_config = BQ40Z80::Driver::defaultConfig();
    driver_config.deviceAddress = config->device_address;
    
    // Create driver instance with injected I2C handle and configuration
    BQ40Z80::Driver battery(config->i2c_handle, driver_config);
    
    // Initialize communication and perform device recovery if needed
    HAL_StatusTypeDef status = battery.init();
    if (status != HAL_OK) {
        BATTERY_LOG("%s: Failed to initialize battery driver (status=%d)", task_name, status);
        vTaskDelete(NULL);
        return;
    }
    
    BATTERY_LOG("%s: Battery driver initialized successfully", task_name);
    
    // Main monitoring loop - runs until task is deleted or system reset
    while (1) {
        // Generate comprehensive battery report including voltage, current,
        // temperature, capacity, and status flags
        battery.printBatteryReport();
        
        // Use configured update interval
        vTaskDelay(pdMS_TO_TICKS(config->update_interval_ms));
    }
}