cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

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

# Enable unit tests for this platform (must be before app subdirectory)
set(ENABLE_UNIT_TESTS ON CACHE BOOL "Enable unit tests" FORCE)

# Include parent project to access app target
add_subdirectory(../../ ${CMAKE_BINARY_DIR}/STM32G474)

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
    ENABLE_UNIT_TESTS=1
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
