# STM32 Multi-Board Project

A modern multi-board embedded systems project supporting various STM32 microcontrollers with FreeRTOS, featuring a modular app library architecture and C++17 compilation support.

## Overview

This project provides a unified foundation for developing embedded applications across multiple STM32 microcontroller platforms. It features a clean separation between platform-specific board implementations and reusable application code, with comprehensive build system support and modern C++17 capabilities.

## Supported Boards

### STM32G474 (custom-G474)
- **Microcontroller**: STM32G474 (ARM Cortex-M4F)
- **Features**: FPU, up to 170MHz, 512KB Flash, 128KB RAM
- **Status**: Single-core with FreeRTOS

### STM32H755ZI-Q (nucleo-H755ZI-Q)  
- **Microcontroller**: STM32H755ZI-Q (Dual-core ARM Cortex-M7 + M4)
- **CM7**: 480 MHz main core with FreeRTOS
- **CM4**: 240 MHz secondary core
- **Features**: Asymmetric dual-core architecture with HSEM synchronization
- **Debug**: VSCode multicore debugging support

### STM32U575ZI-Q (nucleo-U575ZI-Q)
- **Microcontroller**: STM32U575ZI-Q (ARM Cortex-M33)
- **Features**: TrustZone, up to 160MHz, 2MB Flash, 768KB RAM  
- **Status**: Single-core with FreeRTOS

## Architecture

### Project Structure
```
STM32G474/
├── app/                          # Shared application library
│   ├── Inc/                      # Public headers
│   ├── Src/                      # Source directory
│   │   ├── RTT/                  # SEGGER RTT submodule
│   │   ├── Tasks/                # FreeRTOS tasks submodule
│   │   │   ├── hal_implementations.cpp  # Weak HAL fallbacks
│   │   │   ├── smbus_task.cpp    # SMBus communication task
│   │   │   └── uart_task.cpp     # UART communication task
│   │   ├── logging.cpp           # Application logging
│   │   └── placeholder.cpp       # App utilities
│   └── uTests/                   # Unit tests
├── boards/                       # Board-specific implementations
│   ├── custom-G474/              # STM32G474 board
│   ├── nucleo-H755ZI-Q/          # STM32H755 dual-core board
│   └── nucleo-U575ZI-Q/          # STM32U575 board
├── toolchains/                   # Cross-compilation toolchains
│   ├── cortex-m4f.cmake          # Cortex-M4F configuration
│   ├── cortex-m7.cmake           # Cortex-M7 configuration
│   └── cortex-m33.cmake          # Cortex-M33 configuration
└── CLAUDE.md                     # Development guidelines
```

### App Library Features
- **Platform-agnostic design**: Builds standalone or with any board
- **Weak symbol linking**: HAL functions with platform overrides
- **C++17 compilation**: Modern C++ with .cpp extensions
- **RTT logging**: Default implementations log warnings via SEGGER RTT
- **Canonical structure**: Standard Inc/Src layout with submodules

### Key Components
- **Tasks**: SMBus and UART communication tasks with HAL abstraction
- **RTT**: SEGGER Real-Time Transfer for debugging output
- **HAL Abstraction**: Platform-agnostic HAL function declarations
- **Weak Implementations**: Default HAL behaviors with platform overrides

## Build Instructions

### Prerequisites
- **STM32CubeCLT 1.18.0+**: Complete toolchain with ARM GCC, CMake, OpenOCD
- **CMake 3.22+**: Build system
- **Ninja**: Fast parallel builds

### Building

#### Default Board (STM32G474)
```bash
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug
```

#### Specific Board Selection
```bash
# STM32G474 board
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DBOARD=custom-G474
cmake --build cmake-build-debug

# STM32H755ZI-Q dual-core board  
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DBOARD=nucleo-H755ZI-Q
cmake --build cmake-build-debug

# STM32U575ZI-Q board
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug -DBOARD=nucleo-U575ZI-Q
cmake --build cmake-build-debug
```

#### CMake Presets
```bash
# Configure
cmake --preset Debug
cmake --preset Release

# Build
cmake --build --preset Debug
cmake --build --preset Release
```

### Build Targets
- **app**: Shared application library
- **RTT**: SEGGER RTT debugging library  
- **Tasks**: FreeRTOS tasks with HAL abstraction
- **[board-name]**: Final firmware executable

## Development

### C++17 Port
- **App files**: Native .cpp compilation with C++17 standard
- **Platform compatibility**: main.c compiles as C++ while keeping .c extension
- **STM32CubeMX compatibility**: Generated code works with C++ compilation
- **FreeRTOS integration**: C++ tasks with extern "C" linkage

### VSCode Multicore Debugging (STM32H755ZI-Q)
Complete dual-core debugging workflow with STM32CubeCLT integration:

```bash
# Quick setup (builds, flashes, starts debug server)
Ctrl+Shift+P → "Tasks: Run Task" → "Setup Multicore Debug"

# Individual debugging
F5 → "Debug CM7" or "Debug CM4"
```

**Features**:
- Simultaneous CM7 + CM4 debugging
- FreeRTOS thread awareness on both cores
- STM32CubeCLT toolchain integration
- Automatic symbol loading and breakpoints

### Adding New Boards
1. Create board directory in `boards/[board-name]/`
2. Add STM32CubeMX project and generated code
3. Create board-specific CMakeLists.txt
4. Add toolchain configuration in `toolchains/`
5. Update root CMakeLists.txt with board option

### HAL Function Override
Platform can override app's weak implementations:
```cpp
// App provides weak fallback with RTT warning
__weak void HAL_Delay_MS(uint32_t ms) {
    SEGGER_RTT_printf(0, "WARNING: HAL_Delay_MS not implemented by platform\n");
}

// Platform overrides with real implementation  
extern "C" void HAL_Delay_MS(uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}
```

## Getting Started

1. **Clone and build**
   ```bash
   git clone <repository-url>
   cd STM32G474
   cmake -B cmake-build-debug -DBOARD=nucleo-U575ZI-Q
   cmake --build cmake-build-debug
   ```

2. **Flash firmware**
   - ELF output: `boards/[board]/build/[board].elf`
   - Use ST-Link, J-Link, or OpenOCD

3. **Debug output**
   - Connect SEGGER RTT Viewer or use VSCode RTT extension
   - View task logging and HAL implementation warnings

## Contributing

This project follows modern embedded C++ practices:
- C++17 standard with embedded-friendly features
- Platform-agnostic app design with HAL abstraction  
- Comprehensive .gitignore for clean version control
- STM32CubeMX integration for hardware configuration

## License

This project contains components with various licenses:
- **STM32 HAL drivers**: STMicroelectronics license
- **FreeRTOS**: MIT license  
- **CMSIS**: Apache 2.0 license
- **SEGGER RTT**: BSD-3-Clause license

See individual component directories for specific license information.