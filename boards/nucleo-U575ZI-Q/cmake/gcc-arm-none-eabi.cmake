#[[
================================================================================
                    STM32U575ZI-Q Board-Specific ARM GCC Toolchain
================================================================================

OVERVIEW:
This toolchain configuration file provides board-specific ARM GCC settings
for the STM32U575ZI-Q microcontroller on the NUCLEO-U575ZI-Q development board.
It configures the cross-compilation environment for this specific STM32U5 target.

BOARD SPECIFICATIONS:
- MCU: STM32U575ZITxQ (LQFP144 package)
- Core: ARM Cortex-M33 @ 160 MHz
- Flash: 2048 KB (0x08000000-0x081FFFFF)
- SRAM: 786 KB total (SRAM1: 192KB, SRAM2: 64KB, SRAM3: 512KB, SRAM4: 16KB)
- Package: LQFP144 (144-pin Low-profile Quad Flat Package)
- Board: NUCLEO-U575ZI-Q development board

MCU-SPECIFIC SETTINGS:
- CPU: ARM Cortex-M33 with TrustZone (Non-secure world)
- FPU: FPv4-SP-D16 single-precision floating-point unit
- Float ABI: Hard float (hardware floating-point linkage)
- Architecture: ARMv8-M Mainline with security extensions
- Instruction Set: Thumb-2 for optimal code density

MEMORY LAYOUT:
The linker script (STM32U575xx_FLASH.ld) defines:
- Flash memory: 0x08000000 (2048 KB) - Application code and constants
- SRAM1: 0x20000000 (192 KB) - Main application RAM
- SRAM2: 0x20030000 (64 KB) - Additional RAM for buffers
- SRAM3: 0x20040000 (512 KB) - Large data structures
- SRAM4: 0x28000000 (16 KB) - Backup SRAM (battery-backed)

COMPILER OPTIMIZATIONS:
- Debug builds: -O0 -g3 (no optimization, full debug info)
- Release builds: -Os -g0 (size optimization, no debug info)
- Section-based compilation for dead code elimination
- Memory usage reporting enabled (--print-memory-usage)

FLOATING-POINT CONFIGURATION:
- FPU: FPv4-SP-D16 (16 single-precision registers)
- ABI: Hard float (FP arguments passed in FP registers)
- IEEE 754-2008 compliant single-precision arithmetic
- Hardware acceleration for floating-point operations

LINKING CONFIGURATION:
- Linker script: STM32U575xx_FLASH.ld (board-specific memory layout)
- C library: newlib-nano (reduced footprint for embedded)
- System calls: none (bare-metal embedded application)
- Garbage collection: Remove unused sections (--gc-sections)
- Memory map: Generated for debugging (${PROJECT_NAME}.map)

BUILD OUTPUTS:
- Primary: ${PROJECT_NAME}.elf (executable with debug symbols)
- Map file: ${PROJECT_NAME}.map (memory usage and symbol locations)
- Binary formats: Can generate .bin and .hex via objcopy

BOARD-SPECIFIC FEATURES:
- TrustZone: Configured for Non-secure world execution
- MPU: 8-region Memory Protection Unit available
- Cache: Instruction cache (I-Cache) support
- Security: Secure/Non-secure attribution and isolation
- Power: Multiple low-power modes and clock management

DEVELOPMENT BOARD FEATURES (NUCLEO-U575ZI-Q):
- Debugger: ST-Link/V3E onboard programmer/debugger
- USB: USB-C connector for power and ST-Link communication
- Arduino: Arduino Uno V3 connectivity headers
- Morpho: ST Morpho extension connectors (full pin access)
- LEDs: User LEDs (LD1, LD2, LD3) and power LED
- Button: User button (B1) and reset button

PROGRAMMING/DEBUGGING:
- Interface: SWD (Serial Wire Debug) via ST-Link
- Programmer: STM32CubeProgrammer compatible
- Debugger: GDB with ST-Link, J-Link, or OpenOCD
- Real-time: SEGGER RTT support via debug interface

COMPATIBILITY:
- STM32CubeMX: Compatible with HAL/LL drivers
- STM32CubeIDE: Native project import support
- VSCode: Debug configuration for Cortex-Debug extension
- Eclipse CDT: CDT project files generated

PREREQUISITES:
- ARM GCC toolchain: arm-none-eabi-gcc in PATH
- STM32CubeCLT: STM32 development toolchain
- Linker script: STM32U575xx_FLASH.ld in project root
- Board support: STM32U5 HAL/LL drivers

================================================================================
]]

set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)

set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

# Some default GCC settings
# arm-none-eabi- must be part of path environment
set(TOOLCHAIN_PREFIX                arm-none-eabi-)

set(CMAKE_C_COMPILER                ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER              ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER              ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_LINKER                    ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_OBJCOPY                   ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE                      ${TOOLCHAIN_PREFIX}size)

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# MCU specific flags
set(TARGET_FLAGS "-mcpu=cortex-m33 -mfpu=fpv4-sp-d16 -mfloat-abi=hard ")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -fdata-sections -ffunction-sections")

set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-Os -g0")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -g0")

# C++ specific flags  
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -fno-threadsafe-statics")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -fdata-sections -ffunction-sections")

set(CMAKE_C_LINK_FLAGS "${TARGET_FLAGS}")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -T \"${CMAKE_SOURCE_DIR}/STM32U575xx_FLASH.ld\"")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} --specs=nano.specs")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group -lc -lm -Wl,--end-group")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--print-memory-usage")

set(CMAKE_CXX_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group -lstdc++ -lsupc++ -Wl,--end-group")