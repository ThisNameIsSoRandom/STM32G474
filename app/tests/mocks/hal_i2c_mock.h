/**
 * @file hal_i2c_mock.h
 * @brief Mock implementation of HAL I2C functions for unit testing
 * 
 * This mock allows testing of I2C-dependent code without actual hardware.
 * It provides controllable responses and failure injection for comprehensive
 * test coverage of error handling paths.
 */

#ifndef HAL_I2C_MOCK_H
#define HAL_I2C_MOCK_H

#include "hal_types.h"
#include "SEGGER_RTT.h"
#include <vector>
#include <queue>
#include <map>
#include <cstdint>

/**
 * @class HAL_I2C_Mock
 * @brief Mock class for simulating I2C communication
 * 
 * This class provides a mock implementation of HAL I2C functions
 * that can be configured to return specific data sequences and
 * error conditions for testing.
 * 
 * ## Usage Example:
 * ```cpp
 * TEST(BQ40Z80Test, ReadVoltage) {
 *     HAL_I2C_Mock::getInstance().reset();
 *     HAL_I2C_Mock::getInstance().setNextReadData({0x10, 0x27}); // 10000mV
 *     
 *     BQ40Z80::Driver battery;
 *     uint16_t voltage;
 *     EXPECT_EQ(HAL_OK, battery.read(Reading::Voltage, voltage));
 *     EXPECT_EQ(10000, voltage);
 * }
 * ```
 */
class HAL_I2C_Mock {
public:
    /**
     * @brief Get singleton instance of the mock
     */
    static HAL_I2C_Mock& getInstance() {
        static HAL_I2C_Mock instance;
        return instance;
    }
    
    /**
     * @brief Reset mock to initial state
     * 
     * Clears all configured responses and resets counters.
     * Should be called at the beginning of each test.
     */
    void reset() {
        // Clear queue by swapping with empty queue (std::queue doesn't have clear())
        std::queue<std::vector<uint8_t>> empty_queue;
        read_responses_.swap(empty_queue);
        
        write_data_.clear();
        register_map_.clear();
        next_status_ = HAL_OK;
        transmit_count_ = 0;
        receive_count_ = 0;
        fail_after_count_ = -1;
        
        // Debug output to confirm mock reset
        SEGGER_RTT_printf(0, "HAL_I2C_Mock: Reset complete, status=HAL_OK\n");
    }
    
    /**
     * @brief Set the next status code to return
     * @param status HAL status code (HAL_OK, HAL_ERROR, HAL_TIMEOUT, etc.)
     */
    void setNextStatus(HAL_StatusTypeDef status) {
        next_status_ = status;
    }
    
    /**
     * @brief Configure mock to fail after N successful operations
     * @param count Number of successful operations before failure
     * 
     * Useful for testing retry logic and error recovery.
     */
    void failAfter(int count) {
        fail_after_count_ = count;
    }
    
    /**
     * @brief Queue read response data
     * @param data Byte sequence to return on next read
     * 
     * Multiple calls queue multiple responses for sequential reads.
     */
    void setNextReadData(const std::vector<uint8_t>& data) {
        read_responses_.push(data);
        SEGGER_RTT_printf(0, "HAL_I2C_Mock: Queued %d bytes: ", data.size());
        for (size_t i = 0; i < std::min(data.size(), static_cast<size_t>(8)); i++) {
            SEGGER_RTT_printf(0, "0x%02X ", data[i]);
        }
        SEGGER_RTT_printf(0, "\n");
    }
    
    /**
     * @brief Get data written via HAL_I2C_Master_Transmit
     * @return Vector of all bytes written
     */
    std::vector<uint8_t> getWrittenData() const {
        return write_data_;
    }
    
    /**
     * @brief Get number of transmit operations performed
     */
    int getTransmitCount() const { return transmit_count_; }
    
    /**
     * @brief Get number of receive operations performed
     */
    int getReceiveCount() const { return receive_count_; }
    
    /**
     * @brief Verify expected data was written
     * @param expected Expected byte sequence
     * @return true if data matches
     */
    bool verifyWrittenData(const std::vector<uint8_t>& expected) const {
        return write_data_ == expected;
    }
    
    /**
     * @brief Set behavior for specific register reads
     * @param reg Register address
     * @param value Value to return when register is read
     * 
     * Allows simulating specific device register values.
     */
    void setRegisterValue(uint8_t reg, uint16_t value) {
        register_map_[reg] = value;
    }
    
    // Internal methods called by HAL mock functions
    HAL_StatusTypeDef handleTransmit(uint16_t DevAddress, uint8_t* pData, uint16_t Size);
    HAL_StatusTypeDef handleReceive(uint16_t DevAddress, uint8_t* pData, uint16_t Size);
    HAL_StatusTypeDef handleMemWrite(uint16_t DevAddress, uint16_t MemAddress, 
                                     uint16_t MemAddSize, uint8_t* pData, uint16_t Size);
    HAL_StatusTypeDef handleMemRead(uint16_t DevAddress, uint16_t MemAddress,
                                    uint16_t MemAddSize, uint8_t* pData, uint16_t Size);
    
private:
    HAL_I2C_Mock() : next_status_(HAL_OK), transmit_count_(0), 
                     receive_count_(0), fail_after_count_(-1) {}
    
    HAL_StatusTypeDef next_status_;
    std::queue<std::vector<uint8_t>> read_responses_;
    std::vector<uint8_t> write_data_;
    std::map<uint8_t, uint16_t> register_map_;
    int transmit_count_;
    int receive_count_;
    int fail_after_count_;
    
    HAL_StatusTypeDef checkFailureInjection() {
        if (fail_after_count_ > 0) {
            fail_after_count_--;
        } else if (fail_after_count_ == 0) {
            return HAL_ERROR;
        }
        return next_status_;
    }
};

// Wrapped HAL function declarations for linker --wrap option
extern "C" {
    HAL_StatusTypeDef __wrap_HAL_I2C_Master_Transmit(I2C_HandleTypeDef* hi2c, uint16_t DevAddress,
                                                     uint8_t* pData, uint16_t Size, uint32_t Timeout);
    HAL_StatusTypeDef __wrap_HAL_I2C_Master_Receive(I2C_HandleTypeDef* hi2c, uint16_t DevAddress,
                                                    uint8_t* pData, uint16_t Size, uint32_t Timeout);
    HAL_StatusTypeDef __wrap_HAL_I2C_Mem_Write(I2C_HandleTypeDef* hi2c, uint16_t DevAddress,
                                               uint16_t MemAddress, uint16_t MemAddSize,
                                               uint8_t* pData, uint16_t Size, uint32_t Timeout);
    HAL_StatusTypeDef __wrap_HAL_I2C_Mem_Read(I2C_HandleTypeDef* hi2c, uint16_t DevAddress,
                                              uint16_t MemAddress, uint16_t MemAddSize,
                                              uint8_t* pData, uint16_t Size, uint32_t Timeout);
}

#endif // HAL_I2C_MOCK_H