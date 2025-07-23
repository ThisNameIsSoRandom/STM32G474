#include "hal_types.h"
#include "freertos_types.h"
#include "BQ40Z80/bq40z80.h"
#include "SEGGER_RTT.h"

extern "C" void batteryMonitorTask(void *pvParameters) {
    (void)pvParameters;
    
    SEGGER_RTT_printf(0, "Battery Monitor Task: Starting\n");
    
    // Initialize BQ40Z80 driver with default configuration
    BQ40Z80::Driver battery;
    
    // Initialize communication with battery
    HAL_StatusTypeDef status = battery.init();
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "Battery Monitor Task: Failed to initialize battery driver\n");
        vTaskDelete(nullptr);
        return;
    }
    
    SEGGER_RTT_printf(0, "Battery Monitor Task: Battery driver initialized successfully\n");
    
    // Main monitoring loop
    while (1) {
        // Read complete battery data
        BQ40Z80::BatteryData batteryData;
        status = battery.read(BQ40Z80::Reading::AllBatteryData, batteryData);
        
        if (status == HAL_OK) {
            // Convert temperature to Celsius for display
            float tempC = BQ40Z80::Driver::temperatureToC(batteryData.temperature);
            
            SEGGER_RTT_printf(0, "Battery Status:\n");
            SEGGER_RTT_printf(0, "  Voltage: %u mV\n", batteryData.voltage);
            SEGGER_RTT_printf(0, "  Current: %d mA\n", batteryData.current);
            SEGGER_RTT_printf(0, "  Temperature: %.1f°C\n", tempC);
            SEGGER_RTT_printf(0, "  State of Charge: %u%%\n", batteryData.stateOfCharge);
            SEGGER_RTT_printf(0, "  Remaining Capacity: %u mAh\n", batteryData.remainingCapacity);
            SEGGER_RTT_printf(0, "  Full Charge Capacity: %u mAh\n", batteryData.fullChargeCapacity);
            SEGGER_RTT_printf(0, "  Cycle Count: %u\n", batteryData.cycleCount);
            
            // Status flags
            SEGGER_RTT_printf(0, "  Status: ");
            if (batteryData.status.discharging) {
                SEGGER_RTT_printf(0, "DISCHARGING ");
            }
            if (batteryData.status.fullyCharged) {
                SEGGER_RTT_printf(0, "FULLY_CHARGED ");
            }
            if (batteryData.status.fullyDischarged) {
                SEGGER_RTT_printf(0, "FULLY_DISCHARGED ");
            }
            if (batteryData.status.initialized) {
                SEGGER_RTT_printf(0, "INITIALIZED ");
            }
            SEGGER_RTT_printf(0, "\n");
            
            // Alarm conditions
            if (batteryData.status.overChargedAlarm) {
                SEGGER_RTT_printf(0, "  ALARM: Over-charged!\n");
            }
            if (batteryData.status.overTempAlarm) {
                SEGGER_RTT_printf(0, "  ALARM: Over-temperature!\n");
            }
            if (batteryData.status.terminateDischargeAlarm) {
                SEGGER_RTT_printf(0, "  ALARM: Terminate discharge!\n");
            }
            if (batteryData.status.remainingCapacityAlarm) {
                SEGGER_RTT_printf(0, "  ALARM: Low remaining capacity!\n");
            }
            
            if (batteryData.status.errorCode != 0) {
                SEGGER_RTT_printf(0, "  ERROR CODE: 0x%X\n", batteryData.status.errorCode);
            }
            
        } else {
            SEGGER_RTT_printf(0, "Battery Monitor Task: Failed to read battery data (status: %d)\n", status);
            
            // Try to read individual parameters for debugging
            uint16_t voltage;
            if (battery.read(BQ40Z80::Reading::Voltage, voltage) == HAL_OK) {
                SEGGER_RTT_printf(0, "  Individual voltage read successful: %u mV\n", voltage);
            } else {
                SEGGER_RTT_printf(0, "  Individual voltage read also failed\n");
            }
        }
        
        SEGGER_RTT_printf(0, "\n"); // Empty line for readability
        
        // Wait 5 seconds before next reading
        vTaskDelay(5000);  // Platform will convert to ticks
    }
}