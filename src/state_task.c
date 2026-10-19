#include "state_task.h"

#include <stdbool.h>
#include <stdint.h>

#include "app_events.h"
#include "app_log.h"
#include "system_state.h"

static const char *TAG = "STATE";

/*
 * The wait below works in whole ticks, so use the timeout as it exists in ticks.
 * (If the timeout were not a multiple of the tick period, "elapsed >= timeout" could
 * never become true and the task would spin.)
 */
#define INACTIVITY_TIMEOUT_TICKS  pdMS_TO_TICKS(SYSTEM_INACTIVITY_TIMEOUT_MS)
#define INACTIVITY_TIMEOUT_MS_EFF ((uint32_t)(INACTIVITY_TIMEOUT_TICKS * portTICK_PERIOD_MS))

static void apply_state(SystemState state)
{
    if (state == SYSTEM_STATE_ACTIVE) {
        xEventGroupSetBits(g_system_events, EVENT_ACTIVE);
    } else {
        xEventGroupClearBits(g_system_events, EVENT_ACTIVE);
    }
    app_log(TAG, "State -> %s", system_state_name(state));
}

void state_task(void *pvParameters)
{
    (void)pvParameters;

    SystemState state = SYSTEM_STATE_ACTIVE;
    TickType_t last_motion_tick = xTaskGetTickCount();

    apply_state(state);

    for (;;) {
        /*
         * ACTIVE   : block until motion OR until the remaining inactivity time runs out.
         * INACTIVE : block (no timeout) until motion.
         * Either way the task is Blocked, never busy-looping.
         */
        TickType_t wait_ticks = portMAX_DELAY;
        if (state == SYSTEM_STATE_ACTIVE) {
            const TickType_t elapsed = xTaskGetTickCount() - last_motion_tick;
            wait_ticks = (elapsed >= INACTIVITY_TIMEOUT_TICKS) ? 0 : (INACTIVITY_TIMEOUT_TICKS - elapsed);
        }

        const EventBits_t bits = xEventGroupWaitBits(g_system_events,
                                                     EVENT_MOTION,
                                                     pdTRUE,    /* clear EVENT_MOTION on exit */
                                                     pdFALSE,
                                                     wait_ticks);
        const bool motion = (bits & EVENT_MOTION) != 0;

        const TickType_t now = xTaskGetTickCount();
        if (motion) {
            last_motion_tick = now;
        }
        const uint32_t ms_since_motion = (uint32_t)((now - last_motion_tick) * portTICK_PERIOD_MS);

        const SystemState next = evaluate_system_state(state, motion, ms_since_motion, INACTIVITY_TIMEOUT_MS_EFF);
        if (next != state) {
            state = next;
            apply_state(state);
        }
    }
}

BaseType_t state_task_start(void)
{
    return xTaskCreate(state_task, "StateTask", STATE_TASK_STACK, NULL, STATE_TASK_PRIORITY, NULL);
}