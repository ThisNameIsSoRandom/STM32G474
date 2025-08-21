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

// Queue handle type
typedef void* QueueHandle_t;

// Semaphore handle type
typedef void* SemaphoreHandle_t;

// Tick type
typedef uint32_t TickType_t;

// Base type
typedef long BaseType_t;

// Unsigned base type
typedef unsigned long UBaseType_t;

// Common FreeRTOS constants
#ifndef pdTRUE
#define pdTRUE  ((BaseType_t)1)
#endif
#ifndef pdFALSE
#define pdFALSE ((BaseType_t)0)
#endif
#ifndef pdPASS
#define pdPASS  (pdTRUE)
#endif
#ifndef pdFAIL
#define pdFAIL  (pdFALSE)
#endif

// Tick conversion macros - Only define if real FreeRTOS not present
#ifndef pdMS_TO_TICKS
#define pdMS_TO_TICKS(xTimeInMs) ((TickType_t)(((TickType_t)(xTimeInMs) * 1000) / 1000))
#endif

#endif /* INC_FREERTOS_H */

// ===============================
// FreeRTOS Function Declarations
// ===============================

#ifdef INC_FREERTOS_H
// When real FreeRTOS is included, just use the real headers
#include "queue.h"
#include "task.h"
#else
// When FreeRTOS is not available, declare our own weak implementations
// Task control functions - platform will provide implementations
void vTaskDelete(TaskHandle_t xTaskToDelete);
void vTaskDelay(const TickType_t xTicksToDelay);
TickType_t xTaskGetTickCount(void);

// Queue functions - platform will provide implementations
QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength, UBaseType_t uxItemSize);
BaseType_t xQueueSend(QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait);
BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait);
BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken);
UBaseType_t uxQueueMessagesWaiting(const QueueHandle_t xQueue);
#endif

// Common queue constants
#ifndef portMAX_DELAY
#define portMAX_DELAY ((TickType_t)0xffffffffUL)
#endif

// Tick conversion constants
#ifndef portTICK_PERIOD_MS
#define portTICK_PERIOD_MS ((TickType_t)1)
#endif

#ifdef __cplusplus
}
#endif

#endif /* FREERTOS_TYPES_H */