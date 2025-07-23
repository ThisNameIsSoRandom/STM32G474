#include "BQ40Z80/bq40z80.h"
#include "hal_types.h"
#include "freertos_types.h"
#include "SEGGER_RTT.h"
#include <vector>
#include <cstring>

namespace BQ40Z80 {

// ============================================================================
// HIGH LEVEL USER INTERFACE IMPLEMENTATION
// ============================================================================

Driver::Driver(const Config& config) 
    : config_(config)
    , writeAddress_(config.deviceAddress << 1)
    , readAddress_((config.deviceAddress << 1) | 0x01) {
}

Config Driver::defaultConfig() {
    Config config;
    config.deviceAddress = 0x0B;           // Default 7-bit address
    config.commandDelayMs = 1;             // 1ms between commands
    return config;
}

HAL_StatusTypeDef Driver::init() {
    uint16_t deviceType;
    HAL_StatusTypeDef status = manufacturerRead(0x0001, deviceType); // DeviceType command
    
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "BQ40Z80: Failed to read device type\n");
        return status;
    }
    
    SEGGER_RTT_printf(0, "BQ40Z80: Device type: 0x%04X\n", deviceType);
    return HAL_OK;
}

// Read uint16_t values
HAL_StatusTypeDef Driver::read(Reading what, uint16_t& value) {
    switch (what) {
        case Reading::Voltage:
        case Reading::Current:
        case Reading::Temperature:
        case Reading::RemainingCapacity:
        case Reading::FullChargeCapacity:
        case Reading::CycleCount:
        case Reading::SerialNumber:
            return readWord(static_cast<uint8_t>(what), value);
            
        default:
            SEGGER_RTT_printf(0, "BQ40Z80: Reading not supported for uint16_t\n");
            return HAL_ERROR;
    }
}

// Read int16_t values
HAL_StatusTypeDef Driver::read(Reading what, int16_t& value) {
    uint16_t rawValue;
    HAL_StatusTypeDef status = read(what, rawValue);
    if (status == HAL_OK) {
        value = static_cast<int16_t>(rawValue);
    }
    return status;
}

// Read uint8_t values
HAL_StatusTypeDef Driver::read(Reading what, uint8_t& value) {
    if (what != Reading::StateOfCharge) {
        return HAL_ERROR;
    }
    
    uint16_t rawValue;
    HAL_StatusTypeDef status = readWord(static_cast<uint8_t>(what), rawValue);
    if (status == HAL_OK) {
        value = static_cast<uint8_t>(rawValue & 0xFF);
    }
    return status;
}

// Read string values
HAL_StatusTypeDef Driver::read(Reading what, std::string& value) {
    std::vector<uint8_t> data;
    HAL_StatusTypeDef status;
    
    switch (what) {
        case Reading::ManufacturerName:
        case Reading::DeviceName:
            status = readBlock(static_cast<uint8_t>(what), data);
            if (status == HAL_OK && !data.empty()) {
                value.assign(data.begin(), data.end());
                // Remove null terminator if present
                if (!value.empty() && value.back() == '\0') {
                    value.pop_back();
                }
            }
            return status;
            
        default:
            return HAL_ERROR;
    }
}

// Read Status
HAL_StatusTypeDef Driver::read(Reading what, Status& value) {
    if (what != Reading::AllBatteryData) {
        return HAL_ERROR;
    }
    
    uint16_t rawStatus;
    HAL_StatusTypeDef status = readWord(0x16, rawStatus); // BatteryStatus command
    
    if (status == HAL_OK) {
        value.overChargedAlarm = (rawStatus & 0x8000) != 0;
        value.terminateChargeAlarm = (rawStatus & 0x4000) != 0;
        value.overTempAlarm = (rawStatus & 0x1000) != 0;
        value.terminateDischargeAlarm = (rawStatus & 0x0800) != 0;
        value.remainingCapacityAlarm = (rawStatus & 0x0200) != 0;
        value.remainingTimeAlarm = (rawStatus & 0x0100) != 0;
        value.initialized = (rawStatus & 0x0080) != 0;
        value.discharging = (rawStatus & 0x0040) != 0;
        value.fullyCharged = (rawStatus & 0x0020) != 0;
        value.fullyDischarged = (rawStatus & 0x0010) != 0;
        value.errorCode = rawStatus & 0x000F;
    }
    
    return status;
}

// Read BatteryData (composite)
HAL_StatusTypeDef Driver::read(Reading what, BatteryData& value) {
    if (what != Reading::AllBatteryData) {
        return HAL_ERROR;
    }
    
    HAL_StatusTypeDef status;
    
    status = read(Reading::Voltage, value.voltage);
    if (status != HAL_OK) return status;
    applyCommandDelay();
    
    status = read(Reading::Current, value.current);
    if (status != HAL_OK) return status;
    applyCommandDelay();
    
    status = read(Reading::Temperature, value.temperature);
    if (status != HAL_OK) return status;
    applyCommandDelay();
    
    status = read(Reading::StateOfCharge, value.stateOfCharge);
    if (status != HAL_OK) return status;
    applyCommandDelay();
    
    status = read(Reading::RemainingCapacity, value.remainingCapacity);
    if (status != HAL_OK) return status;
    applyCommandDelay();
    
    status = read(Reading::FullChargeCapacity, value.fullChargeCapacity);
    if (status != HAL_OK) return status;
    applyCommandDelay();
    
    status = read(Reading::CycleCount, value.cycleCount);
    if (status != HAL_OK) return status;
    applyCommandDelay();
    
    status = read(Reading::AllBatteryData, value.status);
    
    return status;
}

// Write uint16_t values
HAL_StatusTypeDef Driver::write(Setting what, uint16_t value) {
    return writeWord(static_cast<uint8_t>(what), value);
}

HAL_StatusTypeDef Driver::reset() {
    HAL_StatusTypeDef status = manufacturerCommand(0x0005); // ResetData command
    if (status == HAL_OK) {
        vTaskDelay(100); // Reset delay
    }
    return status;
}

} // namespace BQ40Z80