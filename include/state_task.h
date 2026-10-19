#ifndef STATE_TASK_H
#define STATE_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define STATE_TASK_PRIORITY   2      /* gates Display/Alarm/Input; must not wait behind OLED drawing (prio 1) */
#define STATE_TASK_STACK      4096   /* bytes */

/* Create StateTask. Returns pdPASS on success. */
BaseType_t state_task_start(void);

void state_task(void *pvParameters);

#endif /* STATE_TASK_H */