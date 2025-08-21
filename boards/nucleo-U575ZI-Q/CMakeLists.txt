cmake_minimum_required(VERSION 3.22)

#[[
================================================================================
                        STM32U575ZI-Q Nucleo Board Project
================================================================================

OVERVIEW:
This is the board-specific CMakeLists.txt for the STM32U575ZI-Q Nucleo board.
It configures and builds a complete embedded application that combines the
shared app library with STM32U575 HAL drivers and FreeRTOS real-time OS.

TARGET SPECIFICATIONS:
- MCU: STM32U575ZITxQ (ARM Cortex-M33 with TrustZone)
- Core: ARM Cortex-M33 @ 160 MHz
- Flash: 2048 KB
- RAM: 786 KB
- FPU: Single-precision floating-point unit
- Security: ARM TrustZone support (non-secure world)
- Board: NUCLEO-U575ZI-Q development board

ARCHITECTURE:
The build integrates several components:
├── Core/                   # STM32CubeMX generated HAL code
│   ├── Inc/               # MCU-specific headers and configuration
│   └── Src/               # Main application entry point and HAL init
├── Drivers/               # STM32U5 HAL/LL drivers and CMSIS
├── Middlewares/           # FreeRTOS kernel and CMSIS-RTOS wrapper
├── cmake/                 # Board-specific build configuration
│   ├── gcc-arm-none-eabi.cmake    # ARM GCC toolchain setup
│   ├── freertos.cmake             # FreeRTOS configuration
│   └── stm32cubemx/              # STM32CubeMX generated build rules
└── ../../app/            # Shared application library (linked)

BUILD FEATURES:
- ARM GCC cross-compilation toolchain
- C11 and C++20 standard support
- FreeRTOS v10.x real-time operating system
- STM32U5 HAL/LL driver libraries
- CMSIS-RTOS v2 API wrapper
- Debug symbol generation
- Compile commands export for IDE IntelliSense
- Linker script for STM32U575xx memory layout

MEMORY CONFIGURATION:
- Flash: 0x08000000 - 0x081FFFFF (2048 KB)
- RAM:   0x20000000 - 0x200C3FFF (786 KB)
- Stack: Configured for FreeRTOS task stacks
- Heap:  FreeRTOS heap_4 memory management

APPLICATION INTEGRATION:
Links with the shared 'app' library to provide:
- BQ40Z80 battery management system driver
- Real-time task scheduling and management
- SEGGER RTT logging and debugging
- Unit test framework integration (optional)

FREERTOS CONFIGURATION:
- Kernel: FreeRTOS with CMSIS-RTOS v2 wrapper
- Port: ARM Cortex-M33 non-secure (GCC)
- Heap: heap_4 algorithm with thread-safe allocation
- Tick: SysTick timer at 1000 Hz
- Priority: 16 priority levels (0 = lowest, 15 = highest)

DEBUGGING:
- Support for STM32CubeProgrammer and ST-Link
- SEGGER J-Link compatible
- SWD interface debugging
- Real-time variable watching via RTT
- Eclipse CDT and VSCode debug configurations

BUILD COMMANDS:
- Configure: cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
- Build: cmake --build cmake-build-debug --target nucleo-U575ZI-Q
- Flash: (Use STM32CubeProgrammer or debugger)

PREREQUISITES:
- STM32CubeCLT 1.18.0+ (CMake, ARM GCC, OpenOCD)
- Git (for FreeRTOS dependency fetching)
- STM32CubeMX (for hardware configuration updates)

================================================================================
]]

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME nucleo-U575ZI-Q)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM, C and C++ languages
enable_language(C CXX ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Disable unit tests for this platform (mocking removed)
set(ENABLE_UNIT_TESTS OFF CACHE BOOL "Enable unit tests" FORCE)

# Pass platform-specific definitions to app library
add_compile_definitions(STM32U575xx USE_HAL_DRIVER)

# Include app from parent directory (builds automatically)
add_subdirectory(../../app ${CMAKE_BINARY_DIR}/app)

# Generate IDE configuration files (only if not called from root-level IDE generator)
if(NOT IDE_CONFIG_GENERATION_ONLY)
    # Generate STM32CubeIDE project files
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32cubeide.cmake)
    generate_stm32cubeide_project(
        PROJECT_NAME "${CMAKE_PROJECT_NAME}"
        MCU_NAME "STM32U575ZITxQ"
        MCU_FAMILY "STM32U5"
        MCU_CORE "thumb"
        FPU_TYPE "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.fpu.value.fpv5-sp-d16"
        FLOAT_ABI "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.floatabi.value.hard"
        LINKER_SCRIPT "STM32U575xx_FLASH.ld"
        INCLUDE_DIRS "Core/Inc" "Drivers/STM32U5xx_HAL_Driver/Inc" "Drivers/STM32U5xx_HAL_Driver/Inc/Legacy" "Drivers/CMSIS/Device/ST/STM32U5xx/Include" "Drivers/CMSIS/Include"
        SOURCE_DIRS "Core/Src" "Core/Inc"
        HAL_DEFINES "USE_HAL_DRIVER" "STM32U575xx"
    )

    # Generate VSCode project files
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/vscode.cmake)
    generate_vscode_project(
        PROJECT_NAME "${CMAKE_PROJECT_NAME}"
        PROJECT_TYPE "EXECUTABLE"
        MCU_NAME "STM32U575ZITxQ"
        MCU_FAMILY "STM32U5"
        MCU_CORE "thumb"
        LINKER_SCRIPT "STM32U575xx_FLASH.ld"
        INCLUDE_DIRS "Core/Inc" "Drivers/STM32U5xx_HAL_Driver/Inc" "Drivers/STM32U5xx_HAL_Driver/Inc/Legacy" "Drivers/CMSIS/Device/ST/STM32U5xx/Include" "Drivers/CMSIS/Include"
        SOURCE_DIRS "Core/Src" "Core/Inc"
        HAL_DEFINES "USE_HAL_DRIVER" "STM32U575xx"
    )
endif()

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Add FreeRTOS
include(cmake/freertos.cmake)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
    
    # Enable unit tests for this platform
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    freertos_kernel

    # Add user defined libraries
)

# Link with app target using linker groups to resolve weak symbols
if(TARGET app)
    target_link_libraries(${CMAKE_PROJECT_NAME} app)
    message(STATUS "Linking with app target using linker groups")
    
    # Link with test library if it exists
    if(TARGET app_tests)
        # Use function wrapping to override HAL functions with mocks
        target_link_libraries(${CMAKE_PROJECT_NAME} 
            -Wl,--whole-archive app_tests -Wl,--no-whole-archive
            -Wl,--wrap=HAL_I2C_Master_Transmit
            -Wl,--wrap=HAL_I2C_Master_Receive
            -Wl,--wrap=HAL_I2C_Mem_Write
            -Wl,--wrap=HAL_I2C_Mem_Read)
        message(STATUS "Linking with app_tests target using HAL function wrapping")
    endif()
else()
    message(STATUS "app target not found, building without app")
endif()

# Force only main.c to be compiled as C++ while keeping .c extension
set_source_files_properties(
    Core/Src/main.c
    PROPERTIES LANGUAGE CXX
)

# Set output directory for easier debugging with CLion/J-Link
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/build"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/build"
)
