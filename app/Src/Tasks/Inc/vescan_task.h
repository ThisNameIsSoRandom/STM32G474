/**
  ******************************************************************************
  * @file           : vescan_task.h
  * @brief          : VESC CAN task header for battery data transmission
  ******************************************************************************
  */

#ifndef VESCAN_TASK_H
#define VESCAN_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "freertos_types.h"

// ===============================
// Data Structure Definitions
// ===============================

// Battery data structure for inter-task communication
typedef struct __attribute__((packed)) {
    uint16_t voltage_mv;      // Battery voltage in millivolts
    int16_t current_ma;       // Current in milliamps (positive = charging)
    uint8_t soc_percent;      // State of charge (0-100%)
    uint8_t soh_percent;      // State of health (0-100%)
    uint16_t temp_deciK;      // Temperature in 0.1 Kelvin (2731 = 0°C)
    uint16_t device_type;     // BQ40Z80 device type
    uint16_t fw_version;      // Firmware version
    uint32_t timestamp;       // System tick count when data was read
} BatteryTelemetryData;

// Command structure for vescan task control
typedef enum {
    VESCAN_CMD_ENABLE_LOGGING = 0,
    VESCAN_CMD_DISABLE_LOGGING = 1,
    VESCAN_CMD_SET_INTERVAL = 2,
    VESCAN_CMD_SEND_LAYOUT = 3
} VescanCommand;

typedef struct {
    VescanCommand command;
    uint32_t parameter;  // Command-specific parameter (e.g., interval in ms)
} VescanControlMessage;

// Data layout descriptor for receivers
typedef struct __attribute__((packed)) {
    uint8_t version;          // Protocol version (1)
    uint8_t field_count;      // Number of fields (10)
    struct {
        uint8_t field_id;     // Field identifier
        uint8_t field_type;   // 0=uint8, 1=int8, 2=uint16, 3=int16, 4=uint32, 5=int32
        uint8_t field_scale;  // Scale factor (0=none, 1=x10, 2=x100, 3=x1000)
        uint8_t field_unit;   // 0=none, 1=mV, 2=mA, 3=%, 4=K, 5=count
    } fields[10];
} DataLayoutDescriptor;

// ===============================
// Global Queue Handles
// ===============================

extern QueueHandle_t vescanDataQueue;     // Queue for battery data
extern QueueHandle_t vescanControlQueue;  // Queue for control commands

// ===============================
// Task Function
// ===============================

void vescanTask(void *pvParameters);

// ===============================
// Helper Functions
// ===============================

// Initialize queues (called before task creation)
void vescanInitQueues(void);

// Send battery data to vescan task (called from smbus_task)
BaseType_t vescanSendBatteryData(const BatteryTelemetryData *data);

// Send control command to vescan task
BaseType_t vescanSendCommand(VescanCommand cmd, uint32_t param);

#ifdef __cplusplus
}
#endif

#endif /* VESCAN_TASK_H */