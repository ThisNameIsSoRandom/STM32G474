#[[
================================================================================
              FreeRTOS Configuration for STM32U575ZI-Q Board
================================================================================

OVERVIEW:
This configuration file sets up FreeRTOS real-time operating system for the
STM32U575ZI-Q microcontroller. It configures the FreeRTOS kernel for ARM
Cortex-M33 with TrustZone support, downloading and integrating FreeRTOS
kernel sources with board-specific settings.

FREERTOS SPECIFICATIONS:
- Version: FreeRTOS Kernel V10.6.2 (stable LTS release)
- Port: GCC_ARM_CM33_NTZ_NONSECURE (Cortex-M33 Non-secure TrustZone)
- Scheduler: Preemptive multitasking with priority-based scheduling
- Memory: heap_4 allocation algorithm (configurable)
- Tick: SysTick timer at 1000 Hz (1ms tick period)

TARGET-SPECIFIC CONFIGURATION:
- Architecture: ARM Cortex-M33 @ 160 MHz
- TrustZone: Non-secure world execution
- FPU: Single-precision floating-point context switching
- MPU: Memory Protection Unit integration (if enabled)
- Cache: Instruction cache coherency maintenance
- Power: Low-power tickless idle mode support

FREERTOS PORT FEATURES:
GCC_ARM_CM33_NTZ_NONSECURE port provides:
- Non-secure TrustZone world execution
- Automatic FPU context saving/restoration
- MPU support for memory protection
- SVC exception for kernel entry
- PendSV exception for context switching
- SysTick timer for time base
- Critical section management

KERNEL CONFIGURATION:
The FreeRTOS configuration is defined in FreeRTOSConfig.h:
- Task priorities: 0 (lowest) to configMAX_PRIORITIES-1 (highest)
- Stack size: Configurable per task (typically 128-2048 bytes)
- Heap size: Defined by configTOTAL_HEAP_SIZE
- Tick rate: configTICK_RATE_HZ (typically 1000 Hz)
- Idle hook: configUSE_IDLE_HOOK for low-power modes

MEMORY MANAGEMENT:
- Heap algorithm: heap_4 (coalescent, thread-safe allocation)
- Stack overflow: Stack overflow detection available
- Memory regions: Configurable memory protection via MPU
- Allocation: pvPortMalloc()/vPortFree() API
- Statistics: Memory usage tracking and reporting

SYNCHRONIZATION PRIMITIVES:
- Tasks: Concurrent execution units with individual stacks
- Queues: Inter-task communication and data exchange
- Semaphores: Binary and counting semaphores for synchronization
- Mutexes: Priority inheritance mutexes for resource protection
- Event Groups: Event flag synchronization
- Stream Buffers: High-performance byte streams

TIMING SERVICES:
- Software Timers: Callback-based periodic/one-shot timers
- Tick delays: vTaskDelay() for relative delays
- Absolute delays: vTaskDelayUntil() for periodic tasks
- Tick counting: xTaskGetTickCount() for time measurement
- Time conversion: pdMS_TO_TICKS() for millisecond conversion

INTERRUPT INTEGRATION:
- ISR-safe API: Functions ending with "FromISR"
- Priority: Configurable interrupt priority levels
- Nesting: Nested interrupt support with context preservation
- Yield: Interrupt-driven context switching via portYIELD_FROM_ISR()
- Critical sections: taskENTER/EXIT_CRITICAL() for atomic operations

LOW-POWER SUPPORT:
- Tickless idle: Automatic low-power mode during idle periods
- Sleep modes: Integration with STM32U5 low-power modes
- Wake sources: Interrupt-driven wake from sleep
- Clock gating: Peripheral clock management during idle
- STOP mode: Ultra-low-power stop mode with RAM retention

DEBUGGING FEATURES:
- Task list: Runtime task state and stack usage inspection
- Statistics: CPU usage and runtime statistics collection
- Trace: Task execution tracing via trace macros
- Assertions: Configurable assertion handling
- Stack checking: Stack overflow detection and reporting

DOWNLOAD MECHANISM:
Uses CMake FetchContent to download FreeRTOS kernel:
- Repository: https://github.com/FreeRTOS/FreeRTOS-Kernel.git
- Tag: V10.6.2 (LTS stable release)
- Shallow clone: Minimal download for faster builds
- Integration: Automatic CMake target creation

CONFIGURATION INTERFACE:
The freertos_config INTERFACE library provides:
- Include directories: Core/Inc for FreeRTOSConfig.h
- Compile definitions: Project-specific configuration macros
- System integration: Board-specific header includes
- Port selection: Automatic port selection based on target

INTEGRATION WITH APPLICATION:
- Task creation: Application tasks defined in app/Src/Tasks/
- Hardware abstraction: STM32 HAL integration for peripherals
- Memory mapping: Compatible with STM32U575 memory layout
- Startup: FreeRTOS initialization in main.c after HAL init
- Services: CMSIS-RTOS v2 wrapper for portability

PERFORMANCE CHARACTERISTICS:
- Context switch: ~50-100 cycles (depends on FPU usage)
- Interrupt latency: <10 instruction cycles
- Memory overhead: ~200 bytes RAM + 8KB+ Flash
- Task overhead: ~100-200 bytes per task
- Queue overhead: Configurable based on data size

================================================================================
]]

# FreeRTOS Configuration
cmake_minimum_required(VERSION 3.22)

# Create FreeRTOS config target FIRST before fetching FreeRTOS
add_library(freertos_config INTERFACE)

# FreeRTOS include directories
target_include_directories(freertos_config SYSTEM INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
)

# FreeRTOS compile definitions
target_compile_definitions(freertos_config INTERFACE
    projCOVERAGE_TEST=0
)

# FreeRTOS port configuration for ARM Cortex-M33
set(FREERTOS_PORT GCC_ARM_CM33_NTZ_NONSECURE CACHE STRING "FreeRTOS port")

include(FetchContent)

# Download FreeRTOS
FetchContent_Declare(
    freertos_kernel
    GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
    GIT_TAG        V10.6.2
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(freertos_kernel)