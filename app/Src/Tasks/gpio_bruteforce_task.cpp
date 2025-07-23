/**
  ******************************************************************************
  * @file           : gpio_bruteforce_task.cpp
  * @brief          : GPIO pin pair brute force testing task
  ******************************************************************************
  */

#include "hal_types.h"
#include "freertos_types.h"
#include "SEGGER_RTT.h"
#include <array>
#include <cstddef>

// GPIO pin pair structure - must be outside extern "C" for templates
struct GpioPinPair {
    void* port1;  // GPIO_TypeDef* cast to void* for platform independence
    uint16_t pin1;
    void* port2;  // GPIO_TypeDef* cast to void* for platform independence
    uint16_t pin2;
    const char* name;
    
    // Constructor for easy initialization
    constexpr GpioPinPair(void* p1, uint16_t pin1, void* p2, uint16_t pin2, const char* n)
        : port1(p1), pin1(pin1), port2(p2), pin2(pin2), name(n) {}
    
    // Check if pair is valid (not null terminator)
    constexpr bool isValid() const {
        return port1 != nullptr && port2 != nullptr && name != nullptr;
    }
};

// Template wrapper for safe pin pair array handling - must be outside extern "C"
template<size_t N>
class SafePinPairArray {
private:
    const std::array<GpioPinPair, N>* pinPairs_;
    size_t validCount_;
    
public:
    explicit SafePinPairArray(const std::array<GpioPinPair, N>* pairs) : pinPairs_(pairs), validCount_(0) {
        if (pairs) {
            // Count valid entries (stop at first invalid entry as null terminator)
            for (size_t i = 0; i < N; ++i) {
                if ((*pairs)[i].isValid()) {
                    validCount_++;
                } else {
                    break; // Stop at null terminator
                }
            }
        }
    }
    
    bool isValid() const { return pinPairs_ != nullptr && validCount_ > 0; }
    size_t size() const { return validCount_; }
    const GpioPinPair& operator[](size_t index) const { 
        return (*pinPairs_)[index % validCount_]; // Safe modulo access
    }
    
    // Circular navigation
    size_t nextIndex(size_t current) const { return (current + 1) % validCount_; }
    size_t prevIndex(size_t current) const { return (current + validCount_ - 1) % validCount_; }
};

extern "C" {

/**
 * @brief GPIO pin pair brute force testing task
 * @param pvParameters Pointer to std::array<GpioPinPair, N> (cast as void*)
 */
void gpioBruteforceTask(void *pvParameters)
{
    if (pvParameters == nullptr) {
        SEGGER_RTT_printf(0, "GPIO Brute Force Task: No pin pairs provided, terminating\n\r");
        vTaskDelete(nullptr);
        return;
    }
    
    // Try to determine array size by checking for common sizes
    // This is a workaround since we can't get template size from void*
    auto tryArraySize = [pvParameters](auto sizeTag) -> bool {
        constexpr size_t N = decltype(sizeTag)::value;
        using ArrayType = std::array<GpioPinPair, N>;
        
        const ArrayType* pinPairs = static_cast<const ArrayType*>(pvParameters);
        SafePinPairArray<N> safeArray(pinPairs);
            
        if (!safeArray.isValid()) {
            return false;
        }
        
        SEGGER_RTT_printf(0, "GPIO Brute Force Task: Starting with %d pin pairs\n\r", safeArray.size());
        SEGGER_RTT_printf(0, "Commands: 'd' or 'D' = next pair, 'a' or 'A' = previous pair\n\r");
        
        size_t currentPairIndex = 0;
        char inputChar;
        
        // Configure all pins as GPIO outputs
        HAL_GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_InitStruct.Pin = 0;  // Will be set for each pin
        GPIO_InitStruct.Mode = HAL_GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = HAL_GPIO_NOPULL;
        GPIO_InitStruct.Speed = HAL_GPIO_SPEED_FREQ_LOW;
        
        for (size_t i = 0; i < safeArray.size(); i++) {
            const auto& pair = safeArray[i];
            
            // Configure pin 1
            GPIO_InitStruct.Pin = pair.pin1;
            HAL_GPIO_Init(static_cast<GPIO_TypeDef*>(pair.port1), &GPIO_InitStruct);
            HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(pair.port1), pair.pin1, HAL_GPIO_PIN_RESET);
            
            // Configure pin 2
            GPIO_InitStruct.Pin = pair.pin2;
            HAL_GPIO_Init(static_cast<GPIO_TypeDef*>(pair.port2), &GPIO_InitStruct);
            HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(pair.port2), pair.pin2, HAL_GPIO_PIN_RESET);
        }
        
        SEGGER_RTT_printf(0, "All pins configured as outputs and set to LOW\n\r");
        
        // Main testing loop
        while (1) {
            const auto& currentPair = safeArray[currentPairIndex];
            
            SEGGER_RTT_printf(0, "\n=== Testing Pin Pair %d/%d: %s ===\n\r", 
                             currentPairIndex + 1, safeArray.size(), currentPair.name);
            SEGGER_RTT_printf(0, "Pin1: Port %p, Pin 0x%04X\n\r", currentPair.port1, currentPair.pin1);
            SEGGER_RTT_printf(0, "Pin2: Port %p, Pin 0x%04X\n\r", currentPair.port2, currentPair.pin2);
            SEGGER_RTT_printf(0, "Blinking pattern: Pin1=HIGH/Pin2=LOW -> Pin1=LOW/Pin2=HIGH\n\r");
            SEGGER_RTT_printf(0, "Press 'd'/'D' for next pair, 'a'/'A' for previous pair\n\r");
            
            // Blink current pin pair until command received
            while (1) {
                // Pattern 1: Pin1=HIGH, Pin2=LOW
                HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(currentPair.port1), currentPair.pin1, HAL_GPIO_PIN_SET);
                HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(currentPair.port2), currentPair.pin2, HAL_GPIO_PIN_RESET);
                
                // Check for RTT input during delay
                bool commandReceived = false;
                for (int i = 0; i < 50; i++) { // 500ms total delay in 10ms chunks
                    vTaskDelay(10);
                    if (SEGGER_RTT_HasKey()) {
                        inputChar = SEGGER_RTT_GetKey();
                        if (inputChar == 'd' || inputChar == 'D' || inputChar == 'a' || inputChar == 'A') {
                            commandReceived = true;
                            break;
                        }
                    }
                }
                
                if (commandReceived) break;
                
                // Pattern 2: Pin1=LOW, Pin2=HIGH
                HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(currentPair.port1), currentPair.pin1, HAL_GPIO_PIN_RESET);
                HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(currentPair.port2), currentPair.pin2, HAL_GPIO_PIN_SET);
                
                // Check for RTT input during delay
                for (int i = 0; i < 50; i++) { // 500ms total delay in 10ms chunks
                    vTaskDelay(10);
                    if (SEGGER_RTT_HasKey()) {
                        inputChar = SEGGER_RTT_GetKey();
                        if (inputChar == 'd' || inputChar == 'D' || inputChar == 'a' || inputChar == 'A') {
                            commandReceived = true;
                            break;
                        }
                    }
                }
                
                if (commandReceived) break;
            }
            
            // Reset current pins to LOW before switching
            HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(currentPair.port1), currentPair.pin1, HAL_GPIO_PIN_RESET);
            HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(currentPair.port2), currentPair.pin2, HAL_GPIO_PIN_RESET);
            
            // Process command
            if (inputChar == 'd' || inputChar == 'D') {
                currentPairIndex = safeArray.nextIndex(currentPairIndex);
                SEGGER_RTT_printf(0, "Moving to NEXT pair\n\r");
            } else if (inputChar == 'a' || inputChar == 'A') {
                currentPairIndex = safeArray.prevIndex(currentPairIndex);
                SEGGER_RTT_printf(0, "Moving to PREVIOUS pair\n\r");
            }
            
            vTaskDelay(100); // Brief pause before next pair
        }
        
        return true; // Success
    };
    
    // Try common array sizes
    if (tryArraySize(std::integral_constant<size_t, 16>{})) return;
    if (tryArraySize(std::integral_constant<size_t, 32>{})) return;
    if (tryArraySize(std::integral_constant<size_t, 64>{})) return;
    if (tryArraySize(std::integral_constant<size_t, 8>{})) return;
    if (tryArraySize(std::integral_constant<size_t, 4>{})) return;
    
    SEGGER_RTT_printf(0, "GPIO Brute Force Task: Failed to determine array size or invalid array, terminating\n\r");
    vTaskDelete(nullptr);
}

}