/**
 * @file test_bq40z80_basic.cpp
 * @brief Basic on-target tests for BQ40Z80 battery gauge driver
 * 
 * These tests run on actual hardware and verify basic driver functionality
 * without requiring HAL mocks. They test configuration, utilities, and
 * basic driver behavior that doesn't depend on I2C hardware being present.
 */

#include "../runner/gtest_minimal.h"
#include "BQ40Z80/bq40z80.h"
#include "SEGGER_RTT.h"
#include <cstring>

/**
 * @brief Test fixture for basic BQ40Z80 driver tests
 * 
 * Provides common setup and teardown for on-target testing
 * without relying on HAL mocks.
 */
class BQ40Z80BasicTest : public testing::Test {
protected:
    void SetUp() override {
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
TEST_F(BQ40Z80BasicTest, DefaultConfiguration) {
    BQ40Z80::Config config = BQ40Z80::Driver::defaultConfig();
    
    EXPECT_EQ(0x0B, config.deviceAddress);
    EXPECT_EQ(1, config.commandDelayMs);
}

/**
 * @test Temperature Conversion
 * Tests the temperature conversion helper function
 */
TEST_F(BQ40Z80BasicTest, TemperatureConversion) {
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
 * @test Driver Construction
 * Tests that the driver can be constructed with default and custom configs
 */
TEST_F(BQ40Z80BasicTest, DriverConstruction) {
    // Test construction with mock I2C handle and default config
    I2C_HandleTypeDef testI2C;
    memset(&testI2C, 0, sizeof(testI2C));
    testI2C.Instance = nullptr;  // Mock handle
#ifdef HAL_I2C_MODULE_ENABLED
    testI2C.State = HAL_I2C_STATE_READY;
#endif
    
    BQ40Z80::Driver defaultDriver(&testI2C);
    
    // Test construction with custom config
    BQ40Z80::Config customConfig;
    customConfig.deviceAddress = 0x55;
    customConfig.commandDelayMs = 5;
    
    BQ40Z80::Driver customDriver(&testI2C, customConfig);
    
    // Both should construct successfully (no crashes)
    EXPECT_TRUE(true);  // If we get here, construction worked
}

/**
 * @test Initialization Attempt
 * Tests that driver initialization can be called (may fail due to no hardware)
 */
TEST_F(BQ40Z80BasicTest, InitializationAttempt) {
    // This may fail if no BQ40Z80 is connected, but shouldn't crash
    HAL_StatusTypeDef status = driver->init();
    
    // We don't care if it succeeds or fails, just that it doesn't crash
    // and returns a valid HAL status
    EXPECT_TRUE(status == HAL_OK || status == HAL_ERROR || 
                status == HAL_BUSY || status == HAL_TIMEOUT);
}