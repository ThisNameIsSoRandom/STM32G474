#[[
================================================================================
                           FreeRTOS Tasks Library
================================================================================

OVERVIEW:
This library provides the application-level FreeRTOS tasks for the STM32G474
multi-board project. It implements the real-time task scheduling and execution
logic for battery management, communication, and system monitoring functions.

PURPOSE:
- Real-time task implementations for embedded application logic
- FreeRTOS-based concurrent execution of system functions
- Battery monitoring and management task scheduling
- Communication protocol handling (SMBus, UART)
- System health monitoring and error handling
- Hardware abstraction layer (HAL) integration

TASK ARCHITECTURE:
The library implements several concurrent tasks:

1. Battery Monitor Task (battery_monitor_task.cpp):
   - Periodic battery status polling via BQ40Z80 driver
   - State-of-charge (SOC) and health monitoring
   - Battery protection and safety checks
   - Data logging and alerting

2. SMBus Task (smbus_task.cpp):
   - SMBus/I2C communication management
   - Protocol-level error handling and retries
   - Message queuing and processing
   - Device discovery and configuration

3. UART Task (uart_task.cpp):
   - Serial communication handling
   - Command processing and response generation
   - Debug output and user interface
   - Data streaming and logging

4. HAL Implementations (hal_implementations.cpp):
   - STM32 HAL driver integration
   - Hardware-specific implementations
   - Resource management and sharing
   - Error handling and recovery

FREERTOS INTEGRATION:
- Task creation and management using FreeRTOS API
- Inter-task communication via queues and semaphores
- Task synchronization and priority management
- Memory management using FreeRTOS heap
- Timer-based periodic task execution

TASK PRIORITIES:
- Battery Monitor: High priority (safety-critical)
- SMBus Communication: Medium priority (real-time data)
- UART Communication: Low priority (user interface)
- Background tasks: Idle priority (maintenance)

MEMORY USAGE:
- Stack allocation: 512-2048 bytes per task
- Queue buffers: Configurable based on message size
- Shared data structures: Minimal static allocation
- No dynamic memory allocation in task execution

TIMING REQUIREMENTS:
- Battery monitoring: 100ms-1s periodic updates
- SMBus communication: Event-driven with timeout handling
- UART processing: Asynchronous with buffering
- System watchdog: Regular task activity monitoring

ERROR HANDLING:
- Hardware communication timeouts and retries
- Task fault detection and recovery
- System-level error reporting via RTT
- Graceful degradation under fault conditions

TESTING:
- Unit tests with mocked HAL functions
- Task behavior validation
- Inter-task communication testing
- Performance and timing verification

DEPENDENCIES:
- FreeRTOS kernel (provided by board projects)
- RTT library (debug output and logging)
- BQ40Z80 library (battery management communication)
- STM32 HAL drivers (hardware abstraction)

CONFIGURATION:
Task parameters are configurable via FreeRTOS config:
- Task stack sizes
- Task priorities
- Queue depths
- Timer periods

================================================================================
]]

cmake_minimum_required(VERSION 3.16)

# Set system information for cross-compilation
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(Tasks)

# Create Tasks static library
add_library(${PROJECT_NAME} STATIC)

# Add task source files
target_sources(${PROJECT_NAME} PRIVATE
    battery_monitor_task.cpp
    vescan_task.cpp
    hal_implementations.cpp
)

# Include directories for tasks
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../VESCCan/Inc
)

# Add header files
target_sources(${PROJECT_NAME} PUBLIC
        Inc/freertos_tasks.h
        Inc/battery_monitor_task.h
        Inc/vescan_task.h
)

# No wydmuszka linking needed - platform provides HAL implementations

# Tasks need RTT, BQ40Z80, and VESCCan library
if(TARGET RTT)
    target_link_libraries(${PROJECT_NAME} PUBLIC RTT)
endif()

if(TARGET BQ40Z80)
    target_link_libraries(${PROJECT_NAME} PUBLIC BQ40Z80)
endif()

if(TARGET VESCCan)
    target_link_libraries(${PROJECT_NAME} PUBLIC VESCCan)
endif()

# C++ files are now native .cpp - no forced compilation needed