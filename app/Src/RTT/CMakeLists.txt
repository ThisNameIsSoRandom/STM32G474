#[[
================================================================================
                        SEGGER Real-Time Transfer (RTT) Library
================================================================================

OVERVIEW:
This library provides SEGGER RTT (Real-Time Transfer) functionality for high-
speed, real-time communication between the embedded target and a debug host
without halting the target or requiring additional hardware pins.

PURPOSE:
- Real-time debug output and logging from embedded applications
- Printf-style formatted output to debug host
- Bi-directional communication channel for debugging
- Low-overhead alternative to UART/SWO for debug messages
- Integration with SEGGER J-Link and other RTT-compatible tools

FEATURES:
- Multiple RTT channels (up and down buffers)
- Thread-safe operation in multi-threaded environments
- Minimal RAM footprint and CPU overhead
- No additional hardware pins required (uses debug interface)
- Support for printf-style formatted output
- GCC syscall integration for standard I/O redirection

INTEGRATION:
RTT output can be viewed using:
- SEGGER J-Link RTT Viewer
- SEGGER Ozone debugger
- OpenOCD with RTT support
- pyOCD with RTT plugin
- Any RTT-compatible debug tool

USAGE IN APPLICATION:
```c
#include "SEGGER_RTT.h"

// Simple output
SEGGER_RTT_WriteString(0, "Hello RTT!\n");

// Printf-style output
SEGGER_RTT_printf(0, "Value: %d\n", 42);

// Using as printf replacement (with syscalls)
printf("This goes to RTT\n");
```

FILES:
- SEGGER_RTT.c: Core RTT implementation
- SEGGER_RTT_ASM_ARMv7M.S: Assembly optimizations for ARM Cortex-M
- SEGGER_RTT_printf.c: Printf implementation for RTT
- SEGGER_RTT_Syscalls_GCC.c: GCC syscall redirection to RTT

MEMORY USAGE:
- Configurable buffer sizes (default: 1KB up, 32B down)
- Statically allocated buffers in RAM
- Minimal code size footprint (~2-4KB)

PERFORMANCE:
- Transfer speeds up to several MB/s (depending on debug interface)
- Microsecond-level latency for debug output
- Non-blocking operation (configurable blocking modes available)

================================================================================
]]

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