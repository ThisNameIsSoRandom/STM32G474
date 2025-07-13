# ARM Cortex-M33 toolchain (STM32U5 series)
include(${CMAKE_CURRENT_LIST_DIR}/arm-gcc-base.cmake)

# Cortex-M33 specific flags
set(TARGET_FLAGS "-mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard")

# Apply target flags
set(CMAKE_C_FLAGS "${COMMON_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TARGET_FLAGS}")

# Add target flags to link flags
set(CMAKE_C_LINK_FLAGS "${TARGET_FLAGS} ${CMAKE_C_LINK_FLAGS}")
set(CMAKE_CXX_LINK_FLAGS "${TARGET_FLAGS} ${CMAKE_CXX_LINK_FLAGS}")

# Set variant identifier
set(MCU_VARIANT "cortex-m33" CACHE STRING "MCU variant")