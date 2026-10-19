#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "input.h"
#include "display_mode.h"
#include "rtos_objects.h"

#define ENC_CLK_GPIO     GPIO_NUM_32
#define ENC_DT_GPIO      GPIO_NUM_33
#define ENC_SW_GPIO      GPIO_NUM_25
#define STEPS_PER_DETENT 4

// vTaskDelayUntil() asserts if the period is 0 ticks.
// At 100 Hz tick rate, pdMS_TO_TICKS(2) == 0, so force a minimum of 1 tick.
#define INPUT_PERIOD_TICKS ((pdMS_TO_TICKS(2) > 0) ? pdMS_TO_TICKS(2) : 1)

// Quadrature decoder: index = (previous << 2) | current, each = (CLK << 1) | DT
static const int8_t enc_table[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
};

static const char *mode_name(DisplayMode m)
{
    switch (m) {
    case DISPLAY_MODE_TEMPERATURE: return "TEMPERATURE";
    case DISPLAY_MODE_HUMIDITY:    return "HUMIDITY";
    case DISPLAY_MODE_LIGHT:       return "LIGHT";
    case DISPLAY_MODE_MOTION:      return "MOTION";
    default:                       return "?";
    }
}

void input_init(void)
{
    gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << ENC_CLK_GPIO) | (1ULL << ENC_DT_GPIO) | (1ULL << ENC_SW_GPIO),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&cfg));
}

void InputTask(void *pvParameters)
{
    DisplayMode mode = DISPLAY_MODE_TEMPERATURE;
    uint8_t prev = (uint8_t)((gpio_get_level(ENC_CLK_GPIO) << 1) | gpio_get_level(ENC_DT_GPIO));
    int acc = 0;

    TickType_t lastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        uint8_t curr = (uint8_t)((gpio_get_level(ENC_CLK_GPIO) << 1) | gpio_get_level(ENC_DT_GPIO));
        acc += enc_table[(prev << 2) | curr];
        prev = curr;

        bool changed = false;
        if (acc >= STEPS_PER_DETENT) {
            mode = nextDisplayMode(mode);
            acc = 0;
            changed = true;
        } else if (acc <= -STEPS_PER_DETENT) {
            mode = previousDisplayMode(mode);
            acc = 0;
            changed = true;
        }

        if (changed) {
            xQueueOverwrite(modeQueue, &mode);

            xSemaphoreTake(serialMutex, portMAX_DELAY);
            printf("Display mode: %s\n", mode_name(mode));
            xSemaphoreGive(serialMutex);
        }

        vTaskDelayUntil(&lastWakeTime, INPUT_PERIOD_TICKS);
    }
}