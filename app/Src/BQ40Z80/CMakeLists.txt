#[[
================================================================================
                      BQ40Z80 Battery Management IC Driver Library
================================================================================

OVERVIEW:
This library provides a complete driver for the Texas Instruments BQ40Z80
battery fuel gauge and protection IC. The BQ40Z80 is a fully integrated,
single-chip battery management solution for 1-series to 4-series cell Li-Ion
and Li-Polymer battery packs.

PURPOSE:
- I2C/SMBus communication with BQ40Z80 battery management IC
- Battery state monitoring (voltage, current, temperature, capacity)
- Battery protection and safety management
- Real-time battery data acquisition for embedded systems
- Integration with STM32 HAL I2C drivers

FEATURES:
- SMBus/I2C protocol implementation for BQ40Z80 communication
- High-level API for battery status and configuration
- Low-level register access for advanced functionality
- Error handling and communication timeout management
- Thread-safe operation in FreeRTOS environment
- Comprehensive battery data structure definitions

BQ40Z80 CAPABILITIES:
- 1-series to 4-series Li-Ion/Li-Polymer support
- Integrated analog front-end (AFE) for cell monitoring
- Pack-level current and voltage measurement
- Temperature monitoring with multiple thermistors
- State-of-charge (SOC) and state-of-health (SOH) calculation
- Battery protection and safety features
- Manufacturing and authentication data storage

COMMUNICATION:
- SMBus/I2C interface at up to 400 kHz
- Standard SMBus commands and manufacturer-specific extensions
- Block read/write operations for data transfer
- CRC-8 error checking for data integrity
- Configurable I2C address (0x0B default)

API STRUCTURE:
High-level functions:
- bq40z80_init(): Initialize communication and verify IC presence
- bq40z80_read_status(): Get comprehensive battery status
- bq40z80_get_voltage(): Read pack voltage
- bq40z80_get_current(): Read pack current
- bq40z80_get_temperature(): Read temperature measurements

Low-level functions:
- bq40z80_read_word(): Read 16-bit register
- bq40z80_write_word(): Write 16-bit register
- bq40z80_read_block(): Read data block
- bq40z80_write_block(): Write data block

INTEGRATION:
Requires STM32 HAL I2C driver for hardware communication:
- HAL_I2C_Master_Transmit()
- HAL_I2C_Master_Receive()
- HAL_I2C_Mem_Read()
- HAL_I2C_Mem_Write()

TESTING:
Unit tests provide mock HAL functions for hardware-independent testing:
- Mock I2C communication responses
- Simulated battery data scenarios
- Error condition testing
- Communication timeout validation

FILES:
- bq40z80.cpp: High-level driver API implementation
- bq40z80_lowlevel.cpp: Low-level SMBus/I2C communication
- Inc/bq40z80.h: Public API and data structure definitions

DEPENDENCIES:
- STM32 HAL I2C driver (hardware abstraction layer)
- RTT library (for debug logging and error reporting)
- Standard C++ library (minimal subset for embedded use)

MEMORY USAGE:
- Minimal RAM footprint (~100-200 bytes static data)
- No dynamic memory allocation
- Configurable communication buffer sizes
- Stack usage: ~50-100 bytes per function call

================================================================================
]]

# BQ40Z80 Battery Gauge Driver Library
cmake_minimum_required(VERSION 3.16)

# Set system information for cross-compilation
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Define BQ40Z80 library
add_library(BQ40Z80 STATIC
    Src/bq40z80.cpp
    Src/bq40z80_lowlevel.cpp
)

# Include directories
target_include_directories(BQ40Z80 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Inc
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Inc  # app/Inc directory
        ${CMAKE_CURRENT_SOURCE_DIR}/../Tasks/Inc  # For hal_types.h with DEBUG_LOG macro
)

# Link with parent app includes for HAL types
target_link_libraries(BQ40Z80 
    PRIVATE
        RTT  # For logging unimplemented functions
)

# Compile options
target_compile_options(BQ40Z80 PRIVATE
    -fno-exceptions
    -fno-rtti
    -fno-threadsafe-statics
)

# Export library for tasks
set_target_properties(BQ40Z80 PROPERTIES
    POSITION_INDEPENDENT_CODE OFF
)