cmake_minimum_required(VERSION 3.22)

# Select board (default to custom-G474)
if(NOT DEFINED BOARD)
    #set(BOARD "custom-G474" CACHE STRING "Target board name")
endif()

set(BOARD "nucleo-H755ZI-Q")
# Include board-specific toolchain file
if(BOARD STREQUAL "custom-G474")
    include(boards/custom-G474/cmake/gcc-arm-none-eabi.cmake)
elseif(BOARD STREQUAL "nucleo-H755ZI-Q")
    include(boards/nucleo-H755ZI-Q/cmake/gcc-arm-none-eabi.cmake)
else()
    message(FATAL_ERROR "Unknown board: ${BOARD}")
endif()

# Define the project - toolchain file should be passed via -DCMAKE_TOOLCHAIN_FILE
project(STM32MultiBoard)

# Build board executables (each board sets its own toolchain and builds app)
add_subdirectory(boards/${BOARD})