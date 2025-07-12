# STM32G474 FreeRTOS Project

A modern embedded systems project for the STM32G474 microcontroller featuring FreeRTOS real-time operating system and STM32 HAL drivers.

## Overview

This project provides a foundation for developing embedded applications on the STM32G474 microcontroller. It leverages the power of FreeRTOS for real-time task management and the comprehensive STM32 HAL (Hardware Abstraction Layer) for peripheral control.

## Hardware

- **Microcontroller**: STM32G474 (ARM Cortex-M4F)
- **Architecture**: 32-bit ARM Cortex-M4F with FPU
- **Flash**: Up to 512KB
- **RAM**: Up to 128KB
- **Clock**: Up to 170MHz

## Features

- **FreeRTOS Integration**: Real-time operating system with task scheduling
- **STM32 HAL Drivers**: Hardware abstraction layer for easy peripheral access
- **CMSIS Support**: ARM Cortex Microcontroller Software Interface Standard
- **CMake Build System**: Modern build configuration with presets
- **ARM GCC Toolchain**: Professional embedded development toolchain

## Project Structure

```
STM32G474/
├── Core/
│   ├── Inc/          # Header files and configuration
│   └── Src/          # Main application source code
├── Drivers/
│   ├── STM32G4xx_HAL_Driver/  # STM32 HAL drivers
│   └── CMSIS/        # ARM CMSIS libraries
├── Middlewares/
│   └── Third_Party/
│       └── FreeRTOS/ # Real-time operating system
├── cmake/            # CMake configuration files
└── startup_stm32g474xx.s  # Assembly startup file
```

## Build Instructions

### Prerequisites

- ARM GCC toolchain
- CMake 3.22+
- Ninja build system

### Building

```bash
# Configure with Debug preset
cmake --preset Debug

# Build the project
cmake --build --preset Debug
```

### Available Build Presets

- `Debug`: Development build with debugging symbols
- `Release`: Optimized production build
- `RelWithDebInfo`: Release with debug information
- `MinSizeRel`: Size-optimized release build

## Getting Started

1. **Clone the repository**
   ```bash
   git clone <repository-url>
   cd STM32G474
   ```

2. **Build the project**
   ```bash
   cmake --preset Debug
   cmake --build --preset Debug
   ```

3. **Flash to target**
   - Use your preferred STM32 programmer (ST-Link, J-Link, etc.)
   - Flash the generated `.elf` file to your STM32G474 board

## Development

The main application entry point is located in `Core/Src/main.c`. The project is configured to use FreeRTOS with a default task that can be extended for your specific application needs.

### Key Components

- **main.c**: Application entry point and system initialization
- **app_freertos.c**: FreeRTOS application configuration
- **FreeRTOSConfig.h**: FreeRTOS kernel configuration
- **stm32g4xx_hal_conf.h**: HAL driver configuration

## Contributing

This project follows embedded systems best practices and maintains compatibility with STM32CubeMX for hardware configuration management.

## License

This project contains components with various licenses:
- STM32 HAL drivers: STMicroelectronics license
- FreeRTOS: MIT license
- CMSIS: Apache 2.0 license

See individual component directories for specific license information.