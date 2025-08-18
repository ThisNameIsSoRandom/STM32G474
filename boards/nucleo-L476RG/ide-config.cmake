# IDE Configuration for STM32L476RG Nucleo Board Project  
# This file contains metadata used to generate IDE configuration files
# for the nucleo-L476RG board project.

# Project identification  
set(PROJECT_NAME "nucleo-L476RG")
set(PROJECT_TYPE "EXECUTABLE")

# MCU specifications
set(MCU_NAME "STM32L476RGTx")
set(MCU_FAMILY "STM32L4")
set(MCU_CORE "thumb")

# STM32CubeIDE specific MCU settings
set(FPU_TYPE "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.fpu.value.fpv4-sp-d16")
set(FLOAT_ABI "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.floatabi.value.hard")

# Linker script
set(LINKER_SCRIPT "STM32L476XX_FLASH.ld")

# Source and include directories
set(SOURCE_DIRS "Core/Src" "Core/Inc")
set(INCLUDE_DIRS 
    "Core/Inc"
    "Drivers/STM32L4xx_HAL_Driver/Inc"
    "Drivers/STM32L4xx_HAL_Driver/Inc/Legacy"
    "Drivers/CMSIS/Device/ST/STM32L4xx/Include"
    "Drivers/CMSIS/Include"
    "Middlewares/Third_Party/FreeRTOS/Source/include"
    "Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2"
    "Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F"
)

# HAL preprocessor defines
set(HAL_DEFINES "USE_HAL_DRIVER" "STM32L476xx")