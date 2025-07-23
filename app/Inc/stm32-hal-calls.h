#ifndef STM32_HAL_CALLS_H
#define STM32_HAL_CALLS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Weak attribute for function definitions
#ifndef __weak
#define __weak __attribute__((weak))
#endif

// All HAL types and function declarations are now in Tasks/Inc/hal_types.h

#ifdef __cplusplus
}
#endif

#endif /* STM32_HAL_CALLS_H */