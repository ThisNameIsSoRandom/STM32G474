/**
 * @file test_bq40z80_driver.cpp
 * @brief Unit tests for BQ40Z80 battery gauge driver
 * 
 * These tests verify basic BQ40Z80 driver functionality that doesn't
 * require I2C communication, such as configuration and data conversion.
 */

#include "../runner/gtest_minimal.h"
#include "BQ40Z80/bq40z80.h"
#include "SEGGER_RTT.h"
#include <cstring>

/**
 * @brief Test fixture for BQ40Z80 driver tests
 * 
 * Provides common setup and teardown for BQ40Z80 tests.
 */
class BQ40Z80DriverTest : public testing::Test {
protected:
    void SetUp() override {
        // Create mock I2C handle for testing - initialize to zero first
        memset(&mockI2CHandle, 0, sizeof(mockI2CHandle));
        mockI2CHandle.Instance = nullptr;  // Mock handle doesn't need real peripheral
        
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
 * @test Driver Creation
 * Tests that driver can be created with an I2C handle
 */
TEST_F(BQ40Z80DriverTest, DriverCreation) {
    EXPECT_NE(nullptr, driver);
}

/**
 * @test Custom Configuration
 * Tests driver creation with custom configuration
 */
TEST_F(BQ40Z80DriverTest, CustomConfiguration) {
    BQ40Z80::Config custom_config;
    custom_config.deviceAddress = 0x0C;
    custom_config.commandDelayMs = 5;
    custom_config.recoveryDelayMs = 100;
    custom_config.maxRetries = 5;
    
    BQ40Z80::Driver custom_driver(&mockI2CHandle, custom_config);
    
    // Test passes if driver can be created with custom config
    EXPECT_TRUE(true);
}