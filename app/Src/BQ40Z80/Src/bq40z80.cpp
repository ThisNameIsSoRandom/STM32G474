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
    config.deviceAddress = 0x0B;              // BQ40Z80 default 7-bit address (0x16 write, 0x17 read in 8-bit)
    config.commandDelayMs = 1;             // 1ms between commands
    return config;
}

HAL_StatusTypeDef Driver::init() {
    SEGGER_RTT_printf(0, "BQ40Z80: Initializing battery gauge at address 0x0B\n");
    
    // Test basic communication
    uint16_t batteryMode;
    HAL_StatusTypeDef status = readWord(0x03, batteryMode);
    
    if (status == HAL_OK) {
        if (batteryMode == 0x6081) {
            SEGGER_RTT_printf(0, "BQ40Z80: Device functional - BatteryMode: 0x%04X\n", batteryMode);
            return HAL_OK;
        } else if (batteryMode == 0x16CC) {
            SEGGER_RTT_printf(0, "BQ40Z80: Device frozen - all SBS registers return 0x16CC\n");
            SEGGER_RTT_printf(0, "BQ40Z80: Attempting recovery sequence...\n");
            
            // Try recovery commands
            manufacturerCommand(0x0041); // Device Reset
            HAL_Delay_MS(500);
            manufacturerCommand(0x0414); // Unseal key 1
            HAL_Delay_MS(10);
            manufacturerCommand(0x3672); // Unseal key 2
            HAL_Delay_MS(100);
            
            // Test if recovery worked
            uint16_t testMode;
            if (readWord(0x03, testMode) == HAL_OK && testMode != 0x16CC) {
                SEGGER_RTT_printf(0, "BQ40Z80: Recovery successful\n");
                return HAL_OK;
            } else {
                SEGGER_RTT_printf(0, "BQ40Z80: Recovery failed - device firmware corrupted\n");
            }
        }
    } else {
        SEGGER_RTT_printf(0, "BQ40Z80: Communication failed (status: %d)\n", status);
    }
    
    SEGGER_RTT_printf(0, "BQ40Z80: Initialization complete (limited functionality)\n");
    return HAL_OK;
}

// Read uint16_t values - try standard SBS first, fall back to ManufacturerAccess if needed
HAL_StatusTypeDef Driver::read(Reading what, uint16_t& value) {
    // First try standard SBS commands as per the reference library
    HAL_StatusTypeDef status = readWord(static_cast<uint8_t>(what), value);
    
    if (status == HAL_OK) {
        // Check if we're getting a constant wrong value (0x16CC indicates sealed/wrong address)
        if (value == 0x16CC) {
            SEGGER_RTT_printf(0, "BQ40Z80: SBS register 0x%02X returned constant 0x16CC, trying MAC fallback\n", 
                             static_cast<uint8_t>(what));
            
            // Try ManufacturerBlockAccess (0x44) since ManufacturerData (0x23) is frozen
            SEGGER_RTT_printf(0, "BQ40Z80: Trying ManufacturerBlockAccess for register 0x%02X\n", 
                             static_cast<uint8_t>(what));
            return manufacturerBlockAccessRead(static_cast<uint8_t>(what), value);
        }
        return HAL_OK;
    }
    
    // If SBS command failed completely, try ManufacturerBlockAccess fallback
    SEGGER_RTT_printf(0, "BQ40Z80: SBS command 0x%02X failed, trying ManufacturerBlockAccess\n", 
                     static_cast<uint8_t>(what));
    return manufacturerBlockAccessRead(static_cast<uint8_t>(what), value);
}

// Read int16_t values - using standard SMBus registers 
HAL_StatusTypeDef Driver::read(Reading what, int16_t& value) {
    uint16_t rawValue;
    HAL_StatusTypeDef status = read(what, rawValue); // Use the uint16_t version with standard SMBus
    if (status == HAL_OK) {
        value = static_cast<int16_t>(rawValue);
    }
    return status;
}

// Read uint8_t values - using standard SMBus registers
HAL_StatusTypeDef Driver::read(Reading what, uint8_t& value) {
    uint16_t rawValue;
    HAL_StatusTypeDef status = read(what, rawValue); // Use the uint16_t version with standard SMBus
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

// Read Status - try standard SBS first, then ManufacturerAccess fallback
HAL_StatusTypeDef Driver::read(Reading what, Status& value) {
    if (what != Reading::BatteryStatus) {
        return HAL_ERROR;
    }
    
    uint16_t rawStatus;
    // First try standard SBS BatteryStatus register
    HAL_StatusTypeDef status = readWord(static_cast<uint8_t>(Reading::BatteryStatus), rawStatus);
    
    // If standard register returns constant 0x16CC, try ManufacturerBlockAccess fallback
    if (status == HAL_OK && rawStatus == 0x16CC) {
        SEGGER_RTT_printf(0, "BQ40Z80: SBS BatteryStatus returned 0x16CC, trying ManufacturerBlockAccess\n");
        status = manufacturerBlockAccessRead(static_cast<uint8_t>(Reading::BatteryStatus), rawStatus);
    }
    
    if (status == HAL_OK) {
        // Parse standard SBS BatteryStatus bits (same format for both SBS and MAC)
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
        
        SEGGER_RTT_printf(0, "BQ40Z80: BatteryStatus read: 0x%04X\n", rawStatus);
    } else {
        SEGGER_RTT_printf(0, "BQ40Z80: Failed to read BatteryStatus (tried both SBS and MAC)\n");
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
    
    // Try to read CycleCount, but don't fail if it's problematic
    status = read(Reading::CycleCount, value.cycleCount);
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "BQ40Z80: Warning - CycleCount read failed, setting to 0\n");
        value.cycleCount = 0;  // Set to 0 if read fails
    }
    applyCommandDelay();
    
    // Read battery status using standard SMBus
    status = read(Reading::BatteryStatus, value.status);
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "BQ40Z80: Failed to read battery status in AllBatteryData\n");
        return status; // Status is critical, so fail if it can't be read
    }
    
    return HAL_OK;  // Return success even if CycleCount failed
}

// Write uint16_t values
HAL_StatusTypeDef Driver::write(Setting what, uint16_t value) {
    return writeWord(static_cast<uint8_t>(what), value);
}

HAL_StatusTypeDef Driver::reset() {
    HAL_StatusTypeDef status = manufacturerCommand(0x0041); // Use correct DeviceReset MAC command
    if (status == HAL_OK) {
        vTaskDelay(100); // Reset delay
    }
    return status;
}

// ManufacturerAccess command reading - public API (using ManufacturerBlockAccess)
HAL_StatusTypeDef Driver::readMAC(MACCommand command, uint16_t& value) {
    return manufacturerBlockAccessRead(static_cast<uint16_t>(command), value);
}

// ManufacturerAccess command writing - public API  
HAL_StatusTypeDef Driver::writeMAC(MACCommand command, uint16_t value) {
    if (value == 0) {
        // Command only (no data)
        return manufacturerCommand(static_cast<uint16_t>(command));
    } else {
        // Command with data - send command first, then data
        HAL_StatusTypeDef status = manufacturerCommand(static_cast<uint16_t>(command));
        if (status == HAL_OK) {
            applyCommandDelay();
            status = writeWord(0x00, value); // Write data to ManufacturerAccess register
        }
        return status;
    }
}


void Driver::printBatteryReport() {
    SEGGER_RTT_printf(0, "\n");
    SEGGER_RTT_printf(0, "================== BQ40Z80 BATTERY STATUS REPORT ==================\n");
    
    // Read all battery data
    BatteryData data;
    HAL_StatusTypeDef status = read(Reading::AllBatteryData, data);
    
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "ERROR: Failed to read complete battery data (status: %d)\n", status);
        SEGGER_RTT_printf(0, "====================================================================\n\n");
        return;
    }
    
    // Basic measurements
    SEGGER_RTT_printf(0, "BASIC MEASUREMENTS:\n");
    SEGGER_RTT_printf(0, "  Voltage:           %d.%03d V\n", data.voltage / 1000, data.voltage % 1000);
    SEGGER_RTT_printf(0, "  Current:           %d mA", data.current);
    if (data.current > 0) {
        SEGGER_RTT_printf(0, " (CHARGING)\n");
    } else if (data.current < 0) {
        SEGGER_RTT_printf(0, " (DISCHARGING)\n");
    } else {
        SEGGER_RTT_printf(0, " (IDLE)\n");
    }
    SEGGER_RTT_printf(0, "  Temperature:       %.1f°C\n", temperatureToC(data.temperature));
    SEGGER_RTT_printf(0, "\n");
    
    // Capacity information
    SEGGER_RTT_printf(0, "CAPACITY:\n");
    SEGGER_RTT_printf(0, "  State of Charge:   %d%%\n", data.stateOfCharge);
    SEGGER_RTT_printf(0, "  Remaining:         %d mAh\n", data.remainingCapacity);
    SEGGER_RTT_printf(0, "  Full Capacity:     %d mAh\n", data.fullChargeCapacity);
    if (data.cycleCount == 0) {
        SEGGER_RTT_printf(0, "  Cycle Count:       N/A (read failed)\n");
    } else {
        SEGGER_RTT_printf(0, "  Cycle Count:       %d cycles\n", data.cycleCount);
    }
    
    // Calculate capacity health
    if (data.fullChargeCapacity > 0) {
        // Assume design capacity is around 3000mAh based on current readings
        uint16_t designCapacity = 3000;
        float health = (float)data.fullChargeCapacity / designCapacity * 100.0f;
        SEGGER_RTT_printf(0, "  Battery Health:    %.1f%% (estimated)\n", health);
    }
    SEGGER_RTT_printf(0, "\n");
    
    // Status flags
    SEGGER_RTT_printf(0, "STATUS FLAGS:\n");
    SEGGER_RTT_printf(0, "  Initialized:       %s\n", data.status.initialized ? "YES" : "NO");
    SEGGER_RTT_printf(0, "  Fully Charged:     %s\n", data.status.fullyCharged ? "YES" : "NO");
    SEGGER_RTT_printf(0, "  Fully Discharged:  %s\n", data.status.fullyDischarged ? "YES" : "NO");
    SEGGER_RTT_printf(0, "  Discharging:       %s\n", data.status.discharging ? "YES" : "NO");
    SEGGER_RTT_printf(0, "\n");
    
    // Alarms
    SEGGER_RTT_printf(0, "ALARMS:\n");
    bool anyAlarm = false;
    if (data.status.overChargedAlarm) {
        SEGGER_RTT_printf(0, "  ⚠️  OVER CHARGED ALARM\n");
        anyAlarm = true;
    }
    if (data.status.terminateChargeAlarm) {
        SEGGER_RTT_printf(0, "  ⚠️  TERMINATE CHARGE ALARM\n");
        anyAlarm = true;
    }
    if (data.status.overTempAlarm) {
        SEGGER_RTT_printf(0, "  ⚠️  OVER TEMPERATURE ALARM\n");
        anyAlarm = true;
    }
    if (data.status.terminateDischargeAlarm) {
        SEGGER_RTT_printf(0, "  ⚠️  TERMINATE DISCHARGE ALARM\n");
        anyAlarm = true;
    }
    if (data.status.remainingCapacityAlarm) {
        SEGGER_RTT_printf(0, "  ⚠️  LOW CAPACITY ALARM\n");
        anyAlarm = true;
    }
    if (data.status.remainingTimeAlarm) {
        SEGGER_RTT_printf(0, "  ⚠️  LOW TIME ALARM\n");
        anyAlarm = true;
    }
    if (!anyAlarm) {
        SEGGER_RTT_printf(0, "  ✅ No active alarms\n");
    }
    if (data.status.errorCode != 0) {
        SEGGER_RTT_printf(0, "  ❌ Error Code: 0x%X\n", data.status.errorCode);
    }
    SEGGER_RTT_printf(0, "\n");
    
    // Calculated information
    SEGGER_RTT_printf(0, "CALCULATED INFO:\n");
    
    // Estimated runtime
    if (data.current < 0) {  // Discharging
        uint32_t runtime_minutes = (uint32_t)data.remainingCapacity * 60 / (-data.current);
        uint32_t hours = runtime_minutes / 60;
        uint32_t minutes = runtime_minutes % 60;
        SEGGER_RTT_printf(0, "  Est. Runtime:      %d hours, %d minutes\n", hours, minutes);
    } else if (data.current > 0 && data.stateOfCharge < 100) {  // Charging
        uint32_t charge_time_minutes = (uint32_t)(data.fullChargeCapacity - data.remainingCapacity) * 60 / data.current;
        uint32_t hours = charge_time_minutes / 60;
        uint32_t minutes = charge_time_minutes % 60;
        SEGGER_RTT_printf(0, "  Est. Charge Time:  %d hours, %d minutes\n", hours, minutes);
    } else {
        SEGGER_RTT_printf(0, "  Runtime:           N/A (idle/full)\n");
    }
    
    // Power calculation
    uint32_t power_mW = (uint32_t)data.voltage * abs(data.current) / 1000;
    SEGGER_RTT_printf(0, "  Current Power:     %d.%03d W", power_mW / 1000, power_mW % 1000);
    if (data.current != 0) {
        SEGGER_RTT_printf(0, " (%s)", data.current > 0 ? "charging" : "discharging");
    }
    SEGGER_RTT_printf(0, "\n");
    
    // Energy calculations
    uint32_t energy_remaining_Wh = (uint32_t)data.remainingCapacity * data.voltage / 1000000;
    uint32_t energy_full_Wh = (uint32_t)data.fullChargeCapacity * data.voltage / 1000000;
    SEGGER_RTT_printf(0, "  Energy Remaining:  %d.%03d Wh\n", energy_remaining_Wh / 1000, energy_remaining_Wh % 1000);
    SEGGER_RTT_printf(0, "  Energy Full:       %d.%03d Wh\n", energy_full_Wh / 1000, energy_full_Wh % 1000);
    
    SEGGER_RTT_printf(0, "====================================================================\n\n");
}

} // namespace BQ40Z80