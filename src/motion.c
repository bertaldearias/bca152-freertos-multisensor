#include "motion.h"

#include <stdatomic.h>

#include "esp_err.h"
#include "app_events.h"
#include "app_log.h"

static const char *TAG = "MOTION";

/* Single writer (MotionTask), many readers -> atomic snapshot, no mutex needed. */
static atomic_bool s_motion_now = false;

void motion_init(void)
{
    const gpio_config_t cfg = {
        .pin_bit_mask = 1ULL << MOTION_PIR_GPIO,
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&cfg));
}

bool motion_is_detected(void)
{
    return atomic_load(&s_motion_now);
}

void motion_task(void *pvParameters)
{
    (void)pvParameters;

    TickType_t last_wake = xTaskGetTickCount();
    bool previous = false;

    for (;;) {
        const bool now = (gpio_get_level(MOTION_PIR_GPIO) != 0);

        atomic_store(&s_motion_now, now);

        if (now) {
            /* "Motion happened" event for StateTask (restarts the inactivity timer). */
            xEventGroupSetBits(g_system_events, EVENT_MOTION);
        }

        if (now != previous) {
            app_log(TAG, "Motion %s", now ? "detected" : "cleared");
            previous = now;
        }

        /* Fixed-rate sampling, no drift. */
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(MOTION_SAMPLE_MS));
    }
}

BaseType_t motion_task_start(void)
{
    return xTaskCreate(motion_task, "MotionTask", MOTION_TASK_STACK, NULL, MOTION_TASK_PRIORITY, NULL);
}