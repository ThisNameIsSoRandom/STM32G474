#ifndef FREERTOS_TYPES_H
#define FREERTOS_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

// ===============================
// FreeRTOS Types - Only define if real FreeRTOS not present
// ===============================

#ifndef INC_FREERTOS_H

// Task handle type
typedef void* TaskHandle_t;

// Tick type
typedef uint32_t TickType_t;

// Base type
typedef long BaseType_t;

// Common FreeRTOS constants
#define pdTRUE  ((BaseType_t)1)
#define pdFALSE ((BaseType_t)0)
#define pdPASS  (pdTRUE)
#define pdFAIL  (pdFALSE)

#endif /* INC_FREERTOS_H */

// ===============================
// FreeRTOS Function Declarations - Always declare
// ===============================

// Task control functions - platform will provide implementations
void vTaskDelete(TaskHandle_t xTaskToDelete);
void vTaskDelay(const TickType_t xTicksToDelay);

#ifdef __cplusplus
}
#endif

#endif /* FREERTOS_TYPES_H */