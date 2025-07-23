#include "test_runner.h"
#include "BQ40Z80/bq40z80.h"
#include "hal_types.h"

using namespace UnitTests;

extern "C" void test_bq40z80_advanced() {
    TestRunner::begin("BQ40Z80 Advanced Tests");
    
    // Test driver with custom configuration
    BQ40Z80::Config customConfig;
    customConfig.deviceAddress = 0x0C;  // Non-standard address
    customConfig.commandDelayMs = 5;    // Longer delay
    
    BQ40Z80::Driver driver(customConfig);
    
    // Test initialization (will use weak HAL functions)
    HAL_StatusTypeDef status = driver.init();
    // With weak HAL functions, this should return HAL_OK but log warnings
    TEST_ASSERT_EQUAL(HAL_OK, status, "Driver initialization with weak HAL");
    
    // Test read operations (will use weak HAL functions)
    uint16_t voltage;
    status = driver.read(BQ40Z80::Reading::Voltage, voltage);
    TEST_ASSERT_EQUAL(HAL_OK, status, "Voltage read with weak HAL");
    
    int16_t current;
    status = driver.read(BQ40Z80::Reading::Current, current);
    TEST_ASSERT_EQUAL(HAL_OK, status, "Current read with weak HAL");
    
    uint8_t soc;
    status = driver.read(BQ40Z80::Reading::StateOfCharge, soc);
    TEST_ASSERT_EQUAL(HAL_OK, status, "SOC read with weak HAL");
    
    // Test string reads (will use weak HAL functions)
    std::string manufacturerName;
    status = driver.read(BQ40Z80::Reading::ManufacturerName, manufacturerName);
    TEST_ASSERT_EQUAL(HAL_OK, status, "Manufacturer name read with weak HAL");
    
    // Test composite read
    BQ40Z80::BatteryData batteryData;
    status = driver.read(BQ40Z80::Reading::AllBatteryData, batteryData);
    TEST_ASSERT_EQUAL(HAL_OK, status, "Complete battery data read with weak HAL");
    
    // Test write operations
    status = driver.write(BQ40Z80::Setting::BatteryMode, 0x8000);
    TEST_ASSERT_EQUAL(HAL_OK, status, "Battery mode write with weak HAL");
    
    // Test reset
    status = driver.reset();
    TEST_ASSERT_EQUAL(HAL_OK, status, "Reset with weak HAL");
    
    TestRunner::end();
}