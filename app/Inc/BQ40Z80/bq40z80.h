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
    Voltage              = 0x09,  // Battery voltage in mV (SBS_Voltage)
    Current              = 0x0A,  // Current in mA (+ = charging, - = discharging) (SBS_Current)
    AverageCurrent       = 0x0B,  // Average current in mA (SBS_AverageCurrent)
    Temperature          = 0x08,  // Temperature in 0.1K units (SBS_Temperature)
    StateOfCharge        = 0x0D,  // Relative state of charge in % (SBS_RelativeStateOfCharge)
    AbsoluteStateOfCharge = 0x0E, // Absolute state of charge in % (SBS_AbsoluteStateOfCharge)
    RemainingCapacity    = 0x0F,  // Remaining capacity in mAh (SBS_RemainingCapacity)
    FullChargeCapacity   = 0x10,  // Full charge capacity in mAh (SBS_FullChargeCapacity)
    CycleCount           = 0x17,  // Number of charge cycles (SBS_CycleCount)
    ManufacturerName     = 0x20,  // Battery manufacturer name (SBS_ManufacturerName)
    DeviceName           = 0x21,  // Device/model name (SBS_DeviceName)
    SerialNumber         = 0x1C,  // Battery serial number (SBS_SerialNumber)
    BatteryStatus        = 0x16,  // Battery status register (SBS_BatteryStatus)
    AllBatteryData       = 0xFF   // Read all battery parameters at once
};

// What you can write to the battery
enum class Setting : uint8_t {
    BatteryMode          = 0x03,  // Battery operating mode (SBS_BatteryMode)
    ChargingCurrent      = 0x14,  // Maximum charging current in mA (SBS_ChargingCurrent)
    ChargingVoltage      = 0x15   // Maximum charging voltage in mV (SBS_ChargingVoltage)
};

// ManufacturerAccess commands (MAC) - used when standard SBS commands fail
enum class MACCommand : uint16_t {
    // Device Information
    DeviceType           = 0x0001,  // MFA_DEVICE_TYPE
    FirmwareVersion      = 0x0002,  // MFA_FIRMWARE_VERSION
    HardwareVersion      = 0x0003,  // MFA_HARDWARE_VERSION
    ChemicalID           = 0x0006,  // MFA_CHEMICAL_ID
    
    // Status Commands
    SafetyAlert          = 0x0050,  // MFA_SAFETY_ALERT
    SafetyStatus         = 0x0051,  // MFA_SAFETY_STATUS
    OperationStatus      = 0x0054,  // MFA_OPERATION_STATUS
    ChargingStatus       = 0x0055,  // MFA_CHARGING_STATUS
    GaugingStatus        = 0x0056,  // MFA_GAUGING_STATUS
    StateOfHealth        = 0x0077,  // MFA_STATE_OF_HEALTH
    
    // Control Commands
    ShutdownMode         = 0x0010,  // MFA_SHUTDOWN_MODE
    SleepMode            = 0x0011,  // MFA_SLEEP_MODE
    FETControl           = 0x0022,  // MFA_FET_CONTROL
    DeviceReset          = 0x0041,  // MFA_DEVICE_RESET
    SealDevice           = 0x0030,  // MFA_SEAL_DEVICE
    
    // Extended Data
    CurrentLong          = 0x0082,  // MFA_CURRENT_LONG
    NoLoadRemCap         = 0x005A,  // MFA_NO_LOAD_REM_CAP
    FilterCapacity       = 0x0078,  // MFA_FILTER_CAPACITY
    ManufacturerInfo     = 0x0070,  // MFA_MANUFACTURER_INFO
    ManufacturerInfoB    = 0x007A,  // MFA_MANUFACTURER_INFO_B
    ManufacturerInfoC    = 0x0080,  // MFA_MANUFACTURER_INFO_C
    ManufacturerInfoD    = 0x0081   // MFA_MANUFACTURER_INFO_D
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
    
    // ManufacturerAccess command execution
    HAL_StatusTypeDef readMAC(MACCommand command, uint16_t& value);
    HAL_StatusTypeDef writeMAC(MACCommand command, uint16_t value = 0);
    
    
    // Utility functions
    HAL_StatusTypeDef reset();
    static Config defaultConfig();
    
    // Comprehensive battery reporting
    void printBatteryReport();
    
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
    HAL_StatusTypeDef manufacturerBlockAccessRead(uint16_t command, uint16_t& data);
    
    // ManufacturerBlockAccess (0x44) functions
    HAL_StatusTypeDef manufacturerBlockWrite(uint16_t mac_command);
    HAL_StatusTypeDef manufacturerBlockRead(std::vector<uint8_t>& data);
    
    // Recovery functions
    void resetI2C();
};

} // namespace BQ40Z80

#endif // BQ40Z80_H