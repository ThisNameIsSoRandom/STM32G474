# STM32 Multi-Board Project

A modern multi-board embedded systems project supporting various STM32 microcontrollers with FreeRTOS, featuring a modular app library architecture and C++17 compilation support.

## Overview

This project provides a unified foundation for developing embedded applications across multiple STM32 microcontroller platforms. It features a clean separation between platform-specific board implementations and reusable application code, with comprehensive build system support and modern C++17 capabilities.

## Supported Boards

### STM32G474 (custom-G474)
- **Microcontroller**: STM32G474 (ARM Cortex-M4F)
- **Features**: FPU, up to 170MHz, 512KB Flash, 128KB RAM
- **Status**: Single-core with FreeRTOS


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
│   │   │   ├── battery_monitor_task.cpp # Battery management task
│   │   │   └── vescan_task.cpp    # VESCAN motor controller task
│   │   ├── logging.cpp           # Application logging
│   │   └── placeholder.cpp       # App utilities
├── boards/                       # Board-specific implementations
│   ├── custom-G474/              # STM32G474 board
│   └── nucleo-U575ZI-Q/          # STM32U575 board
├── toolchains/                   # Cross-compilation toolchains
│   ├── cortex-m4f.cmake          # Cortex-M4F configuration
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
- **Tasks**: Battery monitoring and VESCAN motor controller tasks with HAL abstraction
- **RTT**: SEGGER Real-Time Transfer for debugging output
- **HAL Abstraction**: Platform-agnostic HAL function declarations
- **Weak Implementations**: Default HAL behaviors with platform overrides

## Development Environment Setup

This section covers the complete setup process for developing with this STM32 multi-board project. Follow these steps to establish a fully functional embedded development environment.

### Required Tools

#### Make
Make is a build automation tool that uses pattern-based rules to compile and link programs efficiently. It simplifies complex build processes by managing dependencies automatically and only rebuilding changed components. For this project, Make provides a unified frontend interface for building any STM32 board target with simple commands like `make debug nucleo-u575zi-q`.

#### Git Bash (Windows Users)
Git Bash provides a Unix-like command-line interface on Windows with integrated Git support and POSIX-compliant shell scripting. It includes essential Unix tools (grep, find, sed) that many build scripts and Makefiles expect to be available. This project's Makefile is designed to work seamlessly in Git Bash, WSL, or native Linux environments.

#### STM32CubeCLT (STM32Cube Command Line Tools)
STM32CubeCLT is STMicroelectronics' complete embedded toolchain including ARM GCC compiler, CMake, Ninja build generator, and OpenOCD debugger. It provides a unified, tested toolchain specifically optimized for STM32 development with consistent versions across Windows, Linux, and macOS. The project is configured to use STM32CubeCLT 1.18.0+ with hardcoded paths for reliable builds.

#### IDE Options: VSCode vs CLion
**VSCode** offers lightweight, extensible embedded development with excellent C++ IntelliSense and debugging through the cortex-debug extension with customizable JSON-based configuration and seamless integration with our Makefile frontend. **CLion** provides a full-featured C++ IDE with advanced refactoring, integrated CMake support, and sophisticated debugging capabilities.

#### OpenOCD (On-Chip Debugger)  
OpenOCD provides the critical bridge between your development environment and STM32 hardware through ST-Link, J-Link, or custom debug probes. It enables real-time debugging, flash programming, and hardware trace capabilities across all STM32 families. The tool is pre-integrated with STM32CubeCLT and configured for each board's specific debug interface and memory layout.

#### STM32CubeMX (Hardware Configuration)
STM32CubeMX is the graphical tool for configuring STM32 peripherals (GPIO, timers, communication interfaces) and generating initialization code. It produces `.ioc` project files that define hardware configurations and can regenerate board-specific code when hardware requirements change. This project tracks `.ioc` files in version control while ignoring the generated peripheral code to maintain clean repositories.

### Installation Guide

#### Windows Setup (Recommended Path)
```bash
# 1. Install STM32CubeCLT (includes everything except STM32CubeMX)
# Download from: https://www.st.com/en/development-tools/stm32cubeclt.html
# Install to default location: C:/ST/STM32CubeCLT_1.18.0/

# 2. Install STM32CubeMX separately (for hardware configuration)
# Download from: https://www.st.com/en/development-tools/stm32cubemx.html

# 3. Install Git for Windows (includes Git Bash)
# Download from: https://git-scm.com/download/win
# Ensure "Git Bash" option is selected during installation

# 4. Verify installation
git --version                    # Git available
make --version                   # Make available (from Git tools)
/c/ST/STM32CubeCLT_1.18.0/CMake/bin/cmake.exe --version    # CMake from STM32CubeCLT
```

#### Linux/WSL Setup
```bash
# 1. Download and install STM32CubeCLT for Linux
wget https://www.st.com/content/st_com/en/products/development-tools/software-development-tools/stm32-software-development-tools/stm32-configurators-and-code-generators/stm32cubeclt.html

# 2. Update Makefile paths to point to Linux installation
# Edit: CMAKE := /opt/st/stm32cubeclt/CMake/bin/cmake

# 3. Install native build tools
sudo apt update
sudo apt install build-essential cmake ninja-build

# 4. STM32CubeMX (optional, for hardware changes)
# Download and install STM32CubeMX .deb package
```

### Project Configuration Generation

This project uses a hybrid approach: track `.ioc` configuration files but ignore generated code for repository cleanliness.

#### Generating Board Configurations
```bash
# 1. Open board-specific .ioc file in STM32CubeMX
# Example: boards/nucleo-U575ZI-Q/nucleo-U575ZI-Q.ioc

# 2. Make hardware changes as needed:
#    - Configure GPIO pins, peripherals, clocks
#    - Set up FreeRTOS tasks and stack sizes
#    - Configure debug interfaces (SWD/JTAG)

# 3. Generate code (overwrites Core/ directories)
# STM32CubeMX → Project Manager → Generate Code

# 4. Build immediately to verify configuration
make debug nucleo-u575zi-q

# Note: Generated files are gitignored, only .ioc changes are tracked
```

#### CMake Configuration Workflow
```bash
# The build system automatically configures CMake for each project:

# Manual configuration (if needed):
cd boards/nucleo-U575ZI-Q
/c/ST/STM32CubeCLT_1.18.0/CMake/bin/cmake.exe -B cmake-build-debug \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_COMPILER=/c/ST/STM32CubeCLT_1.18.0/GNU-tools-for-STM32/bin/arm-none-eabi-gcc.exe \
    -DCMAKE_CXX_COMPILER=/c/ST/STM32CubeCLT_1.18.0/GNU-tools-for-STM32/bin/arm-none-eabi-g++.exe \
    -G Ninja

# Build target
cmake --build cmake-build-debug --target nucleo-U575ZI-Q -j 10
```

### Makefile Frontend Usage

The root-level Makefile provides a unified build interface for all projects, eliminating the need for complex IDE extensions.

#### Basic Command Structure
```bash
# Pattern: make [debug|release] [project-name]
make debug nucleo-u575zi-q      # Build STM32U575 board (debug)
make release nucleo-u575zi-q    # Build STM32U575 board (release)
make debug app                  # Build shared app library
make clean nucleo-u575zi-q      # Clean specific project
make clean-all                  # Clean all projects
make list                       # Show available projects
make help                       # Show usage information
```

#### Available Projects
- **app**: Shared application library (platform-agnostic)
- **nucleo-u575zi-q**: STM32U575ZI-Q Nucleo board
- **custom-g474**: Custom STM32G474 board

#### Build Outputs
```bash
# Debug builds output to:
boards/[board-name]/cmake-build-debug/[board-name].elf

# Release builds output to:
boards/[board-name]/cmake-build-release/[board-name].elf

# App library builds to:
app/build/libapp.a
```

### IDE/Editor Configuration

#### VSCode Setup (Recommended)
```bash
# 1. Install required extensions:
# - C/C++ (ms-vscode.cpptools)
# - C/C++ Extension Pack
# - Cortex-Debug (for STM32 debugging)

# 2. Open workspace file:
code STM32G474.code-workspace

# 3. Build using integrated tasks:
# Ctrl+Shift+P → "Tasks: Run Task" → "Build: Debug"
# Select project from dropdown menu

# 4. Features available:
# - IntelliSense via compile_commands.json
# - Problem matcher for build errors
# - Integrated terminal with make commands
# - Debugging support for STM32 boards
```

#### CLion Configuration
```bash
# 1. Open project root directory in CLion
# File → Open → Select STM32G474 directory

# 2. Configure toolchain:
# File → Settings → Build,Execution,Deployment → Toolchains
# Add new toolchain: "STM32CubeCLT"
# - C Compiler: C:/ST/STM32CubeCLT_1.18.0/GNU-tools-for-STM32/bin/arm-none-eabi-gcc.exe
# - C++ Compiler: C:/ST/STM32CubeCLT_1.18.0/GNU-tools-for-STM32/bin/arm-none-eabi-g++.exe
# - Debugger: C:/ST/STM32CubeCLT_1.18.0/GNU-tools-for-STM32/bin/arm-none-eabi-gdb.exe

# 3. CMake configuration:
# File → Settings → Build,Execution,Deployment → CMake
# Add profile for each board with appropriate -DBOARD= option

# 4. Use external Makefile frontend:
# External Tools → Add tool for "Build Debug"
# Program: make
# Arguments: debug $Prompt$
# Working directory: $ProjectFileDir$
```

#### Generic C++ IDE Setup
```bash
# Any IDE supporting C++ can work with this project:

# 1. Configure compiler paths to STM32CubeCLT:
# - C Compiler: [STM32CubeCLT]/GNU-tools-for-STM32/bin/arm-none-eabi-gcc
# - C++ Compiler: [STM32CubeCLT]/GNU-tools-for-STM32/bin/arm-none-eabi-g++
# - Debugger: [STM32CubeCLT]/GNU-tools-for-STM32/bin/arm-none-eabi-gdb

# 2. Configure IntelliSense using compile_commands.json:
# Point IDE to: boards/[board-name]/cmake-build-debug/compile_commands.json

# 3. Set up build commands:
# Build: make debug [project-name]
# Clean: make clean [project-name]

# 4. Configure include paths (if needed):
# - boards/[board-name]/Core/Inc
# - app/Inc
# - STM32CubeCLT HAL driver includes (auto-detected via compile_commands.json)
```

### Verification and Testing

#### Build System Verification
```bash
# Test each component:
make help                       # Should show usage information
make list                       # Should show available projects
make debug app                  # Should build shared library
make debug nucleo-u575zi-q      # Should build STM32U575 firmware
make clean-all                  # Should clean all build directories

# Verify outputs exist:
ls app/build/                   # Should contain libapp.a
ls boards/nucleo-U575ZI-Q/cmake-build-debug/   # Should contain .elf file
```

#### IDE Integration Testing
```bash
# VSCode:
# - Open STM32G474.code-workspace
# - Ctrl+Shift+P → "Tasks: Run Task" → should show build tasks
# - Build task should invoke make commands successfully
# - IntelliSense should provide code completion

# CLion:
# - Open project, verify toolchain detection
# - Build configurations should appear for each board
# - External make tools should execute without errors
```

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