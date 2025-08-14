/**
 * @file bq40z80.h
 * @brief BQ40Z80 Battery Gauge Driver Interface
 * 
 * This driver provides a high-level interface for communicating with the Texas Instruments
 * BQ40Z80 battery fuel gauge IC over SMBus/I2C. The BQ40Z80 is a highly integrated battery
 * management device that provides accurate battery state monitoring, protection features,
 * and communication capabilities for multi-cell Li-ion battery packs.
 * 
 * ## Features
 * - Smart Battery System (SBS) v1.1 compliant communication
 * - Comprehensive battery parameter monitoring (voltage, current, temperature, capacity)
 * - ManufacturerAccess (MAC) command support for advanced features
 * - Automatic recovery from device lock/freeze conditions
 * - Platform-agnostic design using HAL abstraction layer
 * 
 * ## Architecture
 * The driver follows a layered architecture:
 * - High-level API: User-friendly read/write functions with strong typing
 * - Low-level implementation: SMBus protocol handling and error recovery
 * - HAL abstraction: Platform-specific I2C operations (provided by platform)
 * 
 * ## Usage Example
 * ```cpp
 * BQ40Z80::Driver battery;
 * if (battery.init() == HAL_OK) {
 *     uint16_t voltage;
 *     battery.read(BQ40Z80::Reading::Voltage, voltage);
 *     float tempC = battery.temperatureToC(temperature);
 * }
 * ```
 * 
 * ## Platform Requirements
 * - I2C/SMBus interface configured for 100kHz or 400kHz operation
 * - Platform must provide HAL_I2C_Master_Transmit/Receive implementations
 * - FreeRTOS or equivalent delay functions (HAL_Delay_MS)
 * - Optional: SEGGER RTT for debug output
 * 
 * @author Raptors PŁ Poland
 * @date 2025
 * @version 1.0.0
 */

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

/**
 * @enum Reading
 * @brief Battery parameters that can be read from the BQ40Z80
 * 
 * These enum values correspond to Smart Battery System (SBS) standard commands
 * as defined in the SBS specification v1.1. Each reading type maps directly to
 * a specific register address in the BQ40Z80 memory map.
 * 
 * The BQ40Z80 continuously monitors battery parameters and updates these values
 * based on coulomb counting, voltage measurements, and temperature sensing.
 */
enum class Reading : uint8_t {
    Voltage              = 0x09,  ///< Battery pack voltage in mV (16-bit unsigned)
    Current              = 0x0A,  ///< Instantaneous current in mA (16-bit signed, + = charging, - = discharging)
    AverageCurrent       = 0x0B,  ///< Rolling average current over 1 minute in mA (16-bit signed)
    Temperature          = 0x08,  ///< Battery temperature in 0.1K units (16-bit unsigned, subtract 273.15 for °C)
    StateOfCharge        = 0x0D,  ///< Relative state of charge as percentage (0-100%)
    AbsoluteStateOfCharge = 0x0E, ///< Absolute state of charge, compensated for temperature and age (0-100%)
    RemainingCapacity    = 0x0F,  ///< Remaining battery capacity in mAh at current discharge rate
    FullChargeCapacity   = 0x10,  ///< Predicted pack capacity when fully charged in mAh
    CycleCount           = 0x17,  ///< Number of discharge cycles the battery has experienced
    ManufacturerName     = 0x20,  ///< ASCII string containing battery manufacturer name (up to 20 chars)
    DeviceName           = 0x21,  ///< ASCII string containing device/model name (up to 20 chars)
    SerialNumber         = 0x1C,  ///< Battery pack serial number (16-bit unsigned)
    BatteryStatus        = 0x16,  ///< Status flags register containing alarm and status bits
    AllBatteryData       = 0xFF   ///< Special value to read all parameters in single operation
};

/**
 * @enum Setting
 * @brief Configurable battery parameters that can be written to the BQ40Z80
 * 
 * These settings allow runtime configuration of battery charging parameters
 * and operating modes. Changes to these values affect the battery management
 * behavior and should be set according to the battery pack specifications.
 * 
 * @warning Incorrect settings can damage the battery or reduce its lifespan.
 *          Always consult the battery datasheet before modifying these values.
 */
enum class Setting : uint8_t {
    BatteryMode          = 0x03,  ///< Control flags for battery operation (enable/disable charging, etc.)
    ChargingCurrent      = 0x14,  ///< Maximum allowed charging current in mA (protects battery from overcurrent)
    ChargingVoltage      = 0x15   ///< Maximum charging voltage in mV (prevents overcharge)
};

/**
 * @enum MACCommand
 * @brief ManufacturerAccess commands for advanced BQ40Z80 features
 * 
 * The ManufacturerAccess (MAC) interface provides access to extended functionality
 * not available through standard SBS commands. These commands are accessed through
 * register 0x00 (ManufacturerAccess) or 0x44 (ManufacturerBlockAccess) and allow:
 * - Reading device information and status
 * - Controlling power states (shutdown, sleep)
 * - Accessing diagnostic data
 * - Managing security features (seal/unseal)
 * 
 * MAC commands are particularly useful when the device is in a locked or sealed
 * state where standard SBS registers may return fixed values (e.g., 0x16CC).
 * 
 * @note Some MAC commands require the device to be unsealed first.
 */
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

/**
 * @struct Status
 * @brief Decoded battery status flags from BatteryStatus register (0x16)
 * 
 * This structure provides a human-readable representation of the battery
 * status register bits. Each flag indicates a specific condition or alarm
 * state that the battery management system has detected.
 * 
 * The status flags are updated in real-time by the BQ40Z80 based on:
 * - Voltage thresholds
 * - Current measurements
 * - Temperature limits
 * - Capacity calculations
 * - Time-based predictions
 */
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

/**
 * @struct BatteryData
 * @brief Comprehensive battery state snapshot
 * 
 * This structure aggregates all commonly needed battery parameters into
 * a single data structure. It provides a complete view of the battery's
 * current state including electrical parameters, capacity information,
 * and status flags.
 * 
 * Use this structure when you need multiple battery parameters and want
 * to minimize I2C transactions by reading all data in a batch operation.
 */
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

/**
 * @struct Config
 * @brief Driver configuration parameters
 * 
 * Configuration structure for customizing driver behavior including
 * device addressing and timing parameters. The default configuration
 * works for most BQ40Z80 implementations using standard addressing.
 */
struct Config {
    uint8_t deviceAddress;      // SMBus address (default 0x0B)
    uint32_t commandDelayMs;    // Delay between commands (default 1ms)
};

/**
 * @class Driver
 * @brief Main driver class for BQ40Z80 battery gauge communication
 * 
 * This class provides the primary interface for interacting with the BQ40Z80
 * battery fuel gauge. It handles all communication protocols, error recovery,
 * and data conversion to provide a simple, robust API for battery monitoring.
 * 
 * ## Key Features:
 * - Automatic SMBus protocol handling with PEC (Packet Error Checking)
 * - Transparent fallback to ManufacturerBlockAccess when SBS fails
 * - Built-in recovery mechanisms for locked/frozen devices
 * - Type-safe read/write operations with enum-based commands
 * - Comprehensive error reporting via HAL status codes
 * 
 * ## Thread Safety:
 * This driver is NOT thread-safe. If using from multiple FreeRTOS tasks,
 * external synchronization (mutex/semaphore) is required.
 * 
 * ## Error Handling:
 * All operations return HAL_StatusTypeDef:
 * - HAL_OK: Operation successful
 * - HAL_ERROR: Communication or protocol error
 * - HAL_TIMEOUT: Device not responding
 * - HAL_BUSY: I2C bus busy
 */
class Driver {
public:
    /**
     * @brief Construct a new Driver object with I2C handle and configuration
     * @param i2c_handle Pointer to initialized I2C handle for communication
     * @param config Configuration parameters (uses defaultConfig() if not specified)
     * 
     * The I2C handle must be properly initialized before creating the driver.
     * This design enables platform independence and multi-device support.
     */
    explicit Driver(I2C_HandleTypeDef* i2c_handle, const Config& config = defaultConfig());
    
    /**
     * @brief Initialize communication with the battery gauge
     * 
     * This function performs the following initialization sequence:
     * 1. Tests basic I2C communication by reading BatteryMode register
     * 2. Detects if device is frozen (returns 0x16CC for all registers)
     * 3. Attempts recovery if device is locked:
     *    - Sends device reset command
     *    - Attempts to unseal using default keys
     * 4. Validates communication is working correctly
     * 
     * @return HAL_OK if initialization successful, error code otherwise
     * 
     * @note This function should be called once before any other operations.
     *       It may take up to 500ms if recovery is needed.
     */
    HAL_StatusTypeDef init();
    
    /**
     * @brief Read a 16-bit unsigned value from the battery
     * @param what Parameter to read (e.g., Voltage, Temperature)
     * @param value Reference to store the read value
     * @return HAL_OK on success, error code on failure
     * 
     * @note Automatically handles fallback to MAC commands if SBS fails
     */
    HAL_StatusTypeDef read(Reading what, uint16_t& value);
    
    /**
     * @brief Read a 16-bit signed value from the battery (e.g., current)
     * @param what Parameter to read (typically Current or AverageCurrent)
     * @param value Reference to store the read value
     * @return HAL_OK on success, error code on failure
     */
    HAL_StatusTypeDef read(Reading what, int16_t& value);
    
    /**
     * @brief Read an 8-bit value from the battery (e.g., state of charge)
     * @param what Parameter to read (typically StateOfCharge)
     * @param value Reference to store the read value (0-100 for percentages)
     * @return HAL_OK on success, error code on failure
     */
    HAL_StatusTypeDef read(Reading what, uint8_t& value);
    
    /**
     * @brief Read a string value from the battery (e.g., manufacturer name)
     * @param what Parameter to read (ManufacturerName, DeviceName, etc.)
     * @param value Reference to store the read string (up to 20 characters)
     * @return HAL_OK on success, error code on failure
     */
    HAL_StatusTypeDef read(Reading what, std::string& value);
    
    /**
     * @brief Read and decode the battery status register
     * @param what Must be Reading::BatteryStatus
     * @param value Reference to Status structure to fill with decoded flags
     * @return HAL_OK on success, error code on failure
     */
    HAL_StatusTypeDef read(Reading what, Status& value);
    
    /**
     * @brief Read all battery parameters in a single operation
     * @param what Must be Reading::AllBatteryData
     * @param value Reference to BatteryData structure to fill
     * @return HAL_OK on success, error code on failure
     * 
     * @note This is more efficient than multiple individual reads
     */
    HAL_StatusTypeDef read(Reading what, BatteryData& value);
    
    /**
     * @brief Write a configuration value to the battery
     * @param what Setting to modify (BatteryMode, ChargingCurrent, ChargingVoltage)
     * @param value New value to write
     * @return HAL_OK on success, error code on failure
     * 
     * @warning Ensure values are within safe limits for your battery pack
     */
    HAL_StatusTypeDef write(Setting what, uint16_t value);
    
    /**
     * @brief Read data using ManufacturerAccess command
     * @param command MAC command to execute
     * @param value Reference to store the read value
     * @return HAL_OK on success, error code on failure
     * 
     * @note Use when you need data not available through standard SBS
     */
    HAL_StatusTypeDef readMAC(MACCommand command, uint16_t& value);
    
    /**
     * @brief Execute a ManufacturerAccess command
     * @param command MAC command to execute
     * @param value Optional parameter for the command (default 0)
     * @return HAL_OK on success, error code on failure
     * 
     * @note Some commands like DeviceReset don't require a value parameter
     */
    HAL_StatusTypeDef writeMAC(MACCommand command, uint16_t value = 0);
    
    
    /**
     * @brief Reset the battery gauge device
     * @return HAL_OK on success, error code on failure
     * 
     * Sends a device reset command via ManufacturerAccess.
     * The device will restart and reinitialize all parameters.
     * 
     * @note Device may be unavailable for up to 500ms after reset
     */
    HAL_StatusTypeDef reset();
    
    /**
     * @brief Get default configuration for the driver
     * @return Config structure with standard BQ40Z80 settings
     * 
     * Default configuration:
     * - Device address: 0x0B (7-bit)
     * - Command delay: 1ms between operations
     */
    static Config defaultConfig();
    
    /**
     * @brief Print comprehensive battery report to debug output
     * 
     * Outputs all major battery parameters to SEGGER RTT including:
     * - Voltage, current, temperature
     * - State of charge and capacity
     * - Status flags and alarms
     * - Device information
     * 
     * @note Requires SEGGER RTT to be initialized
     */
    void printBatteryReport();
    
    /**
     * @brief Convert raw temperature reading to Celsius
     * @param raw Temperature in 0.1K units from BQ40Z80
     * @return Temperature in degrees Celsius
     * 
     * The BQ40Z80 reports temperature in tenths of Kelvin.
     * This helper converts to the more common Celsius scale.
     * 
     * Example: raw=2980 -> 298.0K -> 24.85°C
     */
    static float temperatureToC(uint16_t raw) {
        return (raw * 0.1f) - 273.15f;
    }

private:
    // ========================================================================
    // LOW LEVEL IMPLEMENTATION - Internal use only
    // ========================================================================
    /**
     * The following private members handle the low-level SMBus protocol
     * implementation including packet formatting, PEC calculation, error
     * recovery, and I2C transaction management. These should not be called
     * directly by user code.
     */
    
    I2C_HandleTypeDef* i2c_handle_;     /**< I2C peripheral handle for communication */
    Config config_;                     /**< Driver configuration parameters */
    uint8_t writeAddress_;              /**< Pre-calculated 8-bit write address */
    uint8_t readAddress_;               /**< Pre-calculated 8-bit read address */
    
    /**
     * @brief Read a 16-bit word using SMBus protocol
     * @param cmd Command/register address
     * @param data Reference to store read data
     * @return HAL_OK on success
     * 
     * Implements SMBus Read Word protocol with PEC validation
     */
    HAL_StatusTypeDef readWord(uint8_t cmd, uint16_t& data);
    
    /**
     * @brief Write a 16-bit word using SMBus protocol
     * @param cmd Command/register address
     * @param data Data to write
     * @return HAL_OK on success
     * 
     * Implements SMBus Write Word protocol with PEC generation
     */
    HAL_StatusTypeDef writeWord(uint8_t cmd, uint16_t data);
    
    /**
     * @brief Read variable-length block using SMBus protocol
     * @param cmd Command/register address
     * @param data Vector to store read data (sized automatically)
     * @return HAL_OK on success
     * 
     * Implements SMBus Block Read protocol, handles up to 32 bytes
     */
    HAL_StatusTypeDef readBlock(uint8_t cmd, std::vector<uint8_t>& data);
    
    /**
     * @brief Write block of data using SMBus protocol
     * @param cmd Command/register address  
     * @param data Data to write (up to 32 bytes)
     * @return HAL_OK on success
     */
    HAL_StatusTypeDef writeBlock(uint8_t cmd, const std::vector<uint8_t>& data);
    
    /**
     * @brief Apply configured delay between commands
     * 
     * Some BQ40Z80 operations require a small delay between
     * successive commands to avoid overwhelming the device.
     */
    void applyCommandDelay();
    
    /**
     * @brief Send a ManufacturerAccess command via register 0x00
     * @param command 16-bit MAC command code
     * @return HAL_OK on success
     */
    HAL_StatusTypeDef manufacturerCommand(uint16_t command);
    
    /**
     * @brief Read data via ManufacturerAccess
     * @param command MAC command to execute
     * @param data Reference to store result
     * @return HAL_OK on success
     */
    HAL_StatusTypeDef manufacturerRead(uint16_t command, uint16_t& data);
    
    /**
     * @brief Read data via ManufacturerBlockAccess (0x44)
     * @param command Command to send via block access
     * @param data Reference to store result
     * @return HAL_OK on success
     * 
     * This is used as a fallback when standard SBS and MAC commands
     * fail due to device being in a locked or frozen state.
     */
    HAL_StatusTypeDef manufacturerBlockAccessRead(uint16_t command, uint16_t& data);
    
    /**
     * @brief Write command to ManufacturerBlockAccess register
     * @param mac_command Command to write
     * @return HAL_OK on success
     */
    HAL_StatusTypeDef manufacturerBlockWrite(uint16_t mac_command);
    
    /**
     * @brief Read response from ManufacturerBlockAccess
     * @param data Vector to store response data
     * @return HAL_OK on success
     */
    HAL_StatusTypeDef manufacturerBlockRead(std::vector<uint8_t>& data);
    
    /**
     * @brief Reset I2C peripheral to recover from bus errors
     * 
     * Performs a complete I2C peripheral reset and reconfiguration
     * to recover from stuck bus conditions or protocol violations.
     */
    void resetI2C();
};

} // namespace BQ40Z80

#endif // BQ40Z80_H