# ARM Cortex-M0+ toolchain (STM32L0, STM32G0 series)
include(${CMAKE_CURRENT_LIST_DIR}/arm-gcc-base.cmake)

# Cortex-M0+ specific flags (no FPU)
set(TARGET_FLAGS "-mcpu=cortex-m0plus -mthumb")

# Apply target flags
set(CMAKE_C_FLAGS "${COMMON_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TARGET_FLAGS}")

# Add target flags to link flags
set(CMAKE_C_LINK_FLAGS "${TARGET_FLAGS} ${CMAKE_C_LINK_FLAGS}")
set(CMAKE_CXX_LINK_FLAGS "${TARGET_FLAGS} ${CMAKE_CXX_LINK_FLAGS}")

# Set variant identifier
set(MCU_VARIANT "cortex-m0plus" CACHE STRING "MCU variant")