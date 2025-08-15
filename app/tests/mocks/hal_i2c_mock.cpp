/**
 * @file hal_i2c_mock.cpp
 * @brief Implementation of HAL I2C mock for unit testing
 * 
 * This file provides the actual implementation of mocked HAL I2C
 * functions that replace the real HAL functions during testing.
 */

#include "hal_i2c_mock.h"
#include "SEGGER_RTT.h"
#include <algorithm>
#include <cstring>

// HAL function mock implementations that override real HAL functions using strong symbols
// These will take precedence over the real HAL functions when linked with --whole-archive

extern "C" {

/**
 * Wrapped mock implementation of HAL_I2C_Master_Transmit
 * The linker --wrap option redirects calls to this function
 */
HAL_StatusTypeDef __wrap_HAL_I2C_Master_Transmit(I2C_HandleTypeDef* hi2c, uint16_t DevAddress,
                                                 uint8_t* pData, uint16_t Size, uint32_t Timeout) {
    (void)hi2c; (void)Timeout; // Unused parameters
    
    return HAL_I2C_Mock::getInstance().handleTransmit(DevAddress, pData, Size);
}

/**
 * Wrapped mock implementation of HAL_I2C_Master_Receive
 * The linker --wrap option redirects calls to this function
 */
HAL_StatusTypeDef __wrap_HAL_I2C_Master_Receive(I2C_HandleTypeDef* hi2c, uint16_t DevAddress,
                                                uint8_t* pData, uint16_t Size, uint32_t Timeout) {
    (void)hi2c; (void)Timeout; // Unused parameters
    
    return HAL_I2C_Mock::getInstance().handleReceive(DevAddress, pData, Size);
}

/**
 * Wrapped mock implementation of HAL_I2C_Mem_Write
 * The linker --wrap option redirects calls to this function
 */
HAL_StatusTypeDef __wrap_HAL_I2C_Mem_Write(I2C_HandleTypeDef* hi2c, uint16_t DevAddress,
                                           uint16_t MemAddress, uint16_t MemAddSize,
                                           uint8_t* pData, uint16_t Size, uint32_t Timeout) {
    (void)hi2c; (void)Timeout; // Unused parameters
    
    return HAL_I2C_Mock::getInstance().handleMemWrite(DevAddress, MemAddress, MemAddSize, pData, Size);
}

/**
 * Wrapped mock implementation of HAL_I2C_Mem_Read
 * The linker --wrap option redirects calls to this function
 */
HAL_StatusTypeDef __wrap_HAL_I2C_Mem_Read(I2C_HandleTypeDef* hi2c, uint16_t DevAddress,
                                          uint16_t MemAddress, uint16_t MemAddSize,
                                          uint8_t* pData, uint16_t Size, uint32_t Timeout) {
    (void)hi2c; (void)Timeout; // Unused parameters
    
    return HAL_I2C_Mock::getInstance().handleMemRead(DevAddress, MemAddress, MemAddSize, pData, Size);
}

} // extern "C"

// HAL_I2C_Mock method implementations

HAL_StatusTypeDef HAL_I2C_Mock::handleTransmit(uint16_t DevAddress, uint8_t* pData, uint16_t Size) {
    transmit_count_++;
    
    HAL_StatusTypeDef status = checkFailureInjection();
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "HAL_I2C_Mock: Transmit returning error status=%d\n", status);
        return status;
    }
    
    // Store transmitted data for verification
    for (uint16_t i = 0; i < Size; i++) {
        write_data_.push_back(pData[i]);
    }
    
    SEGGER_RTT_printf(0, "HAL_I2C_Mock: Transmit to 0x%04X, %d bytes -> HAL_OK\n", DevAddress, Size);
    
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mock::handleReceive(uint16_t DevAddress, uint8_t* pData, uint16_t Size) {
    receive_count_++;
    
    HAL_StatusTypeDef status = checkFailureInjection();
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "HAL_I2C_Mock: Receive returning error status=%d\n", status);
        return status;
    }
    
    // Return queued response data
    if (!read_responses_.empty()) {
        std::vector<uint8_t> response = read_responses_.front();
        read_responses_.pop();
        
        uint16_t copySize = std::min(Size, static_cast<uint16_t>(response.size()));
        std::memcpy(pData, response.data(), copySize);
        
        SEGGER_RTT_printf(0, "HAL_I2C_Mock: Receive from 0x%04X, %d bytes -> HAL_OK\n", DevAddress, copySize);
        return HAL_OK;
    }
    
    // No data queued - return zeros
    std::memset(pData, 0, Size);
    SEGGER_RTT_printf(0, "HAL_I2C_Mock: Receive from 0x%04X, %d bytes (no data queued) -> HAL_OK\n", DevAddress, Size);
    
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mock::handleMemWrite(uint16_t DevAddress, uint16_t MemAddress,
                                               uint16_t MemAddSize, uint8_t* pData, uint16_t Size) {
    transmit_count_++;
    
    HAL_StatusTypeDef status = checkFailureInjection();
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "HAL_I2C_Mock: MemWrite returning error status=%d\n", status);
        return status;
    }
    
    // Store memory address and data
    if (MemAddSize == 1) {
        write_data_.push_back(static_cast<uint8_t>(MemAddress));
    } else {
        write_data_.push_back(static_cast<uint8_t>(MemAddress >> 8));
        write_data_.push_back(static_cast<uint8_t>(MemAddress & 0xFF));
    }
    
    for (uint16_t i = 0; i < Size; i++) {
        write_data_.push_back(pData[i]);
    }
    
    SEGGER_RTT_printf(0, "HAL_I2C_Mock: MemWrite to 0x%04X, reg 0x%04X, %d bytes -> HAL_OK\n", 
                     DevAddress, MemAddress, Size);
    
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mock::handleMemRead(uint16_t DevAddress, uint16_t MemAddress,
                                              uint16_t MemAddSize, uint8_t* pData, uint16_t Size) {
    (void)MemAddSize; // Unused parameter
    receive_count_++;
    
    HAL_StatusTypeDef status = checkFailureInjection();
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "HAL_I2C_Mock: MemRead returning error status=%d\n", status);
        return status;
    }
    
    // Check if we have a specific value for this register
    uint8_t reg = static_cast<uint8_t>(MemAddress);
    if (register_map_.find(reg) != register_map_.end()) {
        uint16_t value = register_map_[reg];
        
        // Return little-endian data
        if (Size >= 1) pData[0] = static_cast<uint8_t>(value & 0xFF);
        if (Size >= 2) pData[1] = static_cast<uint8_t>(value >> 8);
        
        SEGGER_RTT_printf(0, "HAL_I2C_Mock: MemRead from 0x%04X, reg 0x%04X = 0x%04X\n",
                         DevAddress, MemAddress, value);
        return HAL_OK;
    }
    
    // Use queued response if available
    if (!read_responses_.empty()) {
        std::vector<uint8_t> response = read_responses_.front();
        read_responses_.pop();
        
        uint16_t copySize = std::min(Size, static_cast<uint16_t>(response.size()));
        std::memcpy(pData, response.data(), copySize);
        
        SEGGER_RTT_printf(0, "HAL_I2C_Mock: MemRead from 0x%04X, reg 0x%04X, %d bytes\n",
                         DevAddress, MemAddress, copySize);
        return HAL_OK;
    }
    
    // Default response
    std::memset(pData, 0, Size);
    SEGGER_RTT_printf(0, "HAL_I2C_Mock: MemRead from 0x%04X, reg 0x%04X, %d bytes (default)\n",
                     DevAddress, MemAddress, Size);
    
    return HAL_OK;
}