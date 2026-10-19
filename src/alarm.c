#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "alarm.h"
#include "alarm_logic.h"
#include "sensors.h"
#include "rtos_objects.h"

#define BUZZER_GPIO       27
#define BUZZER_FREQ_HZ    2000
#define BUZZER_DUTY_ON    512     // 50 % of 10-bit resolution (1023)
#define BUZZER_DUTY_OFF   0

static void buzzer_set(bool on)
{
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0,
                                  on ? BUZZER_DUTY_ON : BUZZER_DUTY_OFF));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
}

static const char *alarm_name(AlarmState s)
{
    switch (s) {
    case ALARM_NORMAL:           return "NORMAL";
    case ALARM_LOW_TEMPERATURE:  return "LOW TEMPERATURE";
    case ALARM_HIGH_TEMPERATURE: return "HIGH TEMPERATURE";
    default:                     return "?";
    }
}

void alarm_init(void)
{
    ledc_timer_config_t timer = {
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num       = LEDC_TIMER_0,
        .freq_hz         = BUZZER_FREQ_HZ,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer));

    ledc_channel_config_t channel = {
        .gpio_num   = BUZZER_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_0,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = BUZZER_DUTY_OFF,
        .hpoint     = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&channel));
}

void AlarmTask(void *pvParameters)
{
    SensorData data;
    AlarmState current = ALARM_NORMAL;

    for (;;)
    {
        // Blocked until SensorTask publishes a new reading
        if (xQueueReceive(alarmQueue, &data, portMAX_DELAY) == pdTRUE) {
            AlarmState next = evaluateTemperature(data.temperature);

            if (next != current) {
                current = next;
                buzzer_set(current != ALARM_NORMAL);

                xSemaphoreTake(serialMutex, portMAX_DELAY);
                printf("Alarm state: %s (%.1f C)\n", alarm_name(current), data.temperature);
                xSemaphoreGive(serialMutex);
            }
        }
    }
}