# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

### Building the Project
```bash
cmake --build cmake-build-debug
```

### Configure with CMake Presets
```bash
cmake --preset Debug
cmake --preset Release
cmake --preset RelWithDebInfo
cmake --preset MinSizeRel
```

### Build with CMake Presets
```bash
cmake --build --preset Debug
cmake --build --preset Release
```

## Project Architecture

This is an STM32G474 microcontroller project using FreeRTOS and STM32 HAL drivers. The project is generated using STM32CubeMX and uses CMake as the build system.

### Key Components

- **STM32G474 Microcontroller**: ARM Cortex-M4F based MCU
- **FreeRTOS**: Real-time operating system for task management
- **STM32 HAL Drivers**: Hardware abstraction layer for peripheral access
- **CMSIS**: Cortex Microcontroller Software Interface Standard

### Directory Structure

- `Core/`: Main application code
  - `Inc/`: Header files including configuration
  - `Src/`: Source files including main.c, FreeRTOS app, and HAL MSP
- `Drivers/`: STM32 HAL drivers and CMSIS libraries
- `Middlewares/`: Third-party libraries (FreeRTOS)
- `cmake/`: CMake configuration files
- `startup_stm32g474xx.s`: Assembly startup file
- `STM32G474XX_FLASH.ld`: Linker script

### Build System

The project uses a two-level CMake structure:
1. Main `CMakeLists.txt` - Project configuration and user code
2. `cmake/stm32cubemx/CMakeLists.txt` - STM32CubeMX generated build configuration

The build system creates static libraries for:
- STM32_Drivers: HAL and LL drivers
- FreeRTOS: Real-time operating system components

### Key Files

- `Core/Src/main.c:72`: Application entry point with FreeRTOS initialization
- `Core/Src/app_freertos.c`: FreeRTOS application configuration
- `STM32G474.ioc`: STM32CubeMX project file for hardware configuration
- `STM32G474XX_FLASH.ld`: Memory layout definition

### Toolchain

- **Compiler**: ARM GCC (`cmake/gcc-arm-none-eabi.cmake`)
- **Target**: ARM Cortex-M4F
- **Language Standards**: C11 (configurable)
- **Build Generator**: Ninja (via presets)

### Development Notes

- The project uses STM32CubeMX for hardware configuration
- FreeRTOS is configured for ARM Cortex-M4F with heap_4 memory management
- Generated code requires C11 or higher standard
- Build artifacts are placed in `cmake-build-debug/` directory