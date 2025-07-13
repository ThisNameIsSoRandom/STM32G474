cmake_minimum_required(VERSION 3.22)

project(Tasks)

# Create Tasks static library
add_library(${PROJECT_NAME} STATIC)

# Add task source files
target_sources(${PROJECT_NAME} PRIVATE
    smbus_task.cpp
    uart_task.cpp
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
)

# No wydmuszka linking needed - platform provides HAL implementations

# Tasks need RTT (FreeRTOS provided by platform)
if(TARGET RTT)
    target_link_libraries(${PROJECT_NAME} PUBLIC RTT)
endif()

# C++ files are now native .cpp - no forced compilation needed