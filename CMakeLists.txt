cmake_minimum_required(VERSION 3.22)

#[[
================================================================================
                        STM32G474 Multi-Project IDE Configuration Generator
================================================================================

OVERVIEW:
This is the root-level CMakeLists.txt for the STM32G474 multi-board embedded 
project. It serves as an IDE configuration generator that creates project files
for various development environments without performing actual compilation.

PURPOSE:
- Generates VSCode configuration files (.vscode/c_cpp_properties.json, etc.)
- Generates STM32CubeIDE project files (.project, .cproject)
- Provides unified workspace configuration for multiple STM32 board targets
- Enables IntelliSense and code navigation across all board projects

SUPPORTED BOARDS:
- STM32G474 (custom-G474): Single-core ARM Cortex-M4F with FreeRTOS
- STM32H755ZI-Q (nucleo-H755ZI-Q): Dual-core ARM Cortex-M7 + M4 with FreeRTOS
- STM32U575ZI-Q (nucleo-U575ZI-Q): Single-core ARM Cortex-M33 with FreeRTOS
- STM32L476RG (nucleo-L476RG): Single-core ARM Cortex-M4F with FreeRTOS

ARCHITECTURE:
The project uses a multi-level structure:
├── app/                    # Shared application library (board-agnostic)
├── boards/                 # Board-specific implementations
│   ├── custom-G474/       # STM32G474 custom board
│   ├── nucleo-H755ZI-Q/   # STM32H755 dual-core Nucleo board
│   ├── nucleo-U575ZI-Q/   # STM32U575 Nucleo board
│   └── nucleo-L476RG/     # STM32L476 Nucleo board
├── toolchains/            # Cross-compilation toolchain definitions
└── cmake/                 # CMake utilities and IDE generators

BUILD APPROACH:
This root CMakeLists.txt does NOT build anything. Each board has its own
independent CMakeLists.txt that can be built standalone. This approach:
- Avoids complex multi-target configurations
- Enables independent board development
- Simplifies debugging and testing per board
- Provides clean separation of concerns

USAGE:
1. For IDE configuration: cmake -B build (from project root)
2. For building specific board: cd boards/nucleo-U575ZI-Q && cmake -B build && cmake --build build
3. For Makefile frontend: make debug nucleo-u575zi-q

DEPENDENCIES:
- CMake 3.22 or higher
- STM32CubeCLT toolchain (for actual builds)
- Git (for FreeRTOS dependency fetching)

INTEGRATION:
Works with minimal VSCode setup that avoids CMake Tools extension complexity.
Uses compile_commands.json from individual board builds for IntelliSense.

================================================================================
]]

project(STM32G474_IDE_Config 
    LANGUAGES NONE
    DESCRIPTION "IDE Configuration Generator for STM32G474 Multi-Project Workspace"
    VERSION 1.0.0
)

# Set flag to indicate this is IDE config generation only
# Individual project CMakeLists.txt can check this to skip build configuration
set(IDE_CONFIG_GENERATION_ONLY ON CACHE BOOL "IDE configuration generation mode" FORCE)

message(STATUS "=== STM32G474 IDE Configuration Generator ===")
message(STATUS "Generating IDE configuration files for all projects...")
message(STATUS "No actual building will occur - this is configuration-only")

# Include core IDE generation logic
include(ide-config.cmake)

# Auto-discover all projects and generate their IDE configurations
discover_and_generate_ide_configs()

message(STATUS "IDE configuration generation completed successfully!")
message(STATUS "Individual projects can now be built independently using their own CMakeLists.txt")