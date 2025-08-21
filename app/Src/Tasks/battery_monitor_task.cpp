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

// FDCAN and VESCAN support (only for STM32G474 with FDCAN)
#ifdef STM32G474xx
    extern "C" {
        #include "VESC.h"
    }
    
    // Battery telemetry VESC ID (using custom ID in VESC space)
    #define BATTERY_TELEMETRY_VESC_ID 0x50
#endif

#ifdef STM32G474xx
/**
 * @brief Transmit battery telemetry data via FDCAN using VESC protocol
 * @param telemetry Battery telemetry data to transmit
 * @return HAL_OK if successful, HAL_ERROR if transmission failed
 * 
 * This function converts battery telemetry data to VESC status frames
 * and transmits them over FDCAN. Multiple frames are used to send all
 * battery parameters while maintaining compatibility with VESC protocol.
 */
static HAL_StatusTypeDef transmitBatteryTelemetryFDCAN(const BQ40Z80::BatteryTelemetryData& telemetry) {
    // Create VESC Status_1 frame for primary electrical data
    VESC_Status_1 status1;
    status1.vescID = BATTERY_TELEMETRY_VESC_ID;
    status1.erpm = 0.0f;  // Not applicable for battery
    status1.current = (float)telemetry.current_mA / 1000.0f;  // Convert mA to A
    status1.dutyCycle = (float)telemetry.state_of_charge / 100.0f;  // SoC as duty cycle percentage
    
    // Convert to raw VESC frame
    VESC_RawFrame rawFrame;
    if (!VESC_convertStatus1ToRaw(&rawFrame, &status1)) {
        DEBUG_LOG("Battery FDCAN: Failed to convert Status1 to raw frame");
        return HAL_ERROR;
    }
    
    // Prepare FDCAN message header
    FDCAN_TxHeaderTypeDef txHeader;
    txHeader.Identifier = rawFrame.vescID;  // Use VESC ID from raw frame
    txHeader.IdType = FDCAN_STANDARD_ID;
    txHeader.TxFrameType = FDCAN_DATA_FRAME;
    txHeader.DataLength = FDCAN_DLC_BYTES_8;  // Standard CAN frame size
    txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    txHeader.BitRateSwitch = FDCAN_BRS_OFF;
    txHeader.FDFormat = FDCAN_CLASSIC_CAN;
    txHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    txHeader.MessageMarker = 0;
    
    // Transmit the frame
    HAL_StatusTypeDef status = HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txHeader, rawFrame.rawData);
    if (status != HAL_OK) {
        DEBUG_LOG("Battery FDCAN: Transmission failed (status=%d)", status);
        return status;
    }
    
    DEBUG_LOG("Battery FDCAN: Telemetry transmitted - ID:0x%03X, V:%umV, I:%dmA, SoC:%u%%", 
              txHeader.Identifier, telemetry.voltage_mV, telemetry.current_mA, telemetry.state_of_charge);
    
    return HAL_OK;
}
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
        DEBUG_LOG("Battery Monitor Task: No configuration provided");
        vTaskDelete(NULL);
        return;
    }
    
    if (!config->i2c_handle) {
        DEBUG_LOG("Battery Monitor Task: Invalid I2C handle");
        vTaskDelete(NULL);
        return;
    }
    
    const char* task_name = config->task_name ? config->task_name : "Battery";
    DEBUG_LOG("%s: Starting with I2C handle 0x%08lX", task_name, (uint32_t)config->i2c_handle);
    
    // Create custom configuration for BQ40Z80 driver
    BQ40Z80::Config driver_config = BQ40Z80::Driver::defaultConfig();
    driver_config.deviceAddress = config->device_address;
    
    // Create driver instance with injected I2C handle and configuration
    BQ40Z80::Driver battery(config->i2c_handle, driver_config);
    
    // Initialize communication and perform device recovery if needed
    HAL_StatusTypeDef status = battery.init();
    if (status != HAL_OK) {
        DEBUG_LOG("%s: Failed to initialize battery driver (status=%d)", task_name, status);
        vTaskDelete(NULL);
        return;
    }
    
    DEBUG_LOG("%s: Battery driver initialized successfully", task_name);
    
    // Main monitoring loop - runs until task is deleted or system reset
    while (1) {
        // Get packed battery telemetry data for transmission
        BQ40Z80::BatteryTelemetryData telemetry;
        HAL_StatusTypeDef telemetry_status = battery.getBatteryTelemetryData(telemetry);
        
        if (telemetry_status == HAL_OK) {
            // Generate comprehensive battery report for debugging
            battery.printBatteryReport();
            
            // Transmit telemetry data via FDCAN (STM32G474 only)
            #ifdef STM32G474xx
            HAL_StatusTypeDef can_status = transmitBatteryTelemetryFDCAN(telemetry);
            if (can_status != HAL_OK) {
                DEBUG_LOG("%s: FDCAN transmission failed (status=%d)", task_name, can_status);
            }
            #endif
        } else {
            DEBUG_LOG("%s: Failed to get telemetry data (status=%d)", task_name, telemetry_status);
        }
        
        // Use configured update interval
        vTaskDelay(pdMS_TO_TICKS(config->update_interval_ms));
    }
}