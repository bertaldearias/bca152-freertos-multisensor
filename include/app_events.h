#ifndef APP_EVENTS_H
#define APP_EVENTS_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

/*
 * System event group - one bit = one meaningful system event/status.
 *
 * Bit           Kind   Producer     Set when                      Cleared when                    Consumer(s)
 * ------------  -----  -----------  ----------------------------  ------------------------------  ----------------------------
 * EVENT_ACTIVE  level  StateTask    system enters ACTIVE          system enters INACTIVE          DisplayTask, InputTask, AlarmTask
 * EVENT_MOTION  event  MotionTask   PIR sampled HIGH              StateTask (clear-on-exit wait)  StateTask
 * EVENT_ALARM   level  AlarmTask    temperature outside 18..30 C  temperature back in range       DisplayTask (alarm indicator)
 *
 * "level" = reflects a condition that stays true until it changes.
 * "event" = "something just happened"; the consumer clears it when it handles it.
 */
#define EVENT_ACTIVE  (1U << 0)
#define EVENT_MOTION  (1U << 1)
#define EVENT_ALARM   (1U << 2)

extern EventGroupHandle_t g_system_events;

/* Create the event group. Call once, at the top of app_main(), before any task is created. */
bool app_events_init(void);

#endif /* APP_EVENTS_H */