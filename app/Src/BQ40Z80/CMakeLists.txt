# BQ40Z80 Battery Gauge Driver Library
cmake_minimum_required(VERSION 3.16)

# Set system information for cross-compilation
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Define BQ40Z80 library
add_library(BQ40Z80 STATIC
    Src/bq40z80.cpp
    Src/bq40z80_lowlevel.cpp
)

# Include directories
target_include_directories(BQ40Z80 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Inc
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../Inc  # app/Inc directory
)

# Link with parent app includes for HAL types
target_link_libraries(BQ40Z80 
    PRIVATE
        RTT  # For logging unimplemented functions
)

# Compile options
target_compile_options(BQ40Z80 PRIVATE
    -fno-exceptions
    -fno-rtti
    -fno-threadsafe-statics
)

# Export library for tasks
set_target_properties(BQ40Z80 PROPERTIES
    POSITION_INDEPENDENT_CODE OFF
)