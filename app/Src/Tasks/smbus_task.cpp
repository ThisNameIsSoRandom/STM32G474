/**
  ******************************************************************************
  * @file           : smbus_task.cpp
  * @brief          : SMBus task implementation for BQ40Z80 battery gauge
  ******************************************************************************
  */

#include "hal_types.h"
#include "freertos_types.h"
#include "SEGGER_RTT.h"

extern "C" {

// BQ40Z80 SMBus addresses
#define BQ40Z80_ADDR_WRITE  0x16  // 0x0B << 1
#define BQ40Z80_ADDR_READ   0x17  // (0x0B << 1) | 1

// BQ40Z80 Commands
#define CMD_MANUFACTURER_ACCESS       0x00
#define CMD_MANUFACTURER_BLOCK_ACCESS 0x44
#define CMD_DEVICE_TYPE              0x0001
#define CMD_FIRMWARE_VERSION         0x0002
#define CMD_GAUGING                  0x0021

/**
 * @brief Wait for I2C to be ready
 * @param timeout_ms Timeout in milliseconds
 * @return HAL_OK if ready, HAL_TIMEOUT if timeout
 */
static HAL_StatusTypeDef waitI2CReady(uint32_t timeout_ms)
{
    uint32_t timeout = timeout_ms;
    while (HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY) {
        if (timeout-- == 0) {
            return HAL_TIMEOUT;
        }
        vTaskDelay(1); // 1ms delay
    }
    return HAL_OK;
}

/**
 * @brief Send ManufacturerBlockAccess command to BQ40Z80
 * @param mac_command The MAC command to send (e.g., 0x0021 for Gauging)
 * @return HAL status
 */
static HAL_StatusTypeDef sendManufacturerBlockAccess(uint16_t mac_command)
{
    HAL_StatusTypeDef status;
    
    // Prepare data for block write (command in little endian)
    uint8_t buffer[3];
    buffer[0] = CMD_MANUFACTURER_BLOCK_ACCESS;  // SMBus command
    buffer[1] = mac_command & 0xFF;             // Low byte of MAC command
    buffer[2] = (mac_command >> 8) & 0xFF;      // High byte of MAC command
    
    SEGGER_RTT_printf(0, "Sending MAC command 0x%04X via ManufacturerBlockAccess (0x44)\n", mac_command);
    
    // Send block write with command and data using I2C blocking mode
    status = HAL_I2C_Master_Transmit(&hi2c2, 
                                    BQ40Z80_ADDR_WRITE, 
                                    buffer, 
                                    3,
                                    1000); // 1 second timeout
    
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "Failed to send MAC command: %d\n", status);
        return status;
    }
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "Timeout waiting for MAC command transmission\n");
        return status;
    }
    
    return HAL_OK;
}

/**
 * @brief Read ManufacturerBlockAccess response from BQ40Z80
 * @param data Buffer to store response data
 * @param max_len Maximum length to read
 * @param actual_len Actual length read
 * @return HAL status
 */
static HAL_StatusTypeDef readManufacturerBlockAccess(uint8_t* data, uint8_t max_len, uint8_t* actual_len)
{
    HAL_StatusTypeDef status;
    uint8_t buffer[33]; // Max block read is 32 bytes + length byte
    
    // First send the command byte to read from using I2C blocking mode
    uint8_t cmd = CMD_MANUFACTURER_BLOCK_ACCESS;
    status = HAL_I2C_Master_Transmit(&hi2c2,
                                    BQ40Z80_ADDR_WRITE,
                                    &cmd,
                                    1,
                                    1000); // 1 second timeout
    
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "Failed to send read command: %d\n", status);
        return status;
    }
    
    // Small delay between write and read
    vTaskDelay(10);
    
    // Read the block response using I2C blocking mode
    status = HAL_I2C_Master_Receive(&hi2c2,
                                   BQ40Z80_ADDR_READ,
                                   buffer,
                                   33,
                                   2000); // 2 second timeout for block read
    
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "Failed to read MAC response: %d\n", status);
        return status;
    }
    if (status != HAL_OK) {
        SEGGER_RTT_printf(0, "Timeout reading MAC response\n");
        return status;
    }
    
    // Extract data (first byte is length)
    *actual_len = buffer[0];
    if (*actual_len > max_len) {
        *actual_len = max_len;
    }
    
    if (*actual_len > 0) {
        for (uint8_t i = 0; i < *actual_len; i++) {
            data[i] = buffer[i + 1];
        }
    }
    
    SEGGER_RTT_printf(0, "Read %d bytes from ManufacturerBlockAccess\n", *actual_len);
    
    return HAL_OK;
}

/**
 * @brief SMBus task for BQ40Z80 communication
 * @param pvParameters Task parameters
 */
void smbusTask(void *pvParameters)
{
    (void)pvParameters;
    
    SEGGER_RTT_printf(0, "BQ40Z80 SMBus task started!\n");
    
    // Wait for system to stabilize
    vTaskDelay(500);
    
    // Test sequence counter
    uint32_t test_num = 0;
    
    for(;;)
    {
        SEGGER_RTT_printf(0, "\n--- BQ40Z80 Test %d ---\n", ++test_num);
        
        // Example 1: Read Device Type using ManufacturerBlockAccess
        SEGGER_RTT_printf(0, "Test 1: Reading Device Type (0x0001)\n");
        if (sendManufacturerBlockAccess(CMD_DEVICE_TYPE) == HAL_OK) {
            vTaskDelay(10); // Small delay for command processing
            
            uint8_t response[32];
            uint8_t len;
            if (readManufacturerBlockAccess(response, sizeof(response), &len) == HAL_OK) {
                if (len >= 2) {
                    uint16_t device_type = response[0] | (response[1] << 8);
                    SEGGER_RTT_printf(0, "Device Type: 0x%04X\n", device_type);
                }
            }
        }
        
        vTaskDelay(100);
        
        // Example 2: Enable IT Gauging using ManufacturerBlockAccess
        SEGGER_RTT_printf(0, "Test 2: Enabling IT Gauging (0x0021)\n");
        if (sendManufacturerBlockAccess(CMD_GAUGING) == HAL_OK) {
            SEGGER_RTT_printf(0, "IT Gauging command sent successfully\n");
            // Note: Check ManufacturingStatus()[GAUGE_EN] to verify it's enabled
        }
        
        vTaskDelay(100);
        
        // Example 3: Read Firmware Version
        SEGGER_RTT_printf(0, "Test 3: Reading Firmware Version (0x0002)\n");
        if (sendManufacturerBlockAccess(CMD_FIRMWARE_VERSION) == HAL_OK) {
            vTaskDelay(10);
            
            uint8_t response[32];
            uint8_t len;
            if (readManufacturerBlockAccess(response, sizeof(response), &len) == HAL_OK) {
                if (len >= 2) {
                    uint16_t fw_version = response[0] | (response[1] << 8);
                    SEGGER_RTT_printf(0, "Firmware Version: 0x%04X\n", fw_version);
                }
            }
        }
        
        // Wait before next iteration
        vTaskDelay(5000);
    }
}

}