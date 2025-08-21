/**
  ******************************************************************************
  * @file           : tasks.h
  * @brief          : Task function declarations
  ******************************************************************************
  */

#ifndef TASKS_H
#define TASKS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Task function prototypes */
void smbusTask(void *pvParameters);
void uartTask(void *pvParameters);
void batteryMonitorTask(void *pvParameters);
void vescanTask(void *pvParameters);

#ifdef __cplusplus
}
#endif

#endif /* TASKS_H */