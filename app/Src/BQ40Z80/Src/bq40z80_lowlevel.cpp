#include "BQ40Z80/bq40z80.h"
#include "freertos_types.h"
#include <vector>
#include <cstring>

// Include hal_types.h for DEBUG_LOG macro - must be after other includes  
#include "hal_types.h"

// Local definition of DEBUG_LOG for BQ40Z80 files (outside namespace)
#ifdef STM32G474xx
    #include <cstdio>
    #define DEBUG_LOG(format, ...) printf(format "\n", ##__VA_ARGS__)
#else
    #include "SEGGER_RTT.h"
    #define DEBUG_LOG(format, ...) SEGGER_RTT_printf(0, format "\n", ##__VA_ARGS__)
#endif

namespace BQ40Z80 {

// ============================================================================
// LOW LEVEL IMPLEMENTATION - Internal functions
// ============================================================================

HAL_StatusTypeDef Driver::readWord(uint8_t cmd, uint16_t& data) {
    uint8_t buffer[2] = {0, 0}; // Initialize buffer to detect if data is actually received
    
    DEBUG_LOG("BQ40Z80: Reading word from cmd=0x%02X (addr: write=0x%02X, read=0x%02X)\n", 
                     cmd, writeAddress_, readAddress_);
    
    // Send command using I2C blocking mode
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(i2c_handle_, 
                                                       writeAddress_, 
                                                       &cmd, 
                                                       1, 
                                                       1000); // 1 second timeout
    
    if (status != HAL_OK) {
        DEBUG_LOG("BQ40Z80: Failed to send command 0x%02X (status: %d)\n", cmd, status);
        return status;
    }
    
    // Small delay between write and read
    HAL_Delay_MS(10);
    
    // Debug: Print buffer before read
    DEBUG_LOG("BQ40Z80: Buffer before read: 0x%02X 0x%02X\n", buffer[0], buffer[1]);
    
    // Read 2 bytes of data from the device using I2C blocking mode
    status = HAL_I2C_Master_Receive(i2c_handle_,
                                   readAddress_,
                                   buffer,
                                   2,
                                   1000); // 1 second timeout
    
    if (status != HAL_OK) {
        DEBUG_LOG("BQ40Z80: Failed to read data for command 0x%02X (status: %d)\n", cmd, status);
        return status;
    }
    
    // Debug: Print buffer after read
    DEBUG_LOG("BQ40Z80: Buffer after read: 0x%02X 0x%02X\n", buffer[0], buffer[1]);
    
    // Combine the two received bytes into a 16-bit value (little-endian)
    data = static_cast<uint16_t>(buffer[0] | (buffer[1] << 8));  // Low byte in buffer[0], high byte in buffer[1]
    
    DEBUG_LOG("BQ40Z80: Read word 0x%04X from cmd=0x%02X\n", data, cmd);
    
    return HAL_OK;
}

HAL_StatusTypeDef Driver::writeWord(uint8_t cmd, uint16_t data) {
    uint8_t buffer[3];
    
    // Prepare data following SMBus word write protocol: cmd + low byte + high byte
    buffer[0] = cmd;
    buffer[1] = data & 0xFF;         // Low byte
    buffer[2] = (data >> 8) & 0xFF;  // High byte
    
    DEBUG_LOG("BQ40Z80: Writing word 0x%04X to cmd=0x%02X\n", data, cmd);
    
    // Single transmission with command and data using I2C blocking mode
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(i2c_handle_, 
                                                       writeAddress_, 
                                                       buffer, 
                                                       3, 
                                                       1000); // 1 second timeout
    
    if (status != HAL_OK) {
        DEBUG_LOG("BQ40Z80: Failed to write word to command 0x%02X (status: %d)\n", cmd, status);
        return status;
    }
    
    return HAL_OK;
}

HAL_StatusTypeDef Driver::readBlock(uint8_t cmd, std::vector<uint8_t>& data) {
    uint8_t buffer[33]; // Max 32 data bytes + 1 length byte for SMBus block read
    
    DEBUG_LOG("BQ40Z80: Reading block from cmd=0x%02X\n", cmd);
    
    // First, send the command to indicate which block to read using I2C blocking mode
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(i2c_handle_, 
                                                       writeAddress_, 
                                                       &cmd, 
                                                       1, 
                                                       1000); // 1 second timeout
    
    if (status != HAL_OK) {
        DEBUG_LOG("BQ40Z80: Failed to send block read command 0x%02X (status: %d)\n", cmd, status);
        return status;
    }
    
    // Small delay between write and read
    HAL_Delay_MS(10);
    
    // Read the block data using I2C blocking mode (first byte is length, followed by data)
    status = HAL_I2C_Master_Receive(i2c_handle_,
                                   readAddress_,
                                   buffer,
                                   33,  // Max possible: 1 length byte + 32 data bytes
                                   2000); // 2 second timeout for block read
    
    if (status != HAL_OK) {
        DEBUG_LOG("BQ40Z80: Failed to read block data (status: %d)\n", status);
        return status;
    }
    
    // Extract length and data from SMBus block format
    uint8_t length = buffer[0];
    if (length > 32) {
        DEBUG_LOG("BQ40Z80: Invalid block length: %d\n", length);
        return HAL_ERROR;
    }
    
    data.resize(length);
    if (length > 0) {
        std::memcpy(data.data(), &buffer[1], length);
    }
    
    DEBUG_LOG("BQ40Z80: Read %d bytes from block\n", length);
    
    return HAL_OK;
}

HAL_StatusTypeDef Driver::writeBlock(uint8_t cmd, const std::vector<uint8_t>& data) {
    if (data.size() > 32) {
        DEBUG_LOG("BQ40Z80: Block write size too large: %d\n", data.size());
        return HAL_ERROR;
    }
    
    // Prepare buffer with SMBus block write format: cmd + length + data
    uint8_t buffer[34]; // cmd + length + max 32 data bytes
    buffer[0] = cmd;
    buffer[1] = static_cast<uint8_t>(data.size());
    
    if (!data.empty()) {
        std::memcpy(&buffer[2], data.data(), data.size());
    }
    
    uint8_t totalLength = static_cast<uint8_t>(2 + data.size());
    
    DEBUG_LOG("BQ40Z80: Writing block cmd=0x%02X, len=%d, total=%d\n", cmd, data.size(), totalLength);
    
    // Use I2C blocking mode for reliable transmission
    DEBUG_LOG("BQ40Z80: Using I2C blocking mode for block write\n");
    
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(i2c_handle_, 
                                                       writeAddress_, 
                                                       buffer, 
                                                       totalLength,
                                                       2000); // 2 second timeout
    
    if (status != HAL_OK) {
        DEBUG_LOG("BQ40Z80: Block write failed (status: %d)\n", status);
        return status;
    }
    
    DEBUG_LOG("BQ40Z80: Block write completed successfully\n");
    return HAL_OK;
}

void Driver::applyCommandDelay() {
    vTaskDelay(config_.commandDelayMs);
}

HAL_StatusTypeDef Driver::manufacturerCommand(uint16_t command) {
    // Per BQ40Z80 documentation: Data to address 0x00 is BIG ENDIAN (not little endian)
    // Example: Send 0x0021 as "00 21" not "21 00"
    uint8_t buffer[3];
    buffer[0] = 0x00;                    // ManufacturerAccess command
    buffer[1] = 0x00;                    // High byte first (always 0x00 for our commands)  
    buffer[2] = command & 0xFF;          // Low byte second
    
    DEBUG_LOG("BQ40Z80: Sending MAC command 0x%04X to ManufacturerAccess (big-endian)", command);
    
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(i2c_handle_, 
                                                       writeAddress_, 
                                                       buffer, 
                                                       3,
                                                       1000); // 1 second timeout
    
    if (status != HAL_OK) {
        DEBUG_LOG("BQ40Z80: Failed to send ManufacturerAccess command (status: %d)", status);
    }
    
    return status;
}

// New ManufacturerBlockAccess method - use this instead of broken ManufacturerData 
HAL_StatusTypeDef Driver::manufacturerBlockAccessRead(uint16_t command, uint16_t& data) {
    DEBUG_LOG("BQ40Z80: Using ManufacturerBlockAccess method for command 0x%04X\n", command);
    
    // Step 1: Send MAC command via ManufacturerBlockAccess (0x44)
    HAL_StatusTypeDef status = manufacturerBlockWrite(command);
    if (status != HAL_OK) {
        DEBUG_LOG("BQ40Z80: Failed to send ManufacturerBlockAccess command (status: %d)\n", status);
        return status;
    }
    
    // Step 2: Wait for BQ40Z80 to process the MAC command
    DEBUG_LOG("BQ40Z80: Waiting for MAC command processing...\n");
    HAL_Delay_MS(100); // Give BQ40Z80 time to process MAC command
    
    // Step 3: Read response via ManufacturerBlockAccess (0x44) 
    std::vector<uint8_t> response;
    status = manufacturerBlockRead(response);
    
    if (status == HAL_OK && response.size() >= 4) {
        // Verify MAC command echo (first 2 bytes should match our command)
        uint16_t echo_command = response[0] | (response[1] << 8);
        if (echo_command == command) {
            // Extract result data (bytes 2-3 in little-endian format)
            data = response[2] | (response[3] << 8);
            DEBUG_LOG("BQ40Z80: ManufacturerBlockAccess success - command 0x%04X = 0x%04X\n", 
                             command, data);
            return HAL_OK;
        } else {
            DEBUG_LOG("BQ40Z80: MAC command mismatch - sent 0x%04X, got echo 0x%04X\n", 
                             command, echo_command);
            return HAL_ERROR;
        }
    } else {
        DEBUG_LOG("BQ40Z80: ManufacturerBlockAccess read failed (status: %d) or insufficient data (%d bytes)\n", 
                         status, response.size());
        return HAL_ERROR;
    }
}

HAL_StatusTypeDef Driver::manufacturerRead(uint16_t command, uint16_t& data) {
    // CRITICAL DEBUG: Check what's in ManufacturerData BEFORE sending command
    DEBUG_LOG("BQ40Z80: Reading ManufacturerData BEFORE sending command 0x%04X\n", command);
    std::vector<uint8_t> preCmdData;
    if (readBlock(0x23, preCmdData) == HAL_OK && preCmdData.size() >= 2) {
        uint16_t preData = preCmdData[0] | (preCmdData[1] << 8);
        DEBUG_LOG("BQ40Z80: Pre-command data: 0x%04X, size: %d bytes\n", preData, preCmdData.size());
    }
    
    // Try different approaches to clear the register  
    DEBUG_LOG("BQ40Z80: Attempting to clear ManufacturerData register\n");
    
    // Method 1: Send 0x0000 command
    manufacturerCommand(0x0000);
    HAL_Delay_MS(50);
    
    // Method 2: Read ManufacturerData again to see if it changed
    std::vector<uint8_t> clearCheckData;
    if (readBlock(0x23, clearCheckData) == HAL_OK && clearCheckData.size() >= 2) {
        uint16_t clearData = clearCheckData[0] | (clearCheckData[1] << 8);
        DEBUG_LOG("BQ40Z80: After clear attempt: 0x%04X\n", clearData);
    }
    
    // Send manufacturer command to ManufacturerAccess (0x00)
    DEBUG_LOG("BQ40Z80: Sending MAC command 0x%04X\n", command);
    HAL_StatusTypeDef status = manufacturerCommand(command);
    if (status != HAL_OK) {
        DEBUG_LOG("BQ40Z80: Failed to send manufacturer command 0x%04X\n", command);
        return status;
    }
    
    // CRITICAL: BQ40Z80 needs significant time to process MAC commands and update register 0x23
    // Different commands may need different processing times
    uint32_t processingDelay = 100; // Default 100ms
    switch (command) {
        case 0x0001: // DeviceType - fast
            processingDelay = 50;
            break;
        case 0x0009: // Voltage - needs ADC conversion
        case 0x000A: // Current - needs ADC conversion  
        case 0x0008: // Temperature - needs ADC conversion
            processingDelay = 150;
            break;
        case 0x0054: // OperationStatus
        case 0x0055: // ChargingStatus
        case 0x0056: // GaugingStatus
            processingDelay = 100;
            break;
        default:
            processingDelay = 120;
            break;
    }
    
    DEBUG_LOG("BQ40Z80: Waiting %dms for MAC command 0x%04X processing\n", processingDelay, command);
    HAL_Delay_MS(processingDelay);
    
    // Try reading multiple times if data doesn't change
    uint16_t previousData = 0xFFFF;
    int retries = 3;
    
    for (int i = 0; i < retries; i++) {
        // Per BQ40Z80 documentation: Read result from ManufacturerData (0x23) as BLOCK READ
        std::vector<uint8_t> blockData;
        status = readBlock(0x23, blockData); // ManufacturerData register
        
        if (status == HAL_OK && blockData.size() >= 2) {
            // Debug: Print raw block data to understand what we're getting
            DEBUG_LOG("BQ40Z80: Raw block data (%d bytes): ", blockData.size());
            for (size_t j = 0; j < blockData.size() && j < 16; j++) {
                DEBUG_LOG("0x%02X ", blockData[j]);
            }
            DEBUG_LOG("");
            
            // Check if this looks like ASCII data (common for some BQ40Z80 responses)
            bool isAscii = true;
            for (size_t j = 0; j < blockData.size() && j < 8; j++) {
                if (blockData[j] < 0x20 || blockData[j] > 0x7E) {
                    if (blockData[j] != 0x00) { // Allow null terminator
                        isAscii = false;
                        break;
                    }
                }
            }
            
            if (isAscii && blockData.size() > 4) {
                // This might be ASCII string data - log it
                std::string asciiData(blockData.begin(), blockData.begin() + std::min(blockData.size(), size_t(16)));
                DEBUG_LOG("BQ40Z80: Possible ASCII data: '%s'\n", asciiData.c_str());
            }
            
            // For now, still use the first 2 bytes as little-endian data
            data = blockData[0] | (blockData[1] << 8);
            DEBUG_LOG("BQ40Z80: Attempt %d - Read manufacturer data 0x%04X for command 0x%04X\n", 
                             i+1, data, command);
            
            // Debug: Check if this is ASCII characters (0x6261 = 'ab' in ASCII)
            if (data == 0x6261) {
                char ascii_chars[3] = {blockData[0], blockData[1], 0};
                DEBUG_LOG("BQ40Z80: Constant 0x6261 = '%c%c' in ASCII - possible protocol error\n", 
                                 ascii_chars[0], ascii_chars[1]);
            }
            
            // Special handling for specific commands that might return different data formats
            switch (command) {
                case 0x0001: // DeviceType - should be a simple 16-bit value
                    if (blockData.size() >= 2) {
                        data = blockData[0] | (blockData[1] << 8);
                    }
                    break;
                case 0x0009: // Voltage - should be 16-bit value in mV
                case 0x000A: // Current - should be 16-bit signed value in mA  
                case 0x0008: // Temperature - should be 16-bit value in 0.1K
                    if (blockData.size() >= 2) {
                        data = blockData[0] | (blockData[1] << 8);
                    }
                    break;
                default:
                    // Use first 2 bytes for other commands
                    if (blockData.size() >= 2) {
                        data = blockData[0] | (blockData[1] << 8);
                    }
                    break;
            }
            
            // Check if data changed from previous read
            if (i == 0 || data != previousData) {
                // Data is new or first read
                return HAL_OK;
            }
            
            previousData = data;
            
            // If data hasn't changed, wait a bit more
            if (i < retries - 1) {
                DEBUG_LOG("BQ40Z80: Data unchanged, waiting additional 50ms\n");
                HAL_Delay_MS(50);
            }
        } else if (status == HAL_OK) {
            DEBUG_LOG("BQ40Z80: Insufficient data in ManufacturerData block (size: %d)\n", blockData.size());
            status = HAL_ERROR;
            break;
        } else {
            DEBUG_LOG("BQ40Z80: Failed to read ManufacturerData for command 0x%04X (attempt %d)\n", command, i+1);
            if (i < retries - 1) {
                HAL_Delay_MS(50); // Wait before retry
            }
        }
    }
    
    if (status != HAL_OK) {
        DEBUG_LOG("BQ40Z80: All attempts failed for MAC command 0x%04X\n", command);
    }
    
    return status;
}

HAL_StatusTypeDef Driver::manufacturerBlockWrite(uint16_t mac_command) {
    // Per BQ40Z80 documentation: ManufacturerBlockAccess uses little-endian format
    // Example: Send 0x0006 as "06 00" (low byte first, high byte second)
    std::vector<uint8_t> data(2);
    data[0] = mac_command & 0xFF;           // Low byte first
    data[1] = (mac_command >> 8) & 0xFF;    // High byte second
    
    DEBUG_LOG("BQ40Z80: Sending MAC command 0x%04X via ManufacturerBlockAccess (0x44) as %02X %02X\n", 
                     mac_command, data[0], data[1]);
    
    // Send via ManufacturerBlockAccess (0x44) using SMBus block write
    return writeBlock(0x44, data);
}

HAL_StatusTypeDef Driver::manufacturerBlockRead(std::vector<uint8_t>& data) {
    DEBUG_LOG("BQ40Z80: Reading response from ManufacturerBlockAccess (0x44)\n");
    
    // Per BQ40Z80 documentation: ManufacturerBlockAccess returns data in format:
    // [length] [MAC command low] [MAC command high] [data low] [data high] ...
    // Example: Reading Chemical ID (0x0006) returns "04 06 00 00 01"
    //   - Length: 04 (4 bytes total)
    //   - MAC command: 06 00 (0x0006 in little-endian)
    //   - Data: 00 01 (0x0100 = chemical ID 256 in little-endian)
    
    // Use standard block read to get ManufacturerBlockAccess data
    HAL_StatusTypeDef status = readBlock(0x44, data);
    
    if (status == HAL_OK && data.size() >= 4) {
        // Parse the response according to BQ40Z80 format
        DEBUG_LOG("BQ40Z80: ManufacturerBlockAccess response (%d bytes): ", data.size());
        for (size_t i = 0; i < data.size() && i < 8; i++) {
            DEBUG_LOG("%02X ", data[i]);
        }
        DEBUG_LOG("\n");
        
        // Extract MAC command (bytes 0-1 in little-endian)
        uint16_t mac_command = data[0] | (data[1] << 8);
        DEBUG_LOG("BQ40Z80: MAC command in response: 0x%04X\n", mac_command);
        
        // Extract actual data (bytes 2-3+ in little-endian)
        if (data.size() >= 4) {
            uint16_t result = data[2] | (data[3] << 8);
            DEBUG_LOG("BQ40Z80: MAC result: 0x%04X\n", result);
        }
        
        return HAL_OK;
    } else if (status == HAL_OK) {
        DEBUG_LOG("BQ40Z80: ManufacturerBlockAccess returned insufficient data (%d bytes)\n", data.size());
        return HAL_ERROR;
    } else {
        DEBUG_LOG("BQ40Z80: Failed to read ManufacturerBlockAccess (status: %d)\n", status);
        return status;
    }
}

void Driver::resetI2C() {
    DEBUG_LOG("BQ40Z80: Attempting I2C recovery\n");
    
    // Get current I2C state
    HAL_I2C_StateTypeDef state = HAL_I2C_GetState(i2c_handle_);
    DEBUG_LOG("BQ40Z80: Current I2C state: %d\n", state);
    
    if (state != HAL_I2C_STATE_READY) {
        DEBUG_LOG("BQ40Z80: Attempting I2C reset\n");
        HAL_I2C_DeInit(i2c_handle_);
        HAL_Delay_MS(50);
        MX_I2C2_Init(); // Reinitialize I2C
        HAL_Delay_MS(50);
        
        state = HAL_I2C_GetState(i2c_handle_);
        DEBUG_LOG("BQ40Z80: Post-reset I2C state: %d\n", state);
    }
}

} // namespace BQ40Z80