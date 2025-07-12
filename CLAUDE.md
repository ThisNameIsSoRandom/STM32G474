# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

### Building the Project

#### Default Board (STM32G474)
```bash
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug
```

#### Specific Board Selection
```bash
# Build STM32G474 board
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DBOARD=custom-G474
cmake --build cmake-build-debug

# Build STM32H755ZI-Q dual-core board
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DBOARD=nucleo-H755ZI-Q
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

This is a multi-board STM32 microcontroller project supporting both single-core and dual-core MCUs. The project uses STM32CubeMX for hardware configuration and CMake as the build system.

### Supported Boards

- **custom-G474**: STM32G474 single-core ARM Cortex-M4F MCU with FreeRTOS
- **nucleo-H755ZI-Q**: STM32H755ZI-Q dual-core MCU (Cortex-M7 + Cortex-M4)
  - CM7: Main core running at 480 MHz with FreeRTOS
  - CM4: Secondary core running at 240 MHz

### Key Components

- **STM32 HAL Drivers**: Hardware abstraction layer for peripheral access
- **FreeRTOS**: Real-time operating system for task management
- **CMSIS**: Cortex Microcontroller Software Interface Standard
- **Board Support Package (BSP)**: Board-specific drivers (for Nucleo boards)

### Directory Structure

```
├── boards/                  # Board-specific implementations
│   ├── custom-G474/        # STM32G474 single-core board
│   │   ├── Core/          # Application code
│   │   ├── Drivers/       # HAL and CMSIS drivers
│   │   ├── Middlewares/   # FreeRTOS
│   │   ├── cmake/         # Build configuration
│   │   └── STM32G474.ioc  # STM32CubeMX project
│   └── nucleo-H755ZI-Q/   # STM32H755 dual-core board
│       ├── CM7/           # Cortex-M7 core files
│       ├── CM4/           # Cortex-M4 core files
│       ├── Common/        # Shared boot code
│       ├── Drivers/       # HAL, CMSIS, and BSP
│       ├── Middlewares/   # FreeRTOS
│       ├── cmake/         # Build configuration
│       └── nucleo-H755ZI-Q.ioc  # STM32CubeMX project
└── app/                    # Shared application code (G474 only)

### Build System

The project uses a multi-level CMake structure:

1. **Root `CMakeLists.txt`**: Board selection and global configuration
2. **Board-specific `CMakeLists.txt`**: Target configuration and build setup
3. **`cmake/stm32cubemx/CMakeLists.txt`**: STM32CubeMX generated build configuration

Key build features:
- Board selection via `-DBOARD=<board-name>` CMake option
- Unified build structure for both single and dual-core boards
- Automatic toolchain configuration based on selected board
- Support for C++17 compilation of main.c files

### Key Files

**custom-G474 Board:**
- `boards/custom-G474/Core/Src/main.c`: Application entry point with FreeRTOS
- `boards/custom-G474/STM32G474.ioc`: STM32CubeMX configuration
- `boards/custom-G474/STM32G474XX_FLASH.ld`: Memory layout

**nucleo-H755ZI-Q Board:**
- `boards/nucleo-H755ZI-Q/CM7/Core/Src/main.c`: CM7 main with FreeRTOS
- `boards/nucleo-H755ZI-Q/CM4/Core/Src/main.c`: CM4 main
- `boards/nucleo-H755ZI-Q/nucleo-H755ZI-Q.ioc`: Dual-core configuration
- `boards/nucleo-H755ZI-Q/CM7/stm32h755xx_flash_CM7.ld`: CM7 memory layout
- `boards/nucleo-H755ZI-Q/CM4/stm32h755xx_flash_CM4.ld`: CM4 memory layout

### Toolchain

- **Compiler**: ARM GCC (`boards/<board>/cmake/gcc-arm-none-eabi.cmake`)
- **Targets**: 
  - custom-G474: ARM Cortex-M4F
  - nucleo-H755ZI-Q: ARM Cortex-M7 + Cortex-M4
- **Language Standards**: C11 (minimum), C++17 support
- **Build Generator**: Ninja (via presets)

### Development Notes

- The project uses STM32CubeMX for hardware configuration
- FreeRTOS is configured with heap_4 memory management
- Generated code requires C11 or higher standard
- Build artifacts are placed in `cmake-build-debug/` directory
- Board selection is persistent across builds using CMake cache

### Dual-Core Architecture (nucleo-H755ZI-Q)

The STM32H755 features asymmetric dual-core architecture:

**Boot Sequence:**
1. CM7 boots first and initializes system (clocks, peripherals)
2. CM4 waits in STOP mode for CM7 initialization
3. CM7 releases hardware semaphore (HSEM) to wake CM4
4. Both cores run independently with their own flash and RAM regions

**Core Characteristics:**
- **CM7**: 480 MHz, FPU, runs FreeRTOS, main application logic
- **CM4**: 240 MHz, FPU, secondary processing tasks

### C++17 Port

All `main.c` files are configured to compile as C++17 while maintaining the `.c` extension for STM32CubeMX compatibility:

- **CMake Configuration**: Uses `set_source_files_properties()` to force C++ compilation
- **Language Standards**: C++17 with appropriate compiler flags (`-fno-rtti`, `-fno-exceptions`, `-fno-threadsafe-statics`)
- **C Compatibility**: Headers wrapped in `extern "C"` blocks to maintain C linkage
- **Struct Initialization**: Modified to use positional initialization instead of designated initializers

### VSCode Multicore Debugging (nucleo-H755ZI-Q)

The nucleo-H755ZI-Q board includes VSCode configuration for multicore debugging with both CM7 and CM4 cores running FreeRTOS using STM32CubeCLT on Windows 11:

**VSCode Tasks (Windows 11 + STM32CubeCLT):**
- **Configure CMake**: Configure build system with Ninja generator
- **Build All Cores**: Build both CM7 and CM4 targets
- **Build CM7/CM4**: Build individual core targets  
- **Clean**: Clean build artifacts
- **Flash CM7/CM4**: Flash individual cores via ST-Link
- **Flash Both Cores**: Flash both cores sequentially
- **Start OpenOCD Server**: Start debug server (background task)
- **Stop OpenOCD Server**: Stop debug server
- **Debug CM7/CM4**: Launch GDB debugging sessions
- **Setup Multicore Debug**: Complete workflow (build → flash → start debug server)

**Prerequisites:**
- **STM32CubeCLT_1.18.0**: Complete toolchain installed at `C:/ST/STM32CubeCLT_1.18.0/`
  - CMake: `C:/ST/STM32CubeCLT_1.18.0/CMake/bin/`
  - GNU tools: `C:/ST/STM32CubeCLT_1.18.0/GNU-tools-for-STM32/bin/`
  - OpenOCD: `C:/ST/STM32CubeCLT_1.18.0/OpenOCD/bin/`
- **VSCode Extensions**: C/C++ and CMake Tools
- **ST-Link**: Nucleo-H755ZI-Q onboard ST-Link/V3

**Multicore Debug Workflow:**
1. **Quick Start**: Run `Setup Multicore Debug` task
   - Configures CMake with Ninja generator
   - Builds both CM7 and CM4 cores
   - Flashes both cores to target
   - Starts OpenOCD server in background
2. **Individual Debug**: Run `Debug CM7` or `Debug CM4` tasks
   - Each opens GDB in separate terminal (ports 3333/3334)
   - Automatic symbol loading and main() breakpoint
3. **Simultaneous Debug**: Run both debug tasks for dual-core debugging
4. **Cleanup**: Run `Stop OpenOCD Server` when finished

**STM32CubeCLT Integration:**
- **CMake**: `cmake.exe` from STM32CubeCLT_1.18.0 with `-j 10` parallel builds
- **Compiler**: `arm-none-eabi-gcc.exe` from STM32CubeCLT_1.18.0
- **Debugger**: `arm-none-eabi-gdb.exe` from STM32CubeCLT_1.18.0  
- **OpenOCD**: `openocd.exe` from STM32CubeCLT_1.18.0 with ST-Link support
- **Generator**: Ninja for fast parallel builds
- **IntelliSense**: Configured for ARM GCC toolchain

**Debug Configurations (F5 / Debug Panel):**
- **Debug CM7**: Launch debugger for CM7 core with auto-build
- **Debug CM4**: Launch debugger for CM4 core with auto-build  
- **Attach to CM7**: Attach to running CM7 (OpenOCD must be running)
- **Attach to CM4**: Attach to running CM4 (OpenOCD must be running)
- **Debug Both Cores**: Compound configuration for simultaneous debugging

**Configuration Files:**
- `.vscode/launch.json`: Debug configurations using `cppdbg` type
- `.vscode/tasks.json`: All build, flash, and debug tasks
- `.vscode/settings.json`: STM32CubeCLT toolchain paths and CMake settings
- `.vscode/extensions.json`: Required VSCode extensions