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
        // Print comprehensive battery report
        battery.printBatteryReport();
        
        // Wait 10 seconds before next reading (longer interval for comprehensive report)
        vTaskDelay(10000);  // Platform will convert to ticks
    }
}