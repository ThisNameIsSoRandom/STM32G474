cmake_minimum_required(VERSION 3.16)

#[[
================================================================================
                           STM32G474 Application Library
================================================================================

OVERVIEW:
This is the shared application library for the STM32G474 multi-board project.
It provides board-agnostic application code that can be linked into any of the
supported STM32 board targets (custom-G474, nucleo-U575ZI-Q).

PURPOSE:
- Shared application logic across multiple STM32 board implementations
- Battery management system (BQ40Z80) driver and communication
- Real-time task management and scheduling
- VESCAN motor controller CAN communication
- Logging and debugging utilities via SEGGER RTT

ARCHITECTURE:
The app library is organized into modular components:
├── Inc/                    # Public header files (library interface)
├── Src/                    # Implementation files
│   ├── Inc/               # Private header files
│   ├── RTT/               # SEGGER Real-Time Transfer (logging/debug)
│   ├── Tasks/             # FreeRTOS task implementations
│   ├── BQ40Z80/           # Battery management IC driver
│   ├── VESCCan/           # VESC motor controller CAN communication
│   ├── logging.cpp        # Logging infrastructure
│   └── placeholder.cpp    # Template implementation

BUILD MODES:
1. Standalone Build: Uses system compiler for library development/testing
2. Cross-Compilation: Uses ARM GCC when included by board projects

LIBRARY FEATURES:
- Static library target for linking into board executables
- C11 and C++17 standard compliance
- Export configuration for use by board projects
- Conditional compilation for different target architectures
- CMake target export for find_package() compatibility

DEPENDENCIES:
- SEGGER RTT (for logging and debug output)
- FreeRTOS (task management - provided by board projects)
- VESCCan (VESC motor controller CAN communication)

INTEGRATION:
Each board project includes this library via add_subdirectory() and links
against the 'app' target. The library adapts to the board's toolchain and
provides consistent application functionality across all platforms.


USAGE:
- Include in board project: add_subdirectory(../../app ${CMAKE_BINARY_DIR}/app)
- Link to board executable: target_link_libraries(board_target app)
- Standalone build: cmake -B build && cmake --build build

================================================================================
]]

# Set system information for cross-compilation when building standalone
if(NOT CMAKE_SYSTEM_NAME)
    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_SYSTEM_PROCESSOR arm)
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif()

# For standalone builds, use system compilers if no ARM toolchain is set
if(NOT CMAKE_C_COMPILER AND NOT CMAKE_CXX_COMPILER)
    unset(CMAKE_SYSTEM_NAME CACHE)
    unset(CMAKE_SYSTEM_PROCESSOR CACHE)
    unset(CMAKE_TRY_COMPILE_TARGET_TYPE CACHE)
endif()

project(app
        VERSION 0.0.0
        LANGUAGES C CXX
        DESCRIPTION "Application library with canonical structure"
)

# Enable compile commands export for IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Create main app static library
add_library(${PROJECT_NAME} STATIC)

# Add main app source files
target_sources(${PROJECT_NAME} PRIVATE
    Src/placeholder.cpp
    Src/logging.cpp
)

# Public includes (visible to users of the library)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

# Private includes (only visible within the library)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Inc
)

# Add subdirectories for RTT, Tasks, BQ40Z80, and VESCCan
add_subdirectory(Src/RTT)
add_subdirectory(Src/Tasks)
add_subdirectory(Src/BQ40Z80)
add_subdirectory(Src/VESCCan)


# Link with subdirectory libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
    RTT
    Tasks
    BQ40Z80
    VESCCan
)


# Inherit compile options from parent project
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<COMPILE_LANGUAGE:C>:${STM32CUBEMX_C_FLAGS}>
    $<$<COMPILE_LANGUAGE:CXX>:${STM32CUBEMX_CXX_FLAGS}>
    $<$<COMPILE_LANGUAGE:ASM>:${STM32CUBEMX_ASM_FLAGS}>
)

# Set properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS YES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

# C++ files are now native .cpp - no forced compilation needed

# Create alias for find_package compatibility
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Export targets to build directory for immediate use
# Only export core libraries, not test library (to avoid circular dependency)
export(TARGETS ${PROJECT_NAME} RTT Tasks BQ40Z80 VESCCan
    NAMESPACE ${PROJECT_NAME}::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
)

# Create a simple config file in build directory
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}Targets.cmake\")
set(${PROJECT_NAME}_FOUND TRUE)
")

# Generate IDE configuration files (only if not called from root-level IDE generator)
if(NOT IDE_CONFIG_GENERATION_ONLY)
    # Generate STM32CubeIDE project files for app library
    include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/stm32cubeide.cmake)
    generate_stm32cubeide_project(
        PROJECT_NAME "${PROJECT_NAME}"
        PROJECT_TYPE "LIBRARY"
        SOURCE_DIRS Inc Src
        INCLUDE_DIRS Inc Src/RTT/Inc Src/Tasks/Inc Src/BQ40Z80/Inc Src/VESCCan/Inc
    )

    # Generate VSCode project files for app library
    include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/vscode.cmake)
    generate_vscode_project(
        PROJECT_NAME "${PROJECT_NAME}"
        PROJECT_TYPE "LIBRARY"
        SOURCE_DIRS Inc Src
        INCLUDE_DIRS Inc Src/RTT/Inc Src/Tasks/Inc Src/BQ40Z80/Inc Src/VESCCan/Inc
    )
endif()