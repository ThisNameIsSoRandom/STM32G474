#ifndef APP_TASKS_H
#define APP_TASKS_H

#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

// Task declarations - these will be called by platform apps
void StartDefaultTask(void *argument);
void StartBlinkErrorCode(void *argument);
void StartLedTask(void *argument);

// Task handles and attributes
extern osThreadId_t defaultTaskHandle;
extern const osThreadAttr_t defaultTask_attributes;

extern osThreadId_t BlinkErrorCodeHandle;
extern const osThreadAttr_t BlinkErrorCode_attributes;

#ifdef __cplusplus
}
#endif

#endif // APP_TASKS_H