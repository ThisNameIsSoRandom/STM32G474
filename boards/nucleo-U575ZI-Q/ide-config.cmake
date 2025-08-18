# IDE Configuration for STM32U575ZI-Q Nucleo Board Project  
# This file contains metadata used to generate IDE configuration files
# for the nucleo-U575ZI-Q board project.

# Project identification
set(PROJECT_NAME "nucleo-U575ZI-Q")
set(PROJECT_TYPE "EXECUTABLE")

# MCU specifications
set(MCU_NAME "STM32U575ZITxQ")
set(MCU_FAMILY "STM32U5")
set(MCU_CORE "thumb")

# STM32CubeIDE specific MCU settings
set(FPU_TYPE "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.fpu.value.fpv5-sp-d16")
set(FLOAT_ABI "com.st.stm32cube.ide.mcu.gnu.managedbuild.option.floatabi.value.hard")

# Linker script
set(LINKER_SCRIPT "STM32U575xx_FLASH.ld")

# Source and include directories
set(SOURCE_DIRS "Core/Src" "Core/Inc")
set(INCLUDE_DIRS 
    "Core/Inc"
    "Drivers/STM32U5xx_HAL_Driver/Inc"
    "Drivers/STM32U5xx_HAL_Driver/Inc/Legacy"
    "Drivers/CMSIS/Device/ST/STM32U5xx/Include"
    "Drivers/CMSIS/Include"
    "Middlewares/Third_Party/FreeRTOS/Source/include"
    "Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2"
    "Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure"
)

# HAL preprocessor defines
set(HAL_DEFINES "USE_HAL_DRIVER" "STM32U575xx")