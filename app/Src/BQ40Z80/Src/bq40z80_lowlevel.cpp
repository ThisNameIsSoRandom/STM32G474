#include "BQ40Z80/bq40z80.h"
#include "hal_types.h"
#include "freertos_types.h"
#include "SEGGER_RTT.h"
#include <vector>
#include <cstring>

namespace BQ40Z80 {

// ============================================================================
// LOW LEVEL IMPLEMENTATION - Internal functions
// ============================================================================

HAL_StatusTypeDef Driver::readWord(uint8_t cmd, uint16_t& data) {
    HAL_StatusTypeDef status;
    uint8_t buffer[2];
    
    // Send command byte
    status = HAL_SMBUS_Master_Transmit_IT(&hsmbus2, writeAddress_, &cmd, 1, SMBUS_FIRST_FRAME);
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "BQ40Z80: Failed to send command 0x%02X\n", cmd);
        return status;
    }
    
    // Wait for transmission complete
    while (HAL_SMBUS_GetState(&hsmbus2) != HAL_SMBUS_STATE_READY) {
        // TODO: Add timeout
    }
    
    // Read response data (no PEC for simplicity)
    status = HAL_SMBUS_Master_Receive_IT(&hsmbus2, readAddress_, buffer, 2, SMBUS_LAST_FRAME_NO_PEC);
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "BQ40Z80: Failed to read data for command 0x%02X\n", cmd);
        return status;
    }
    
    // Wait for reception complete
    while (HAL_SMBUS_GetState(&hsmbus2) != HAL_SMBUS_STATE_READY) {
        // TODO: Add timeout
    }
    
    // Combine bytes (little-endian)
    data = static_cast<uint16_t>(buffer[0] | (buffer[1] << 8));
    
    return HAL_OK;
}

HAL_StatusTypeDef Driver::writeWord(uint8_t cmd, uint16_t data) {
    uint8_t buffer[3];
    buffer[0] = cmd;
    buffer[1] = data & 0xFF;         // Low byte
    buffer[2] = (data >> 8) & 0xFF;  // High byte
    
    HAL_StatusTypeDef status = HAL_SMBUS_Master_Transmit_IT(&hsmbus2, writeAddress_, buffer, 3, 
                                                          SMBUS_FIRST_AND_LAST_FRAME_NO_PEC);
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "BQ40Z80: Failed to write word to command 0x%02X\n", cmd);
        return status;
    }
    
    // Wait for transmission complete
    while (HAL_SMBUS_GetState(&hsmbus2) != HAL_SMBUS_STATE_READY) {
        // TODO: Add timeout
    }
    
    return HAL_OK;
}

HAL_StatusTypeDef Driver::readBlock(uint8_t cmd, std::vector<uint8_t>& data) {
    HAL_StatusTypeDef status;
    uint8_t buffer[33]; // Max 32 data bytes + 1 length byte
    
    // Send command
    status = HAL_SMBUS_Master_Transmit_IT(&hsmbus2, writeAddress_, &cmd, 1, SMBUS_FIRST_FRAME);
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "BQ40Z80: Failed to send block read command 0x%02X\n", cmd);
        return status;
    }
    
    // Wait for transmission complete
    while (HAL_SMBUS_GetState(&hsmbus2) != HAL_SMBUS_STATE_READY) {
        // TODO: Add timeout
    }
    
    // Read block data
    status = HAL_SMBUS_Master_Receive_IT(&hsmbus2, readAddress_, buffer, 33, SMBUS_LAST_FRAME_NO_PEC);
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "BQ40Z80: Failed to read block data\n");
        return status;
    }
    
    // Wait for reception complete
    while (HAL_SMBUS_GetState(&hsmbus2) != HAL_SMBUS_STATE_READY) {
        // TODO: Add timeout
    }
    
    // Extract length and data
    uint8_t length = buffer[0];
    if (length > 32) {
        SEGGER_RTT_printf(0, "BQ40Z80: Invalid block length: %d\n", length);
        return HAL_ERROR;
    }
    
    data.resize(length);
    if (length > 0) {
        std::memcpy(data.data(), &buffer[1], length);
    }
    
    return HAL_OK;
}

HAL_StatusTypeDef Driver::writeBlock(uint8_t cmd, const std::vector<uint8_t>& data) {
    if (data.size() > 32) {
        SEGGER_RTT_printf(0, "BQ40Z80: Block write size too large: %d\n", data.size());
        return HAL_ERROR;
    }
    
    uint8_t buffer[34]; // cmd + length + data
    buffer[0] = cmd;
    buffer[1] = static_cast<uint8_t>(data.size());
    
    if (!data.empty()) {
        std::memcpy(&buffer[2], data.data(), data.size());
    }
    
    uint8_t totalLength = static_cast<uint8_t>(2 + data.size());
    HAL_StatusTypeDef status = HAL_SMBUS_Master_Transmit_IT(&hsmbus2, writeAddress_, buffer, totalLength,
                                                          SMBUS_FIRST_AND_LAST_FRAME_NO_PEC);
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "BQ40Z80: Failed to write block\n");
        return status;
    }
    
    // Wait for transmission complete
    while (HAL_SMBUS_GetState(&hsmbus2) != HAL_SMBUS_STATE_READY) {
        // TODO: Add timeout
    }
    
    return HAL_OK;
}

void Driver::applyCommandDelay() {
    vTaskDelay(config_.commandDelayMs);
}

HAL_StatusTypeDef Driver::manufacturerCommand(uint16_t command) {
    return writeWord(0x00, command); // ManufacturerAccess command
}

HAL_StatusTypeDef Driver::manufacturerRead(uint16_t command, uint16_t& data) {
    HAL_StatusTypeDef status = manufacturerCommand(command);
    if (status != HAL_OK) return status;
    
    applyCommandDelay();
    
    return readWord(0x00, data); // Read from ManufacturerAccess
}

} // namespace BQ40Z80