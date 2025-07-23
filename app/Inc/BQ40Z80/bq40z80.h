#ifndef BQ40Z80_H
#define BQ40Z80_H

#include <cstdint>
#include <string>
#include <vector>

// Include HAL types from app
#include "hal_types.h"

namespace BQ40Z80 {

// ============================================================================
// HIGH LEVEL USER INTERFACE - Use these functions
// ============================================================================

// What you can read from the battery
enum class Reading : uint8_t {
    Voltage              = 0x09,  // Battery voltage in mV
    Current              = 0x0A,  // Current in mA (+ = charging, - = discharging)  
    Temperature          = 0x08,  // Temperature in 0.1K units
    StateOfCharge        = 0x0D,  // State of charge in %
    RemainingCapacity    = 0x0F,  // Remaining capacity in mAh
    FullChargeCapacity   = 0x10,  // Full charge capacity in mAh
    CycleCount           = 0x17,  // Number of charge cycles
    ManufacturerName     = 0x20,  // Battery manufacturer name
    DeviceName           = 0x21,  // Device/model name
    SerialNumber         = 0x1C,  // Battery serial number
    AllBatteryData       = 0xFF   // Read all battery parameters at once
};

// What you can write to the battery
enum class Setting : uint8_t {
    BatteryMode          = 0x03,  // Battery operating mode
    ChargingCurrent      = 0x14,  // Maximum charging current in mA
    ChargingVoltage      = 0x15   // Maximum charging voltage in mV
};

// Battery status information
struct Status {
    bool overChargedAlarm;
    bool terminateChargeAlarm; 
    bool overTempAlarm;
    bool terminateDischargeAlarm;
    bool remainingCapacityAlarm;
    bool remainingTimeAlarm;
    bool initialized;
    bool discharging;
    bool fullyCharged;
    bool fullyDischarged;
    uint8_t errorCode;
};

// Complete battery information
struct BatteryData {
    uint16_t voltage;           // mV
    int16_t current;           // mA
    uint16_t temperature;       // 0.1K units
    uint8_t stateOfCharge;     // %
    uint16_t remainingCapacity; // mAh
    uint16_t fullChargeCapacity;// mAh
    uint16_t cycleCount;
    Status status;
};

// Simple configuration
struct Config {
    uint8_t deviceAddress;      // SMBus address (default 0x0B)
    uint32_t commandDelayMs;    // Delay between commands (default 1ms)
};

class Driver {
public:
    // Constructor with optional configuration
    explicit Driver(const Config& config = defaultConfig());
    
    // Initialize communication with battery
    HAL_StatusTypeDef init();
    
    // Read battery parameters - these are the main functions you'll use
    HAL_StatusTypeDef read(Reading what, uint16_t& value);
    HAL_StatusTypeDef read(Reading what, int16_t& value);
    HAL_StatusTypeDef read(Reading what, uint8_t& value);
    HAL_StatusTypeDef read(Reading what, std::string& value);
    HAL_StatusTypeDef read(Reading what, Status& value);
    HAL_StatusTypeDef read(Reading what, BatteryData& value);
    
    // Write battery settings
    HAL_StatusTypeDef write(Setting what, uint16_t value);
    
    // Utility functions
    HAL_StatusTypeDef reset();
    static Config defaultConfig();
    
    // Temperature conversion helper
    static float temperatureToC(uint16_t raw) {
        return (raw * 0.1f) - 273.15f;
    }

private:
    // ========================================================================
    // LOW LEVEL IMPLEMENTATION - Don't call these directly
    // ========================================================================
    
    Config config_;
    uint8_t writeAddress_;
    uint8_t readAddress_;
    
    // Low-level SMBus operations
    HAL_StatusTypeDef readWord(uint8_t cmd, uint16_t& data);
    HAL_StatusTypeDef writeWord(uint8_t cmd, uint16_t data);
    HAL_StatusTypeDef readBlock(uint8_t cmd, std::vector<uint8_t>& data);
    HAL_StatusTypeDef writeBlock(uint8_t cmd, const std::vector<uint8_t>& data);
    
    // Utility functions
    void applyCommandDelay();
    HAL_StatusTypeDef manufacturerCommand(uint16_t command);
    HAL_StatusTypeDef manufacturerRead(uint16_t command, uint16_t& data);
};

} // namespace BQ40Z80

#endif // BQ40Z80_H