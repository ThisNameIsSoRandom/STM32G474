#ifndef BATTERY_MONITOR_TASK_H
#define BATTERY_MONITOR_TASK_H

#include "hal_types.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration structure for battery monitor task
 * 
 * This structure contains all configurable parameters for the battery
 * monitoring task, enabling platform independence and flexibility.
 */
typedef struct {
    I2C_HandleTypeDef* i2c_handle;      /**< I2C peripheral handle for communication */
    uint8_t device_address;             /**< BQ40Z80 I2C device address (typically 0x0B) */
    uint32_t update_interval_ms;        /**< Battery reading update interval in milliseconds */
    const char* task_name;              /**< Task name for debugging and logging */
} BatteryTaskConfig;

/**
 * @brief Battery monitor task function
 * @param pvParameters Pointer to BatteryTaskConfig structure
 * 
 * This task monitors a BQ40Z80 battery gauge device and logs battery
 * status information. The task configuration is passed via pvParameters
 * to enable platform independence and multi-device support.
 * 
 * @note Task will self-delete if configuration is invalid or initialization fails
 */
void batteryMonitorTask(void *pvParameters);

// Default task configuration constants
#define BATTERY_MONITOR_TASK_PRIORITY    2
#define BATTERY_MONITOR_TASK_STACK_SIZE  1024
#define BATTERY_DEFAULT_UPDATE_INTERVAL  5000  /**< Default 5 second update interval */
#define BATTERY_DEFAULT_ADDRESS          0x0B  /**< Standard BQ40Z80 address */

#ifdef __cplusplus
}
#endif

#endif // BATTERY_MONITOR_TASK_H