# FreeRTOS Configuration
cmake_minimum_required(VERSION 3.22)

# Create FreeRTOS config target FIRST before fetching FreeRTOS
add_library(freertos_config INTERFACE)

# FreeRTOS include directories
target_include_directories(freertos_config SYSTEM INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
)

# FreeRTOS compile definitions
target_compile_definitions(freertos_config INTERFACE
    projCOVERAGE_TEST=0
)

# FreeRTOS port configuration for ARM Cortex-M33
set(FREERTOS_PORT GCC_ARM_CM33_NTZ_NONSECURE CACHE STRING "FreeRTOS port")

include(FetchContent)

# Download FreeRTOS
FetchContent_Declare(
    freertos_kernel
    GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
    GIT_TAG        V10.6.2
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(freertos_kernel)