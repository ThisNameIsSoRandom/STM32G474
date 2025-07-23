#ifndef BQ40Z80_TYPES_H
#define BQ40Z80_TYPES_H

#include <cstdint>

namespace BQ40Z80 {

// ManufacturerAccess sub-commands
enum class ManufacturerAccessCommand : uint16_t {
    DeviceType          = 0x0001,
    FirmwareVersion     = 0x0002,
    HardwareVersion     = 0x0003,
    InstructionFlashChecksum = 0x0004,
    StaticDFSignature   = 0x0005,
    ChemID             = 0x0006,
    PreChargeFlag      = 0x0007,
    ChargingStatus     = 0x0008,
    GaugingStatus      = 0x0009,
    ShutdownMode       = 0x0010,
    SleepMode          = 0x0011,
    AutoCalibrationEnable = 0x0012,
    BatteryInsert      = 0x000C,
    BatteryRemove      = 0x000D,
    SetHibernate       = 0x0011,
    ClearHibernate     = 0x0012,
    SetConfigUpdate    = 0x0013,
    ExitConfigUpdate   = 0x0043,
    OutputCCnCV        = 0x0019,
    OutputShort        = 0x001A,
    BootROM            = 0x0F00,
    ROMMode            = 0x0F00,
    ResetData          = 0x0005,
    Unseal             = 0x8000,  // Requires key
    FullAccess         = 0x8000   // Requires different key
};

// Battery Mode flags (Command 0x03)
struct BatteryModeFlags {
    bool internalChargeController : 1;
    bool primaryBatterySupport : 1;
    uint8_t reserved1 : 1;
    bool conditionFlag : 1;
    bool chargeControllerEnabled : 1;
    bool primaryBattery : 1;
    uint8_t reserved2 : 1;
    bool alarmMode : 1;
    bool chargerMode : 1;
    bool capacityMode : 1;
    uint8_t reserved3 : 3;
    bool ccBusLowEnable : 1;
    bool amBusLowEnable : 1;
    bool pbBusLowEnable : 1;
};

// Battery Status flags (Command 0x16)
enum class StatusFlag : uint16_t {
    OverChargedAlarm = 0x8000,
    TerminateChargeAlarm = 0x4000,
    OverTempAlarm = 0x1000,
    TerminateDischargeAlarm = 0x0800,
    RemainingCapacityAlarm = 0x0200,
    RemainingTimeAlarm = 0x0100,
    Initialized = 0x0080,
    Discharging = 0x0040,
    FullyCharged = 0x0020,
    FullyDischarged = 0x0010,
    ErrorMask = 0x000F
};

// Gauging Status flags
struct GaugingStatus {
    bool FD : 1;    // Full discharge
    bool FC : 1;    // Full charge
    bool DSG : 1;   // Discharge
    bool CF : 1;    // Condition flag
    bool DOD0 : 1;  // Depth of discharge 0%
    bool SOC1 : 1;  // State of charge 1%
    bool SOCF : 1;  // State of charge final
    bool H : 1;     // Hibernate
    bool RSVD : 1;  // Reserved
    bool LDMD : 1;  // Load mode
    bool RUP_DIS : 1; // Resistance update disable
    bool VOK : 1;   // Voltage OK
    bool QMax : 1;  // QMax update
};

// Data Flash addresses
namespace DataFlash {
    constexpr uint16_t DesignCapacity = 0x462A;
    constexpr uint16_t DesignVoltage = 0x462C;
    constexpr uint16_t CycleCountThreshold = 0x462E;
    constexpr uint16_t CCThreshold = 0x4630;
    constexpr uint16_t SOC1SetThreshold = 0x4632;
    constexpr uint16_t SOC1ClearThreshold = 0x4634;
    constexpr uint16_t SOCFSetThreshold = 0x4636;
    constexpr uint16_t SOCFClearThreshold = 0x4638;
}

// SMBus protocol constants
constexpr uint8_t SMBUS_ADDRESS_DEFAULT = 0x0B;  // 7-bit address
constexpr uint8_t SMBUS_WRITE_ADDRESS = 0x16;   // Pre-shifted for STM32 HAL
constexpr uint8_t SMBUS_READ_ADDRESS = 0x17;    // Pre-shifted for STM32 HAL
constexpr uint8_t SMBUS_MAX_BLOCK_SIZE = 32;    // Maximum block transfer size

// Timing constants
constexpr uint32_t COMMAND_DELAY_MS = 1;        // Between normal commands
constexpr uint32_t FLASH_WRITE_DELAY_MS = 10;   // After flash operations
constexpr uint32_t UNSEAL_DELAY_MS = 10;        // After unseal command
constexpr uint32_t RESET_DELAY_MS = 100;        // After reset command

// Error codes
enum class ErrorCode : uint8_t {
    None = 0,
    CommunicationTimeout = 1,
    InvalidParameter = 2,
    DeviceSealed = 3,
    FlashAccessDenied = 4,
    PECError = 5,
    DeviceNotResponding = 6,
    InvalidCommand = 7
};

// Conversion utilities
inline float temperatureToC(uint16_t raw) {
    return (raw * 0.1f) - 273.15f;  // Convert from 0.1K to Celsius
}

inline uint16_t temperatureFromC(float celsius) {
    return static_cast<uint16_t>((celsius + 273.15f) * 10.0f);
}

} // namespace BQ40Z80

#endif // BQ40Z80_TYPES_H