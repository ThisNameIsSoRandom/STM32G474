cmake_minimum_required(VERSION 3.16)

# Set system information for cross-compilation
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(Tasks)

# Create Tasks static library
add_library(${PROJECT_NAME} STATIC)

# Add task source files
target_sources(${PROJECT_NAME} PRIVATE
    smbus_task.cpp
    uart_task.cpp
    battery_monitor_task.cpp
    hal_implementations.cpp
)

# Include directories for tasks
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Inc
)

# Add header files
target_sources(${PROJECT_NAME} PUBLIC
        Inc/freertos_tasks.h
        Inc/battery_monitor_task.h
)

# No wydmuszka linking needed - platform provides HAL implementations

# Tasks need RTT and BQ40Z80 library
if(TARGET RTT)
    target_link_libraries(${PROJECT_NAME} PUBLIC RTT)
endif()

if(TARGET BQ40Z80)
    target_link_libraries(${PROJECT_NAME} PUBLIC BQ40Z80)
endif()

# C++ files are now native .cpp - no forced compilation needed