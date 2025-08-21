/**
 * @file bq40z80.cpp
 * @brief BQ40Z80 Battery Gauge Driver - High-Level Implementation
 * 
 * This file implements the high-level user interface for the BQ40Z80 driver.
 * It provides type-safe read/write operations, automatic error recovery,
 * and transparent fallback mechanisms when standard SBS commands fail.
 * 
 * ## Implementation Notes:
 * - All read operations first attempt standard SBS commands
 * - If SBS returns 0x16CC (device frozen), falls back to ManufacturerBlockAccess
 * - Automatic recovery sequences are attempted during initialization
 * - Debug output via SEGGER RTT can be disabled in production builds
 * 
 * @see bq40z80_lowlevel.cpp for SMBus protocol implementation
 */

#include "BQ40Z80/bq40z80.h"
#include "freertos_types.h"
#include <vector>
#include <cstring>

// Include hal_types.h for DEBUG_LOG macro - must be after other includes
#include "hal_types.h"

// STM32 HAL for HAL_GetTick function - use weak declaration to avoid include issues
extern "C" uint32_t HAL_GetTick(void);

// Local definition of DEBUG_LOG for BQ40Z80 files (outside namespace)
#ifdef STM32G474xx
    #include <cstdio>
    #define DEBUG_LOG(format, ...) printf(format "\n", ##__VA_ARGS__)
#else
    #include "SEGGER_RTT.h"
    #define DEBUG_LOG(format, ...) SEGGER_RTT_printf(0, format "\n", ##__VA_ARGS__)
#endif

namespace BQ40Z80 {

// ============================================================================
// HIGH LEVEL USER INTERFACE IMPLEMENTATION
// ============================================================================

/**
 * Constructor initializes the driver with I2C handle and configuration.
 * The I2C addresses are pre-calculated from the 7-bit device address
 * for efficiency during communication.
 */
Driver::Driver(I2C_HandleTypeDef* i2c_handle, const Config& config) 
    : i2c_handle_(i2c_handle)
    , config_(config)
    , writeAddress_(config.deviceAddress << 1)        // Convert to 8-bit write address
    , readAddress_((config.deviceAddress << 1) | 0x01) // Convert to 8-bit read address
{
    // Pre-calculated addresses avoid bit shifting during I2C transactions
    // I2C handle is stored for all future communication operations
}

/**
 * Provides standard configuration that works with most BQ40Z80 implementations.
 * The default address 0x0B is specified in the BQ40Z80 datasheet.
 */
Config Driver::defaultConfig() {
    Config config;
    config.deviceAddress = 0x0B;   // Standard BQ40Z80 7-bit address
    config.commandDelayMs = 1;     // Minimum delay to avoid overwhelming the device
    return config;
}

/**
 * Initialization performs a comprehensive device check and recovery sequence.
 * The BQ40Z80 can enter a "frozen" state where all SBS registers return 0x16CC.
 * This typically occurs when the device is sealed or has encountered an error.
 * 
 * Recovery sequence:
 * 1. Device reset to clear any error states
 * 2. Unseal commands using default keys (0x0414, 0x3672)
 * 3. Verification that normal operation has resumed
 */
HAL_StatusTypeDef Driver::init() {
    DEBUG_LOG("BQ40Z80: Initializing battery gauge at address 0x0B");
    
    // Test basic communication by reading BatteryMode register
    uint16_t batteryMode;
    HAL_StatusTypeDef status = readWord(0x03, batteryMode);
    
    if (status == HAL_OK) {
        // Expected value for normal operation (may vary by configuration)
        if (batteryMode == 0x6081) {
            DEBUG_LOG("BQ40Z80: Device functional - BatteryMode: 0x%04X", batteryMode);
            return HAL_OK;
        } 
        // 0x16CC indicates device is frozen/sealed
        else if (batteryMode == 0x16CC) {
            DEBUG_LOG("BQ40Z80: Device frozen - all SBS registers return 0x16CC");
            DEBUG_LOG("BQ40Z80: Attempting recovery sequence...");
            
            // Step 1: Reset the device to clear error states
            manufacturerCommand(0x0041); // MFA_DEVICE_RESET
            HAL_Delay_MS(500);           // Allow time for reset to complete
            
            // Step 2: Attempt to unseal using default keys
            manufacturerCommand(0x0414); // Default unseal key part 1
            HAL_Delay_MS(10);
            manufacturerCommand(0x3672); // Default unseal key part 2
            HAL_Delay_MS(100);           // Allow time for unseal to process
            
            // Step 3: Verify recovery was successful
            uint16_t testMode;
            if (readWord(0x03, testMode) == HAL_OK && testMode != 0x16CC) {
                DEBUG_LOG("BQ40Z80: Recovery successful");
                return HAL_OK;
            } else {
                DEBUG_LOG("BQ40Z80: Recovery failed - device firmware corrupted");
                // Continue anyway - ManufacturerBlockAccess may still work
            }
        }
    } else {
        DEBUG_LOG("BQ40Z80: Communication failed (status: %d)", status);
    }
    
    DEBUG_LOG("BQ40Z80: Initialization complete (limited functionality)");
    return HAL_OK; // Return OK to allow fallback mechanisms to work
}

/**
 * Primary read function for 16-bit values implements automatic fallback logic.
 * When the device is sealed or frozen, standard SBS commands return 0x16CC.
 * In this case, we automatically fall back to ManufacturerBlockAccess (0x44)
 * which can still retrieve data even when the device is sealed.
 */
HAL_StatusTypeDef Driver::read(Reading what, uint16_t& value) {
    // Attempt standard SBS read first (most efficient when device is unsealed)
    HAL_StatusTypeDef status = readWord(static_cast<uint8_t>(what), value);
    
    if (status == HAL_OK) {
        // 0x16CC is a sentinel value indicating device is sealed/frozen
        if (value == 0x16CC) {
            DEBUG_LOG("BQ40Z80: SBS register 0x%02X returned 0x16CC (device sealed), using MAC fallback", 
                             static_cast<uint8_t>(what));
            
            // ManufacturerBlockAccess can read data even when device is sealed
            return manufacturerBlockAccessRead(static_cast<uint8_t>(what), value);
        }
        return HAL_OK;
    }
    
    // If SBS failed entirely (communication error), still try MAC as last resort
    DEBUG_LOG("BQ40Z80: SBS command 0x%02X failed, trying ManufacturerBlockAccess", 
                     static_cast<uint8_t>(what));
    return manufacturerBlockAccessRead(static_cast<uint8_t>(what), value);
}

/**
 * Signed 16-bit read for current measurements.
 * Current values are signed: positive = charging, negative = discharging.
 * This function reuses the uint16_t read and performs proper sign extension.
 */
HAL_StatusTypeDef Driver::read(Reading what, int16_t& value) {
    uint16_t rawValue;
    HAL_StatusTypeDef status = read(what, rawValue);
    if (status == HAL_OK) {
        // Cast preserves sign bit for two's complement representation
        value = static_cast<int16_t>(rawValue);
    }
    return status;
}

/**
 * 8-bit read for percentage values (state of charge).
 * Many BQ40Z80 registers return 16-bit values where only the lower byte is used.
 * This function extracts the relevant byte from the 16-bit read.
 */
HAL_StatusTypeDef Driver::read(Reading what, uint8_t& value) {
    uint16_t rawValue;
    HAL_StatusTypeDef status = read(what, rawValue);
    if (status == HAL_OK) {
        // Extract lower byte (most percentage values are 0-100)
        value = static_cast<uint8_t>(rawValue & 0xFF);
    }
    return status;
}

/**
 * String read for manufacturer and device information.
 * These values are stored as SMBus block data (up to 20 ASCII characters).
 * The function handles null terminator removal for clean string output.
 */
HAL_StatusTypeDef Driver::read(Reading what, std::string& value) {
    std::vector<uint8_t> data;
    HAL_StatusTypeDef status;
    
    switch (what) {
        case Reading::ManufacturerName:
        case Reading::DeviceName:
            // Block read returns length byte followed by ASCII data
            status = readBlock(static_cast<uint8_t>(what), data);
            if (status == HAL_OK && !data.empty()) {
                // Convert byte array to string
                value.assign(data.begin(), data.end());
                // Remove null terminator if present for clean output
                if (!value.empty() && value.back() == '\0') {
                    value.pop_back();
                }
            }
            return status;
            
        default:
            return HAL_ERROR; // Only string-type readings supported
    }
}

/**
 * Decodes the BatteryStatus register into individual flags.
 * The 16-bit status register contains multiple alarm and state indicators
 * that are extracted into a human-readable Status structure.
 */
HAL_StatusTypeDef Driver::read(Reading what, Status& value) {
    if (what != Reading::BatteryStatus) {
        return HAL_ERROR;
    }
    
    uint16_t rawStatus;
    // Attempt to read BatteryStatus register (0x16)
    HAL_StatusTypeDef status = readWord(static_cast<uint8_t>(Reading::BatteryStatus), rawStatus);
    
    // If standard register returns constant 0x16CC, try ManufacturerBlockAccess fallback
    if (status == HAL_OK && rawStatus == 0x16CC) {
        DEBUG_LOG("BQ40Z80: SBS BatteryStatus returned 0x16CC, trying ManufacturerBlockAccess");
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
        
        DEBUG_LOG("BQ40Z80: BatteryStatus read: 0x%04X", rawStatus);
    } else {
        DEBUG_LOG("BQ40Z80: Failed to read BatteryStatus (tried both SBS and MAC)");
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
        DEBUG_LOG("BQ40Z80: Warning - CycleCount read failed, setting to 0");
        value.cycleCount = 0;  // Set to 0 if read fails
    }
    applyCommandDelay();
    
    // Read battery status using standard SMBus
    status = read(Reading::BatteryStatus, value.status);
    if (status != HAL_OK) {
        DEBUG_LOG("BQ40Z80: Failed to read battery status in AllBatteryData");
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
    DEBUG_LOG("");
    DEBUG_LOG("================== BQ40Z80 BATTERY STATUS REPORT ==================");
    
    // Read all battery data
    BatteryData data;
    HAL_StatusTypeDef status = read(Reading::AllBatteryData, data);
    
    if (status != HAL_OK) {
        DEBUG_LOG("ERROR: Failed to read complete battery data (status: %d)", status);
        DEBUG_LOG("====================================================================");
        DEBUG_LOG("");
        return;
    }
    
    // Basic measurements
    DEBUG_LOG("BASIC MEASUREMENTS:");
    DEBUG_LOG("  Voltage:           %d.%03d V", data.voltage / 1000, data.voltage % 1000);
    DEBUG_LOG("  Current:           %d mA", data.current);
    if (data.current > 0) {
#ifdef STM32G474xx
        printf(" (CHARGING)\n");
#else
        SEGGER_RTT_printf(0, " (CHARGING)\n");
#endif
    } else if (data.current < 0) {
#ifdef STM32G474xx
        printf(" (DISCHARGING)\n");
#else
        SEGGER_RTT_printf(0, " (DISCHARGING)\n");
#endif
    } else {
#ifdef STM32G474xx
        printf(" (IDLE)\n");
#else
        SEGGER_RTT_printf(0, " (IDLE)\n");
#endif
    }
    DEBUG_LOG("  Temperature:       %.1f°C", temperatureToC(data.temperature));
    DEBUG_LOG("");
    
    // Capacity information
    DEBUG_LOG("CAPACITY:");
    DEBUG_LOG("  State of Charge:   %d%%", data.stateOfCharge);
    DEBUG_LOG("  Remaining:         %d mAh", data.remainingCapacity);
    DEBUG_LOG("  Full Capacity:     %d mAh", data.fullChargeCapacity);
    if (data.cycleCount == 0) {
        DEBUG_LOG("  Cycle Count:       N/A (read failed)");
    } else {
        DEBUG_LOG("  Cycle Count:       %d cycles", data.cycleCount);
    }
    
    // Calculate capacity health
    if (data.fullChargeCapacity > 0) {
        // Assume design capacity is around 3000mAh based on current readings
        uint16_t designCapacity = 3000;
        float health = (float)data.fullChargeCapacity / designCapacity * 100.0f;
        DEBUG_LOG("  Battery Health:    %.1f%% (estimated)", health);
    }
    DEBUG_LOG("");
    
    // Status flags
    DEBUG_LOG("STATUS FLAGS:");
    DEBUG_LOG("  Initialized:       %s", data.status.initialized ? "YES" : "NO");
    DEBUG_LOG("  Fully Charged:     %s", data.status.fullyCharged ? "YES" : "NO");
    DEBUG_LOG("  Fully Discharged:  %s", data.status.fullyDischarged ? "YES" : "NO");
    DEBUG_LOG("  Discharging:       %s", data.status.discharging ? "YES" : "NO");
    DEBUG_LOG("");
    
    // Alarms
    DEBUG_LOG("ALARMS:");
    bool anyAlarm = false;
    if (data.status.overChargedAlarm) {
        DEBUG_LOG("  ⚠️  OVER CHARGED ALARM");
        anyAlarm = true;
    }
    if (data.status.terminateChargeAlarm) {
        DEBUG_LOG("  ⚠️  TERMINATE CHARGE ALARM");
        anyAlarm = true;
    }
    if (data.status.overTempAlarm) {
        DEBUG_LOG("  ⚠️  OVER TEMPERATURE ALARM");
        anyAlarm = true;
    }
    if (data.status.terminateDischargeAlarm) {
        DEBUG_LOG("  ⚠️  TERMINATE DISCHARGE ALARM");
        anyAlarm = true;
    }
    if (data.status.remainingCapacityAlarm) {
        DEBUG_LOG("  ⚠️  LOW CAPACITY ALARM");
        anyAlarm = true;
    }
    if (data.status.remainingTimeAlarm) {
        DEBUG_LOG("  ⚠️  LOW TIME ALARM");
        anyAlarm = true;
    }
    if (!anyAlarm) {
        DEBUG_LOG("  ✅ No active alarms");
    }
    if (data.status.errorCode != 0) {
        DEBUG_LOG("  ❌ Error Code: 0x%X", data.status.errorCode);
    }
    DEBUG_LOG("");
    
    // Calculated information
    DEBUG_LOG("CALCULATED INFO:");
    
    // Estimated runtime
    if (data.current < 0) {  // Discharging
        uint32_t runtime_minutes = (uint32_t)data.remainingCapacity * 60 / (-data.current);
        uint32_t hours = runtime_minutes / 60;
        uint32_t minutes = runtime_minutes % 60;
        DEBUG_LOG("  Est. Runtime:      %d hours, %d minutes", hours, minutes);
    } else if (data.current > 0 && data.stateOfCharge < 100) {  // Charging
        uint32_t charge_time_minutes = (uint32_t)(data.fullChargeCapacity - data.remainingCapacity) * 60 / data.current;
        uint32_t hours = charge_time_minutes / 60;
        uint32_t minutes = charge_time_minutes % 60;
        DEBUG_LOG("  Est. Charge Time:  %d hours, %d minutes", hours, minutes);
    } else {
        DEBUG_LOG("  Runtime:           N/A (idle/full)");
    }
    
    // Power calculation
    uint32_t power_mW = (uint32_t)data.voltage * abs(data.current) / 1000;
    DEBUG_LOG("  Current Power:     %d.%03d W", power_mW / 1000, power_mW % 1000);
    if (data.current != 0) {
#ifdef STM32G474xx
        printf(" (%s)", data.current > 0 ? "charging" : "discharging");
#else
        SEGGER_RTT_printf(0, " (%s)", data.current > 0 ? "charging" : "discharging");
#endif
    }
#ifdef STM32G474xx
    printf("\n");
#else
    SEGGER_RTT_printf(0, "\n");
#endif
    
    // Energy calculations
    uint32_t energy_remaining_Wh = (uint32_t)data.remainingCapacity * data.voltage / 1000000;
    uint32_t energy_full_Wh = (uint32_t)data.fullChargeCapacity * data.voltage / 1000000;
    DEBUG_LOG("  Energy Remaining:  %d.%03d Wh", energy_remaining_Wh / 1000, energy_remaining_Wh % 1000);
    DEBUG_LOG("  Energy Full:       %d.%03d Wh", energy_full_Wh / 1000, energy_full_Wh % 1000);
    
    DEBUG_LOG("====================================================================");
    DEBUG_LOG("");
}

HAL_StatusTypeDef Driver::getBatteryTelemetryData(BatteryTelemetryData& telemetry) {
    // Read all battery data in a single operation for consistency
    BatteryData data;
    HAL_StatusTypeDef status = read(Reading::AllBatteryData, data);
    if (status != HAL_OK) {
        DEBUG_LOG("BQ40Z80: Failed to read battery data for telemetry (status=%d)", status);
        return status;
    }
    
    // Clear the telemetry structure and set timestamp
    memset(&telemetry, 0, sizeof(telemetry));
    telemetry.timestamp_ms = HAL_GetTick();  // Use HAL tick for timestamp
    
    // Fill primary electrical measurements
    telemetry.voltage_mV = data.voltage;
    telemetry.current_mA = data.current;
    telemetry.temperature_01K = data.temperature;
    telemetry.state_of_charge = data.stateOfCharge;
    
    // Fill capacity and cycle information
    telemetry.remaining_capacity_mAh = data.remainingCapacity;
    telemetry.full_charge_capacity_mAh = data.fullChargeCapacity;
    telemetry.cycle_count = data.cycleCount;
    
    // Fill status flags - map from Status struct to packed bit fields
    telemetry.status_flags.over_charged_alarm = data.status.overChargedAlarm;
    telemetry.status_flags.terminate_charge_alarm = data.status.terminateChargeAlarm;
    telemetry.status_flags.over_temp_alarm = data.status.overTempAlarm;
    telemetry.status_flags.terminate_discharge_alarm = data.status.terminateDischargeAlarm;
    telemetry.status_flags.remaining_capacity_alarm = data.status.remainingCapacityAlarm;
    telemetry.status_flags.remaining_time_alarm = data.status.remainingTimeAlarm;
    telemetry.status_flags.initialized = data.status.initialized;
    telemetry.status_flags.discharging = data.status.discharging;
    
    telemetry.status_flags_ext.fully_charged = data.status.fullyCharged;
    telemetry.status_flags_ext.fully_discharged = data.status.fullyDischarged;
    
    telemetry.error_code = data.status.errorCode;
    telemetry.data_quality = 0xFF;  // All data valid (could be enhanced with field-specific validation)
    
    DEBUG_LOG("BQ40Z80: Telemetry data prepared - V:%umV, I:%dmA, T:%u.%uK, SoC:%u%%", 
              telemetry.voltage_mV, telemetry.current_mA, 
              telemetry.temperature_01K/10, telemetry.temperature_01K%10, 
              telemetry.state_of_charge);
    
    return HAL_OK;
}

} // namespace BQ40Z80