# IDE Configuration for Custom STM32G474 Board Project  
# This file contains metadata used to generate IDE configuration files
# for the custom-G474 board project.

# Project identification
set(PROJECT_NAME "STM32G474")
set(PROJECT_TYPE "EXECUTABLE")

# MCU specifications
set(MCU_NAME "STM32G474RETx")
set(MCU_FAMILY "STM32G4")
set(MCU_CORE "thumb")

# STM32CubeIDE specific MCU settings
set(FPU_TYPE "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.fpu.value.fpv4-sp-d16")
set(FLOAT_ABI "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.floatabi.value.hard")

# Linker script
set(LINKER_SCRIPT "STM32G474XX_FLASH.ld")

# Source and include directories
set(SOURCE_DIRS "Core/Src" "Core/Inc")
set(INCLUDE_DIRS 
    "Core/Inc"
    "Drivers/STM32G4xx_HAL_Driver/Inc"
    "Drivers/STM32G4xx_HAL_Driver/Inc/Legacy"
    "Drivers/CMSIS/Device/ST/STM32G4xx/Include"
    "Drivers/CMSIS/Include"
    "Middlewares/Third_Party/FreeRTOS/Source/include"
    "Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2"
    "Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F"
)

# HAL preprocessor defines
set(HAL_DEFINES "USE_HAL_DRIVER" "STM32G474xx")