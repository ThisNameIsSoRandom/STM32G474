/**
 * @file test_bq40z80_driver.cpp
 * @brief Unit tests for BQ40Z80 battery gauge driver
 * 
 * These tests verify the BQ40Z80 driver functionality using HAL mocks
 * to simulate I2C communication. Tests cover normal operation, error
 * conditions, and recovery mechanisms.
 */

#include "../runner/gtest_minimal.h"
#include "../mocks/hal_i2c_mock.h"
#include "BQ40Z80/bq40z80.h"
#include "SEGGER_RTT.h"
#include <cstring>

/**
 * @brief Test fixture for BQ40Z80 driver tests
 * 
 * Provides common setup and teardown for all BQ40Z80 tests,
 * including mock reset and driver initialization.
 */
class BQ40Z80DriverTest : public testing::Test {
protected:
    void SetUp() override {
        // Reset HAL mock before each test
        HAL_I2C_Mock::getInstance().reset();
        
        // Create mock I2C handle for testing - initialize to zero first
        memset(&mockI2CHandle, 0, sizeof(mockI2CHandle));
        mockI2CHandle.Instance = nullptr;  // Mock handle doesn't need real peripheral
#ifdef HAL_I2C_MODULE_ENABLED
        // Real HAL is available, use proper initialization
        mockI2CHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
        mockI2CHandle.State = HAL_I2C_STATE_READY;
#endif
        
        // Create driver with mock I2C handle and default config
        driver = new BQ40Z80::Driver(&mockI2CHandle);
    }
    
    void TearDown() override {
        delete driver;
        driver = nullptr;
    }
    
    I2C_HandleTypeDef mockI2CHandle;
    BQ40Z80::Driver* driver;
};

/**
 * @test Default Configuration
 * Verifies that the default configuration matches expected values
 */
TEST_F(BQ40Z80DriverTest, DefaultConfiguration) {
    BQ40Z80::Config config = BQ40Z80::Driver::defaultConfig();
    
    EXPECT_EQ(0x0B, config.deviceAddress);
    EXPECT_EQ(1, config.commandDelayMs);
}

/**
 * @test Temperature Conversion
 * Tests the temperature conversion helper function
 */
TEST_F(BQ40Z80DriverTest, TemperatureConversion) {
    // Test 25°C (298.15K = 2981.5 in 0.1K units)
    uint16_t raw_temp = 2982;  // 298.2K
    float celsius = BQ40Z80::Driver::temperatureToC(raw_temp);
    
    // Allow small floating point error
    EXPECT_GT(celsius, 24.9f);
    EXPECT_LT(celsius, 25.1f);
    
    // Test 0°C (273.15K)
    raw_temp = 2732;  // 273.2K
    celsius = BQ40Z80::Driver::temperatureToC(raw_temp);
    EXPECT_GT(celsius, -0.1f);
    EXPECT_LT(celsius, 0.1f);
    
    // Test -10°C (263.15K)
    raw_temp = 2632;  // 263.2K
    celsius = BQ40Z80::Driver::temperatureToC(raw_temp);
    EXPECT_GT(celsius, -10.1f);
    EXPECT_LT(celsius, -9.9f);
}

/**
 * @test Successful Initialization
 * Tests driver initialization with responsive device
 */
TEST_F(BQ40Z80DriverTest, InitializationSuccess) {
    // Mock returns normal BatteryMode value
    HAL_I2C_Mock::getInstance().setRegisterValue(0x03, 0x6081);
    HAL_I2C_Mock::getInstance().setNextReadData({0x81, 0x60});  // Little-endian
    
    HAL_StatusTypeDef status = driver->init();
    
    EXPECT_EQ(HAL_OK, status);
    EXPECT_EQ(1, HAL_I2C_Mock::getInstance().getReceiveCount());
}

/**
 * @test Frozen Device Recovery
 * Tests recovery sequence when device returns 0x16CC (frozen state)
 */
TEST_F(BQ40Z80DriverTest, FrozenDeviceRecovery) {
    // First read returns 0x16CC (device frozen)
    HAL_I2C_Mock::getInstance().setNextReadData({0xCC, 0x16});
    
    // After reset commands, device returns normal value
    HAL_I2C_Mock::getInstance().setNextReadData({0x81, 0x60});
    
    HAL_StatusTypeDef status = driver->init();
    
    EXPECT_EQ(HAL_OK, status);
    // Should have multiple transmit operations for recovery
    EXPECT_GT(HAL_I2C_Mock::getInstance().getTransmitCount(), 1);
}

/**
 * @test Read Voltage
 * Tests reading battery voltage
 */
TEST_F(BQ40Z80DriverTest, ReadVoltage) {
    // Initialize driver first
    HAL_I2C_Mock::getInstance().setNextReadData({0x81, 0x60});  // BatteryMode
    driver->init();
    
    // Set voltage reading to 12000mV (0x2EE0)
    HAL_I2C_Mock::getInstance().setNextReadData({0xE0, 0x2E});
    
    uint16_t voltage;
    HAL_StatusTypeDef status = driver->read(BQ40Z80::Reading::Voltage, voltage);
    
    EXPECT_EQ(HAL_OK, status);
    EXPECT_EQ(12000, voltage);
}

/**
 * @test Read Current
 * Tests reading battery current (signed value)
 */
TEST_F(BQ40Z80DriverTest, ReadCurrent) {
    // Initialize driver
    HAL_I2C_Mock::getInstance().setNextReadData({0x81, 0x60});
    driver->init();
    
    // Test positive current (charging) - 500mA (0x01F4)
    HAL_I2C_Mock::getInstance().setNextReadData({0xF4, 0x01});
    
    int16_t current;
    HAL_StatusTypeDef status = driver->read(BQ40Z80::Reading::Current, current);
    
    EXPECT_EQ(HAL_OK, status);
    EXPECT_EQ(500, current);
    
    // Test negative current (discharging) - -1000mA (0xFC18)
    HAL_I2C_Mock::getInstance().setNextReadData({0x18, 0xFC});
    
    status = driver->read(BQ40Z80::Reading::Current, current);
    
    EXPECT_EQ(HAL_OK, status);
    EXPECT_EQ(-1000, current);
}

/**
 * @test Read State of Charge
 * Tests reading battery SOC percentage
 */
TEST_F(BQ40Z80DriverTest, ReadStateOfCharge) {
    // Initialize driver
    HAL_I2C_Mock::getInstance().setNextReadData({0x81, 0x60});
    driver->init();
    
    // Set SOC to 75%
    HAL_I2C_Mock::getInstance().setNextReadData({0x4B, 0x00});  // 75 in lower byte
    
    uint8_t soc;
    HAL_StatusTypeDef status = driver->read(BQ40Z80::Reading::StateOfCharge, soc);
    
    EXPECT_EQ(HAL_OK, status);
    EXPECT_EQ(75, soc);
}

/**
 * @test Fallback to ManufacturerBlockAccess
 * Tests automatic fallback when SBS returns 0x16CC
 */
TEST_F(BQ40Z80DriverTest, ManufacturerBlockAccessFallback) {
    // Note: SetUp() already called reset(), so mock is clean
    
    // Initialize driver
    HAL_I2C_Mock::getInstance().setNextReadData({0x81, 0x60});
    driver->init();
    
    // SBS read returns 0x16CC (frozen)
    HAL_I2C_Mock::getInstance().setNextReadData({0xCC, 0x16});
    
    // MAC fallback: first the block length (16 bytes), then MAC response  
    // Format: [length=16] [cmd_echo_low=0x09] [cmd_echo_high=0x00] [result_low=0x10] [result_high=0x27] [padding...]
    HAL_I2C_Mock::getInstance().setNextReadData({0x10, 0x00});  // Block length = 16
    HAL_I2C_Mock::getInstance().setNextReadData({0x09, 0x00, 0x10, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
    
    uint16_t voltage;
    HAL_StatusTypeDef status = driver->read(BQ40Z80::Reading::Voltage, voltage);
    
    EXPECT_EQ(HAL_OK, status);
    EXPECT_EQ(10000, voltage);
}

/**
 * @test Communication Failure
 * Tests handling of I2C communication errors
 */
TEST_F(BQ40Z80DriverTest, CommunicationError) {
    // Set mock to return error
    HAL_I2C_Mock::getInstance().setNextStatus(HAL_ERROR);
    
    HAL_StatusTypeDef status = driver->init();
    
    // Should still return OK to allow fallback mechanisms
    EXPECT_EQ(HAL_OK, status);
}

/**
 * @test Read String Value
 * Tests reading manufacturer name string
 */
TEST_F(BQ40Z80DriverTest, ReadManufacturerName) {
    // Initialize driver
    HAL_I2C_Mock::getInstance().setNextReadData({0x81, 0x60});
    driver->init();
    
    // Set manufacturer name "Texas" in SMBus block format: [length] [data...]
    std::vector<uint8_t> name_data = {5, 'T', 'e', 'x', 'a', 's'};  // Length=5, then string
    HAL_I2C_Mock::getInstance().setNextReadData(name_data);
    
    std::string manufacturer;
    HAL_StatusTypeDef status = driver->read(BQ40Z80::Reading::ManufacturerName, manufacturer);
    
    EXPECT_EQ(HAL_OK, status);
    EXPECT_EQ("Texas", manufacturer);
}

/**
 * @test Write Charging Current
 * Tests setting maximum charging current
 */
TEST_F(BQ40Z80DriverTest, WriteChargingCurrent) {
    // Initialize driver
    HAL_I2C_Mock::getInstance().setNextReadData({0x81, 0x60});
    driver->init();
    
    // Set charging current to 2000mA
    HAL_StatusTypeDef status = driver->write(BQ40Z80::Setting::ChargingCurrent, 2000);
    
    EXPECT_EQ(HAL_OK, status);
    
    // Verify correct data was written
    std::vector<uint8_t> written = HAL_I2C_Mock::getInstance().getWrittenData();
    EXPECT_GT(written.size(), 2);  // Should have command and data
}

/**
 * @test Reset Device
 * Tests device reset functionality
 */
TEST_F(BQ40Z80DriverTest, DeviceReset) {
    // Initialize driver
    HAL_I2C_Mock::getInstance().setNextReadData({0x81, 0x60});
    driver->init();
    
    HAL_StatusTypeDef status = driver->reset();
    
    EXPECT_EQ(HAL_OK, status);
    EXPECT_GT(HAL_I2C_Mock::getInstance().getTransmitCount(), 0);
}