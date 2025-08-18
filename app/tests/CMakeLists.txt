# Google Test integration for embedded STM32 unit testing
# 
# This CMake configuration sets up Google Test for on-target testing
# with STM32 microcontrollers. Tests run as a FreeRTOS task and output
# results via SEGGER RTT.
# 
# Architecture:
# - Tests compile as a static library linked into the main application
# - GTest is configured for embedded use (no exceptions, no RTTI)
# - Custom test listener redirects output to SEGGER RTT
# - Tests can be enabled/disabled via ENABLE_UNIT_TESTS option

cmake_minimum_required(VERSION 3.16)

# Set system information for cross-compilation
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(app_tests CXX C)

# Option to enable/disable unit tests
option(ENABLE_UNIT_TESTS "Build unit tests with Google Test" ON)

if(NOT ENABLE_UNIT_TESTS)
    message(STATUS "Unit tests disabled")
    return()
endif()

# Create test library (static library for embedded target)
add_library(${PROJECT_NAME} STATIC)

# Configure Google Test for embedded systems
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

# Embedded-specific GTest configuration
add_compile_definitions(
    GTEST_HAS_PTHREAD=0      # No POSIX threads
    GTEST_HAS_EXCEPTIONS=0   # No C++ exceptions
    GTEST_HAS_RTTI=0         # No RTTI
    GTEST_HAS_STD_WSTRING=0  # No wide strings
    GTEST_HAS_STREAM_REDIRECTION=0  # No stream redirection
    GTEST_HAS_DEATH_TEST=0   # No death tests on embedded
)

# Add Google Test as subdirectory (will be added as git submodule)
# For now, we'll use a minimal custom implementation
# add_subdirectory(googletest)

# Add test sources
target_sources(${PROJECT_NAME} PRIVATE
    # Test runner and minimal GTest implementation
    runner/test_runner_task.cpp
    runner/gtest_minimal.cpp
    
    # RTT listener for test output
    listeners/rtt_test_listener.cpp
    
    # Unit tests
    unit/test_bq40z80_driver.cpp
)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/runner
    ${CMAKE_CURRENT_SOURCE_DIR}/listeners
    ${CMAKE_CURRENT_SOURCE_DIR}/fixtures
)

# Link with app library for testing
target_link_libraries(${PROJECT_NAME} PUBLIC
    app  # Main app library with BQ40Z80, Tasks, RTT
)

# For now, use a minimal gtest implementation until we add the full library
# target_link_libraries(${PROJECT_NAME} PUBLIC
#     gtest
#     gtest_main
# )

# Set C++ standard for embedded
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

# Compile options for embedded target
target_compile_options(${PROJECT_NAME} PRIVATE
    -fno-exceptions
    -fno-rtti
    -fno-threadsafe-statics
    -ffunction-sections
    -fdata-sections
)

# Export test runner function for platform integration
target_compile_definitions(${PROJECT_NAME} PUBLIC
    ENABLE_UNIT_TESTS=1
)