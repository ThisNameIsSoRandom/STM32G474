#ifndef BATTERY_MONITOR_TASK_H
#define BATTERY_MONITOR_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

// Battery monitor task function
void batteryMonitorTask(void *pvParameters);

// Task configuration
#define BATTERY_MONITOR_TASK_PRIORITY    2
#define BATTERY_MONITOR_TASK_STACK_SIZE  512

#ifdef __cplusplus
}
#endif

#endif // BATTERY_MONITOR_TASK_H