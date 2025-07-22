#ifndef BQ40Z80_INTERNAL_H
#define BQ40Z80_INTERNAL_H

#include <cstdint>
#include "hal_types.h"

// Internal register definitions for BQ40Z80 implementation

namespace BQ40Z80::Internal {

// CRC-8 calculation for PEC
uint8_t calculateCRC8(const uint8_t* data, size_t length);

// Internal state tracking
struct DeviceState {
    bool initialized;
    bool sealed;
    bool fullAccess;
    uint32_t lastCommandTime;
    uint16_t firmwareVersion;
    uint16_t deviceType;
};

// Command validation
bool isValidCommand(uint8_t cmd);
bool requiresUnseal(uint8_t cmd);
bool isBlockCommand(uint8_t cmd);

// Timing helpers
void enforceCommandDelay(uint32_t lastCommandTime, uint32_t delayMs);

} // namespace BQ40Z80::Internal

#endif // BQ40Z80_INTERNAL_H