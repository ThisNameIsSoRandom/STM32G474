# RTT library target
add_library(RTT STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/SEGGER_RTT.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/SEGGER_RTT_ASM_ARMv7M.S
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/SEGGER_RTT_printf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/SEGGER_RTT_Syscalls_GCC.c
)

# Set include directories as PUBLIC so they propagate to consumers
target_include_directories(RTT PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

# RTT will inherit compile options from parent project

# Set properties for better integration
set_target_properties(RTT PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS YES
)