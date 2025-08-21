#[[
================================================================================
                           VESC CAN Communication Library
================================================================================

OVERVIEW:
This library provides VESC (Vedder Electronic Speed Controller) CAN-Bus 
communication functionality for high-performance motor control applications.
It simplifies encoding and decoding of VESC CAN frames into easy-to-work-with
data structures for embedded systems.

PURPOSE:
- CAN-Bus communication with VESC motor controllers
- Frame encoding/decoding for VESC command and status messages
- Support for multiple VESC device types (BLDC, Cubemars, Stepper)
- Platform-agnostic protocol implementation with hardware abstraction
- Real-time motor control and status monitoring

FEATURES:
- Multiple VESC command types (duty cycle, current, RPM, position control)
- Comprehensive status frame parsing (11 different status message types)
- Endianness handling for cross-platform compatibility
- Scaling and unit conversion for motor control parameters
- Support for extended CAN IDs (29-bit addressing)
- Thread-safe operation in multi-threaded environments

INTEGRATION:
VESC CAN communication requires platform-specific CAN hardware implementation.
The library provides protocol-level functionality while relying on weak HAL
implementations for actual CAN transmission/reception.

VESC COMMAND TYPES:
- SET_DUTY: Duty cycle control (% / 100)
- SET_CURRENT: Current control (Amperes)
- SET_RPM: Speed control (RPM)
- SET_POS: Position control (Degrees)
- SET_ORIGIN: Position reference setting
- SET_POS_SPEED_LOOP: Advanced position/speed control

VESC STATUS TYPES:
- STATUS_1: ERPM, current, duty cycle
- STATUS_2: Amp hours consumed/charged
- STATUS_3: Watt hours consumed/charged
- STATUS_4: Temperatures, input current, PID position
- STATUS_5: Tachometer, input voltage
- STATUS_6: ADC readings, PPM value
- STATUS_7: Precise position (64-bit)
- STATUS_8: Weight sensors, distance, environmental data
- STATUS_9: Chemical sensors (NPK, pH)
- STATUS_10: Communication flags, control mode
- STATUS_11: Cubemars-specific status (position, speed, current, temperature)

USAGE IN APPLICATION:
```c
#include "VESC.h"

// Create command frame
VESC_CommandFrame cmd;
cmd.command = VESC_COMMAND_SET_CURRENT;
cmd.commandData = 5.0; // 5 Amperes

// Convert to raw CAN frame
VESC_RawFrame rawFrame;
VESC_convertCmdToRaw(&rawFrame, &cmd);

// Transmit via CAN (platform-specific)
HAL_CAN_AddTxMessage(&hcan, &header, rawFrame.rawData, &mailbox);

// Parse received status frame
VESC_Status_1 status;
VESC_convertRawToStatus1(&status, &receivedFrame);
float erpm = status.erpm;
float current = status.current;
```

FILES:
- VESC.h: Main header including all components
- VESC_Convert.c/h: Frame conversion functions
- VESC_Endian.c/h: Endianness handling utilities
- VESC_Consts.h: Constants, scales, and enumerations
- VESC_Defines.h: Basic type definitions
- VESC_Structs.h: Data structure definitions
- VESC_Status_*_Consts.h: Status-specific constants

MEMORY USAGE:
- Minimal RAM footprint (stack-based conversions)
- No dynamic memory allocation
- Configurable frame buffer sizes
- Stateless conversion functions

PERFORMANCE:
- Fast integer-based scaling operations
- Efficient bit manipulation for endianness
- Direct memory mapping for frame data
- Suitable for real-time motor control applications

CAN HARDWARE REQUIREMENTS:
- STM32 CAN peripheral (CAN1/CAN2/FDCAN)
- 29-bit extended CAN ID support
- CAN transceiver (TJA1050, SN65HVD230, etc.)
- Platform-specific HAL implementation for transmission/reception

================================================================================
]]

# VESCCan library target
add_library(VESCCan STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/VESC_Convert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/VESC_Endian.c
)

# Set include directories as PUBLIC so they propagate to consumers
target_include_directories(VESCCan PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

# VESCCan will inherit compile options from parent project

# Set properties for better integration
set_target_properties(VESCCan PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS YES
)