# Unit Tests for App Library - Embedded Testing
cmake_minimum_required(VERSION 3.16)

# This will be cross-compiled for ARM MCU, not host testing
# Set system information for cross-compilation
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(app_uTests CXX)

# Create test library (not executable - will be linked into firmware)
add_library(${PROJECT_NAME} STATIC)

# Add test source files
target_sources(${PROJECT_NAME} PRIVATE
    test_runner.cpp
    test_bq40z80_basic.cpp
    test_bq40z80_advanced.cpp
)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Inc
)

# Don't link with or build dependencies here - they will be provided by the main app target
# The uTBoards build will link this with the main app target which includes all dependencies

# Set C++ standard for embedded
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

# Compile options for embedded
target_compile_options(${PROJECT_NAME} PRIVATE
    -fno-exceptions
    -fno-rtti
    -fno-threadsafe-statics
)