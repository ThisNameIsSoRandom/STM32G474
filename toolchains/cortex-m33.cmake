#[[
================================================================================
                        ARM Cortex-M33 Toolchain Configuration
================================================================================

OVERVIEW:
This toolchain configuration file provides ARM Cortex-M33 specific compiler
and linker settings for STM32U5 series microcontrollers. It extends the base
ARM GCC configuration with Cortex-M33 architecture features and optimizations.

TARGET SPECIFICATIONS:
- Architecture: ARM Cortex-M33 with ARMv8-M mainline
- FPU: Single-precision floating-point unit (FPv5-SP-D16)
- Float ABI: Hard float (hardware floating-point linkage)
- Security: ARM TrustZone for ARMv8-M support
- DSP: DSP instructions and SIMD operations
- MPU: Memory Protection Unit with 8 regions

CORTEX-M33 FEATURES:
- 32-bit ARM processor with Thumb-2 instruction set
- Single-precision floating-point unit (FPU)
- TrustZone security extensions for secure/non-secure worlds
- Digital Signal Processing (DSP) instructions
- Hardware divide instructions
- Memory Protection Unit (MPU)
- Nested Vectored Interrupt Controller (NVIC)
- SysTick timer and debug support

COMPILER FLAGS:
- -mcpu=cortex-m33: Target Cortex-M33 processor architecture
- -mfpu=fpv5-sp-d16: Use FPv5 single-precision FPU with 16 double registers
- -mfloat-abi=hard: Use hardware floating-point ABI (passes FP args in FP regs)

PERFORMANCE CHARACTERISTICS:
- Pipeline: 3-stage pipeline with branch prediction
- Performance: Up to 1.5 DMIPS/MHz
- Memory: Harvard architecture with separate instruction/data buses
- Cache: Optional instruction cache (I-Cache) support
- Power: Low-power design with multiple sleep modes

MEMORY CONFIGURATION:
- Address space: 32-bit unified address space
- Code: Typically 0x08000000+ (Flash memory)
- SRAM: Typically 0x20000000+ (SRAM1/SRAM2/SRAM3)
- Peripherals: 0x40000000+ (APB/AHB peripherals)
- System: 0xE0000000+ (System control and debug)

FLOATING-POINT SUPPORT:
The hard float ABI provides:
- Hardware acceleration for single-precision operations
- IEEE 754-2008 compliant floating-point arithmetic
- Automatic use of FPU registers for function arguments
- Optimized floating-point library functions
- Reduced code size and improved performance vs. soft float

TRUSTZONE INTEGRATION:
When TrustZone is enabled:
- Secure and Non-secure worlds with separate memory spaces
- Secure/Non-secure state transitions via secure gateway
- Attribution units for memory/peripheral security assignment
- This toolchain targets Non-secure world applications

OPTIMIZATION:
- Thumb-2 instruction set provides optimal code density
- Hardware floating-point reduces computation overhead
- DSP instructions accelerate signal processing operations
- Branch prediction improves pipeline efficiency

USAGE EXAMPLES:
STM32U5 series MCUs using this toolchain:
- STM32U575ZI (used in NUCLEO-U575ZI-Q board)
- STM32U585AI/U585CI
- STM32U595AI/U595CI
- STM32U599AI/U599CI

INHERITANCE:
This file includes arm-gcc-base.cmake and adds Cortex-M33 specific flags
to the common ARM GCC configuration. All base settings (warnings, 
optimization levels, linking) are inherited.

================================================================================
]]

# ARM Cortex-M33 toolchain (STM32U5 series)
include(${CMAKE_CURRENT_LIST_DIR}/arm-gcc-base.cmake)

# Cortex-M33 specific flags
set(TARGET_FLAGS "-mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard")

# Apply target flags
set(CMAKE_C_FLAGS "${COMMON_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TARGET_FLAGS}")

# Add target flags to link flags
set(CMAKE_C_LINK_FLAGS "${TARGET_FLAGS} ${CMAKE_C_LINK_FLAGS}")
set(CMAKE_CXX_LINK_FLAGS "${TARGET_FLAGS} ${CMAKE_CXX_LINK_FLAGS}")

# Set variant identifier
set(MCU_VARIANT "cortex-m33" CACHE STRING "MCU variant")