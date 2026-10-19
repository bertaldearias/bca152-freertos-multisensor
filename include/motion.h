#ifndef MOTION_H
#define MOTION_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

/* PIR OUT is wired to GPIO 13 in diagram.json (GPIO 27 is used by the buzzer) */
#define MOTION_PIR_GPIO        GPIO_NUM_13

#define MOTION_SAMPLE_MS       100     /* PIR sampling period                           */
#define MOTION_TASK_PRIORITY   3       /* needs prompt response: a missed pulse = missed motion */
#define MOTION_TASK_STACK      4096    /* bytes (ESP-IDF), printf needs headroom        */

/* Configure the PIR GPIO as input. Call once from app_main(). */
void motion_init(void);

/* Create MotionTask. Returns pdPASS on success. */
BaseType_t motion_task_start(void);

/* Latest PIR level (true = motion right now). Lock-free atomic snapshot for display/sensor use. */
bool motion_is_detected(void);

void motion_task(void *pvParameters);

#endif /* MOTION_H */