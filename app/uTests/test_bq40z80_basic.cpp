#include "test_runner.h"
#include "BQ40Z80/bq40z80.h"

using namespace UnitTests;

extern "C" void test_bq40z80_basic() {
    TestRunner::begin("BQ40Z80 Basic Tests");
    
    // Test default configuration
    BQ40Z80::Config defaultConfig = BQ40Z80::Driver::defaultConfig();
    TEST_ASSERT_EQUAL(0x0B, defaultConfig.deviceAddress, "Default device address");
    TEST_ASSERT_EQUAL(1, defaultConfig.commandDelayMs, "Default command delay");
    
    // Test driver instantiation
    BQ40Z80::Driver driver(defaultConfig);
    
    // Test temperature conversion utility
    uint16_t tempRaw = 2980;  // 25°C in 0.1K units (298.0K)
    float tempC = BQ40Z80::Driver::temperatureToC(tempRaw);
    TEST_ASSERT(tempC > 24.0f && tempC < 26.0f, "Temperature conversion 25°C");
    
    // Test enum values match BQ40Z80 protocol
    TEST_ASSERT_EQUAL(0x09, static_cast<uint8_t>(BQ40Z80::Reading::Voltage), "Voltage command");
    TEST_ASSERT_EQUAL(0x0A, static_cast<uint8_t>(BQ40Z80::Reading::Current), "Current command");
    TEST_ASSERT_EQUAL(0x08, static_cast<uint8_t>(BQ40Z80::Reading::Temperature), "Temperature command");
    TEST_ASSERT_EQUAL(0x0D, static_cast<uint8_t>(BQ40Z80::Reading::StateOfCharge), "SOC command");
    
    TestRunner::end();
}