#[[
================================================================================
                        ARM GCC Base Toolchain Configuration
================================================================================

OVERVIEW:
This is the base toolchain configuration for ARM GCC cross-compilation across
all ARM Cortex-M variants used in the STM32G474 project. It provides common
compiler settings, flags, and configurations that are inherited by specific
Cortex-M variant toolchain files.

PURPOSE:
- Unified ARM GCC toolchain configuration for cross-compilation
- Common compiler and linker flags for embedded ARM targets
- Optimized settings for STM32 microcontroller development
- Consistent build environment across multiple board targets
- Support for both C and C++ compilation with embedded constraints

SUPPORTED TARGETS:
This base configuration supports all ARM Cortex-M variants:
- Cortex-M0+ (used in low-power applications)
- Cortex-M4F (used in STM32G474, STM32L476RG)
- Cortex-M7 (used in STM32H755ZI-Q dual-core)
- Cortex-M33 (used in STM32U575ZI-Q with TrustZone)

TOOLCHAIN COMPONENTS:
- Compiler: arm-none-eabi-gcc (C/C++)
- Assembler: arm-none-eabi-gcc (ASM)
- Linker: arm-none-eabi-g++
- Object utilities: arm-none-eabi-objcopy, arm-none-eabi-size
- Architecture: ARM EABI (Embedded ABI)
- Float ABI: Hard float (where supported)

COMPILATION FLAGS:
Common flags applied to all variants:
- -Wall -Wextra -Wpedantic: Comprehensive warning reporting
- -fdata-sections -ffunction-sections: Section-based dead code elimination
- -fno-rtti -fno-exceptions: Disable C++ runtime features for embedded
- -fno-threadsafe-statics: Disable thread-safe static initialization

OPTIMIZATION LEVELS:
- Debug: -O0 -g3 (no optimization, full debug symbols)
- Release: -Os -g0 (size optimization, no debug symbols)
- Focus on code size optimization for memory-constrained MCUs

LINKING CONFIGURATION:
- newlib-nano: Reduced-size C library for embedded systems
- nosys: No system call implementation (for bare-metal)
- Garbage collection: Remove unused sections (--gc-sections)
- Start/end groups: Proper library linking order
- C++ support: Include libstdc++ and libsupc++ when needed

MEMORY OPTIMIZATION:
- Section-based compilation enables fine-grained dead code removal
- Size-optimized builds (-Os) prioritize flash usage efficiency
- Newlib-nano reduces RAM footprint of standard library
- Function/data sections allow linker to remove unused code

EMBEDDED-SPECIFIC FEATURES:
- Static library compilation target (no dynamic linking)
- Hard float ABI support (where available)
- Assembly language support with C preprocessor
- Memory-mapped I/O compatibility
- Interrupt service routine support

USAGE:
This base configuration is included by specific Cortex-M variant files:
- cortex-m0plus.cmake
- cortex-m4f.cmake  
- cortex-m7.cmake
- cortex-m33.cmake

Each variant adds CPU-specific flags (FPU, architecture features) while
inheriting the common base configuration defined here.

DEPENDENCIES:
- ARM GCC toolchain installed and in PATH
- CMake 3.16+ with cross-compilation support
- ARM Cortex-M target hardware or simulator

================================================================================
]]

# Base ARM GCC toolchain configuration
# Common settings for all ARM Cortex variants

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

# ARM GCC toolchain
set(TOOLCHAIN_PREFIX arm-none-eabi-)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_LINKER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE ${TOOLCHAIN_PREFIX}size)

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Common flags for all ARM variants
set(COMMON_FLAGS "-Wall -Wextra -Wpedantic -fdata-sections -ffunction-sections")

# Debug/Release flags
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-Os -g0")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -g0")

# C++ specific flags
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -fno-rtti -fno-exceptions -fno-threadsafe-statics")

# Link flags
set(CMAKE_C_LINK_FLAGS "--specs=nano.specs --specs=nosys.specs")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--gc-sections")
set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group -lc -lm -Wl,--end-group")

set(CMAKE_CXX_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--start-group -lstdc++ -lsupc++ -Wl,--end-group")