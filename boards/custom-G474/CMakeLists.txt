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

# Setup C++ compiler settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME STM32G474)

# Setup toolchain for standalone build
set(STM32_MCU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")
set(STM32_LINKER_SCRIPT "STM32G474XX_FLASH.ld")
set(STM32_LINKER_OPTION "")

# Include toolchain file
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/gcc-arm-none-eabi.cmake)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM, C, and C++ languages
enable_language(C CXX ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Include app from parent directory (builds automatically)
add_subdirectory(../../app ${CMAKE_BINARY_DIR}/app)

# Generate IDE configuration files (only if not called from root-level IDE generator)
if(NOT IDE_CONFIG_GENERATION_ONLY)
    # Generate STM32CubeIDE project files
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/stm32cubeide.cmake)
    generate_stm32cubeide_project(
        PROJECT_NAME "${CMAKE_PROJECT_NAME}"
        MCU_NAME "STM32G474RETx"
        MCU_FAMILY "STM32G4"
        MCU_CORE "thumb"
        FPU_TYPE "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.fpu.value.fpv4-sp-d16"
        FLOAT_ABI "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.floatabi.value.hard"
        LINKER_SCRIPT "STM32G474XX_FLASH.ld"
        INCLUDE_DIRS "Core/Inc" "Drivers/STM32G4xx_HAL_Driver/Inc" "Drivers/STM32G4xx_HAL_Driver/Inc/Legacy" "Drivers/CMSIS/Device/ST/STM32G4xx/Include" "Drivers/CMSIS/Include" "Middlewares/Third_Party/FreeRTOS/Source/include" "Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2" "Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F"
        SOURCE_DIRS "Core/Src" "Core/Inc"
        HAL_DEFINES "USE_HAL_DRIVER" "STM32G474xx"
    )

    # Generate VSCode project files
    include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/vscode.cmake)
    generate_vscode_project(
        PROJECT_NAME "${CMAKE_PROJECT_NAME}"
        PROJECT_TYPE "EXECUTABLE"
        MCU_NAME "STM32G474RETx"
        MCU_FAMILY "STM32G4"
        MCU_CORE "thumb"
        LINKER_SCRIPT "STM32G474XX_FLASH.ld"
        INCLUDE_DIRS "Core/Inc" "Drivers/STM32G4xx_HAL_Driver/Inc" "Drivers/STM32G4xx_HAL_Driver/Inc/Legacy" "Drivers/CMSIS/Device/ST/STM32G4xx/Include" "Drivers/CMSIS/Include" "Middlewares/Third_Party/FreeRTOS/Source/include" "Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2" "Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F"
        SOURCE_DIRS "Core/Src" "Core/Inc"
        HAL_DEFINES "USE_HAL_DRIVER" "STM32G474xx"
    )
endif()

# Add STM32CubeMX generated sources first
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

# Force main.c to be compiled as C++
set_source_files_properties(Core/Src/main.c PROPERTIES LANGUAGE CXX)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
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
else()
    message(STATUS "app target not found, building without app")
endif()



