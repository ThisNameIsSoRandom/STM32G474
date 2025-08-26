/**
  ******************************************************************************
  * @file           : vescan_task.cpp
  * @brief          : VESC CAN task for battery telemetry transmission
  ******************************************************************************
  */

#include "vescan_task.h"
#include "hal_types.h"
#include "freertos_types.h"
#include "VESC.h"

// Platform detection for CAN vs FDCAN
#ifdef STM32G474xx
    #define PLATFORM_USES_FDCAN 1
    extern FDCAN_HandleTypeDef hfdcan1;
    #pragma message "Using FDCAN for STM32G474xx"
#elif defined(STM32U575xx)
    #define PLATFORM_USES_FDCAN 0  
    extern CAN_HandleTypeDef hcan1;
    #pragma message "Using CAN for STM32U575xx"
#else
    #define PLATFORM_USES_FDCAN 0
    extern CAN_HandleTypeDef hcan1;
    #pragma message "Using CAN for unknown platform"
#endif

extern "C" {

// ===============================
// Global Queue Handles
// ===============================

QueueHandle_t vescanDataQueue = nullptr;
QueueHandle_t vescanControlQueue = nullptr;

// ===============================
// Task Configuration
// ===============================

#define VESCAN_DATA_QUEUE_LENGTH    10
#define VESCAN_CONTROL_QUEUE_LENGTH 5
#define VESCAN_DEFAULT_INTERVAL_MS  100
#define VESCAN_CAN_ID_BASE          0x100  // Base CAN ID for custom frames

// ===============================
// Task State
// ===============================

static struct {
    bool logging_enabled;
    uint32_t transmission_interval_ms;
    uint32_t last_transmission_tick;
    uint32_t packet_counter;
    bool layout_sent;
} vescan_state = {
    .logging_enabled = false,
    .transmission_interval_ms = VESCAN_DEFAULT_INTERVAL_MS,
    .last_transmission_tick = 0,
    .packet_counter = 0,
    .layout_sent = false
};

// ===============================
// Platform-specific CAN Functions
// ===============================

/**
 * @brief Platform-specific CAN transmission
 */
static HAL_StatusTypeDef platformCanSend(uint32_t canId, uint8_t *data, uint32_t length)
{
#if PLATFORM_USES_FDCAN
    // Use FDCAN for STM32G474
    FDCAN_TxHeaderTypeDef txHeader;
    txHeader.Identifier = canId;
    txHeader.IdType = FDCAN_EXTENDED_ID;
    txHeader.TxFrameType = FDCAN_DATA_FRAME;
    // FDCAN data length code conversion
    uint32_t dlc_values[] = {FDCAN_DLC_BYTES_0, FDCAN_DLC_BYTES_1, FDCAN_DLC_BYTES_2, FDCAN_DLC_BYTES_3,
                            FDCAN_DLC_BYTES_4, FDCAN_DLC_BYTES_5, FDCAN_DLC_BYTES_6, FDCAN_DLC_BYTES_7,
                            FDCAN_DLC_BYTES_8};
    txHeader.DataLength = (length <= 8) ? dlc_values[length] : FDCAN_DLC_BYTES_8;
    txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    txHeader.BitRateSwitch = FDCAN_BRS_OFF;
    txHeader.FDFormat = FDCAN_CLASSIC_CAN;
    txHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    txHeader.MessageMarker = 0;
    
    return HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txHeader, data);
#else
    // Use classic CAN for other platforms
    CAN_TxHeaderTypeDef txHeader;
    txHeader.StdId = 0;
    txHeader.ExtId = canId;
    txHeader.IDE = CAN_ID_EXT;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = (length > 8) ? 8 : length;
    
    uint32_t txMailbox;
    return HAL_CAN_AddTxMessage(&hcan1, &txHeader, data, &txMailbox);
#endif
}

/**
 * @brief Initialize platform-specific CAN
 */
static HAL_StatusTypeDef initPlatformCan(void)
{
#if PLATFORM_USES_FDCAN
    // Start FDCAN
    return HAL_FDCAN_Start(&hfdcan1);
#else
    // Start classic CAN
    return HAL_CAN_Start(&hcan1);
#endif
}

// ===============================
// Helper Functions
// ===============================

/**
 * @brief Initialize vescan queues
 */
void vescanInitQueues(void)
{
    // Create data queue for battery telemetry
    vescanDataQueue = xQueueCreate(VESCAN_DATA_QUEUE_LENGTH, sizeof(BatteryTelemetryData));
    if (vescanDataQueue == nullptr) {
        DEBUG_LOG("VESCAN: Failed to create data queue\n\r");
    }
    
    // Create control queue for commands
    vescanControlQueue = xQueueCreate(VESCAN_CONTROL_QUEUE_LENGTH, sizeof(VescanControlMessage));
    if (vescanControlQueue == nullptr) {
        DEBUG_LOG("VESCAN: Failed to create control queue\n\r");
    }
}

/**
 * @brief Send battery data to vescan task
 */
BaseType_t vescanSendBatteryData(const BatteryTelemetryData *data)
{
    if (vescanDataQueue == nullptr || data == nullptr) {
        return pdFAIL;
    }
    
    // Don't block if queue is full - just drop the data
    return xQueueSend(vescanDataQueue, data, 0);
}

/**
 * @brief Send control command to vescan task
 */
BaseType_t vescanSendCommand(VescanCommand cmd, uint32_t param)
{
    if (vescanControlQueue == nullptr) {
        return pdFAIL;
    }
    
    VescanControlMessage msg = {
        .command = cmd,
        .parameter = param
    };
    
    return xQueueSend(vescanControlQueue, &msg, 0);
}

/**
 * @brief Send data layout descriptor over CAN
 */
static void sendDataLayout(void)
{
    DEBUG_LOG("VESCAN: Sending data layout descriptor\n\r");
    
    // Create layout descriptor
    DataLayoutDescriptor layout = {
        .version = 1,
        .field_count = 10
    };
    
    // Define field descriptors
    layout.fields[0] = {0, 2, 0, 1};  // voltage_mv: uint16, no scale, mV
    layout.fields[1] = {1, 3, 0, 2};  // current_ma: int16, no scale, mA
    layout.fields[2] = {2, 0, 0, 3};  // soc_percent: uint8, no scale, %
    layout.fields[3] = {3, 0, 0, 3};  // soh_percent: uint8, no scale, %
    layout.fields[4] = {4, 2, 1, 4};  // temp_deciK: uint16, x10 scale, K
    layout.fields[5] = {5, 2, 0, 0};  // device_type: uint16, no scale, none
    layout.fields[6] = {6, 2, 0, 0};  // fw_version: uint16, no scale, none
    layout.fields[7] = {7, 4, 0, 5};  // timestamp: uint32, no scale, count
    layout.fields[8] = {8, 0, 0, 0};  // reserved
    layout.fields[9] = {9, 0, 0, 0};  // reserved
    
    uint8_t* layoutBytes = (uint8_t*)&layout;
    
    // Send layout in multiple CAN frames if needed
    for (uint32_t offset = 0; offset < sizeof(layout); offset += 8) {
        uint8_t frameData[8] = {0};
        uint32_t bytesToCopy = (sizeof(layout) - offset) > 8 ? 8 : (sizeof(layout) - offset);
        
        for (uint32_t i = 0; i < bytesToCopy; i++) {
            frameData[i] = layoutBytes[offset + i];
        }
        
        uint32_t frameId = (VESCAN_CAN_ID_BASE | 0x01) + (offset / 8);
        
        if (platformCanSend(frameId, frameData, bytesToCopy) != HAL_OK) {
            DEBUG_LOG("VESCAN: Failed to send layout frame %d\n\r", offset / 8);
        }
        
        vTaskDelay(10);  // Small delay between frames
    }
    
    vescan_state.layout_sent = true;
}

/**
 * @brief Transmit battery data over CAN using VESC protocol
 */
static void transmitBatteryData(const BatteryTelemetryData *data)
{
    // Use VESC Status 8 frame format for custom data
    VESC_Status_8 status8;
    VESC_RawFrame rawFrame;
    
    // Map battery data to VESC Status 8 fields (creative mapping)
    status8.vescID = 0x11;  // Device ID
    status8.weightA = (float)data->voltage_mv / 10.0f;  // Voltage in 0.01V units
    status8.distance = (float)data->current_ma / 10.0f;  // Current in 0.01A units
    status8.humidity = (float)data->soc_percent * 2.0f;  // SOC scaled to float range
    status8.vibrations = (data->soh_percent > 50);  // SOH as boolean vibration level
    status8.weightB = (float)(data->temp_deciK - 2731) / 10.0f;  // Temp in Celsius
    
    // Convert to raw CAN frame
    if (!VESC_convertStatus8ToRaw(&rawFrame, &status8)) {
        DEBUG_LOG("VESCAN: Failed to convert data to VESC format\n\r");
        return;
    }
    
    // Send the CAN frame using platform-specific function
    uint32_t canId = (0x900 | (status8.vescID & 0xFF));  // VESC extended ID format
    
    if (platformCanSend(canId, rawFrame.rawData, VESC_CAN_STATUS_8_DLEN) == HAL_OK) {
        vescan_state.packet_counter++;
        
        // Log every 10th packet to reduce RTT spam
        if (vescan_state.packet_counter % 10 == 0) {
            DEBUG_LOG("VESCAN: Transmitted packet %d (V:%dmV, I:%dmA, SOC:%d%%)\n\r",
                vescan_state.packet_counter,
                data->voltage_mv,
                data->current_ma,
                data->soc_percent);
        }
    } else {
        DEBUG_LOG("VESCAN: CAN transmission failed\n\r");
    }
}


/**
 * @brief Process control command
 */
static void processControlCommand(const VescanControlMessage *cmd)
{
    switch (cmd->command) {
        case VESCAN_CMD_ENABLE_LOGGING:
            DEBUG_LOG("VESCAN: Logging enabled\n\r");
            vescan_state.logging_enabled = true;
            vescan_state.layout_sent = false;  // Send layout on next cycle
            break;
            
        case VESCAN_CMD_DISABLE_LOGGING:
            DEBUG_LOG("VESCAN: Logging disabled\n\r");
            vescan_state.logging_enabled = false;
            break;
            
        case VESCAN_CMD_SET_INTERVAL:
            vescan_state.transmission_interval_ms = cmd->parameter;
            DEBUG_LOG("VESCAN: Interval set to %d ms\n\r", cmd->parameter);
            break;
            
        case VESCAN_CMD_SEND_LAYOUT:
            sendDataLayout();
            break;
            
        default:
            DEBUG_LOG("VESCAN: Unknown command %d\n\r", cmd->command);
            break;
    }
}

/**
 * @brief VESC CAN task main function
 */
extern "C" void vescanTask(void *pvParameters)
{
    (void)pvParameters;
    
    DEBUG_LOG("VESCAN: Task started\n\r");
    
    // Initialize and start platform-specific CAN peripheral
#if PLATFORM_USES_FDCAN
    // FDCAN is already initialized by MX_FDCAN1_Init() in main
    if (initPlatformCan() != HAL_OK) {
        DEBUG_LOG("VESCAN: Failed to start FDCAN\n\r");
    }
#else
    // Initialize classic CAN
    MX_CAN1_Init();
    if (initPlatformCan() != HAL_OK) {
        DEBUG_LOG("VESCAN: Failed to start CAN\n\r");
    }
#endif
    

    // Configure CAN filter to accept all messages (for testing)
    // In production, configure specific filters for command reception
    
    BatteryTelemetryData batteryData;
    VescanControlMessage controlMsg;
    uint32_t currentTick;
    
    for(;;) {
        currentTick = xTaskGetTickCount();
        
        // Check for control commands (non-blocking)
        if (xQueueReceive(vescanControlQueue, &controlMsg, 0) == pdTRUE) {
            processControlCommand(&controlMsg);
        }
        
        // Check for new battery data (non-blocking)
        if (xQueueReceive(vescanDataQueue, &batteryData, 0) == pdTRUE) {
            // Store latest data for periodic transmission
            // Could implement a circular buffer here for averaging
        }
        
        // Periodic transmission if logging is enabled
        if (vescan_state.logging_enabled) {
            // Send layout descriptor first if not sent
            if (!vescan_state.layout_sent) {
                sendDataLayout();
            }
            
            // Check if it's time to transmit
            if ((currentTick - vescan_state.last_transmission_tick) >= 
                (vescan_state.transmission_interval_ms / portTICK_PERIOD_MS)) {
                
                // Transmit using VESC protocol format
                transmitBatteryData(&batteryData);
                
                // Alternative: transmit raw packed data
                // transmitRawBatteryData(&batteryData);
                
                vescan_state.last_transmission_tick = currentTick;
            }
        }
        
        // Task delay to prevent CPU hogging
        vTaskDelay(10);  // 10ms task period
    }
}

} // extern "C"
