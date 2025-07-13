# ARM Cortex-M7 toolchain (STM32H7, STM32F7 series)
include(${CMAKE_CURRENT_LIST_DIR}/arm-gcc-base.cmake)

# Cortex-M7 specific flags
set(TARGET_FLAGS "-mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard")

# Apply target flags
set(CMAKE_C_FLAGS "${COMMON_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TARGET_FLAGS}")

# Add target flags to link flags
set(CMAKE_C_LINK_FLAGS "${TARGET_FLAGS} ${CMAKE_C_LINK_FLAGS}")
set(CMAKE_CXX_LINK_FLAGS "${TARGET_FLAGS} ${CMAKE_CXX_LINK_FLAGS}")

# Set variant identifier
set(MCU_VARIANT "cortex-m7" CACHE STRING "MCU variant")