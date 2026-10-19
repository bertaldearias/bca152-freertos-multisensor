#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include "dht.h"
#include "sensors.h"
#include "rtos_objects.h"

#define DHT_GPIO        GPIO_NUM_4
#define LDR_ADC_CHANNEL ADC_CHANNEL_6   // GPIO34
#define SENSOR_PERIOD_MS 2000

static adc_oneshot_unit_handle_t adc1_handle;

void sensors_init(void)
{
    adc_oneshot_unit_init_cfg_t init_config = { .unit_id = ADC_UNIT_1 };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc1_handle));

    adc_oneshot_chan_cfg_t chan_config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten    = ADC_ATTEN_DB_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, LDR_ADC_CHANNEL, &chan_config));
}

void SensorTask(void *pvParameters)
{
    SensorData data = {0};

    vTaskDelay(pdMS_TO_TICKS(2000));              // DHT22 warm-up
    TickType_t lastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        float t, h;
        if (dht_read_float_data(DHT_TYPE_AM2301, DHT_GPIO, &h, &t) == ESP_OK) {
            data.temperature = t;
            data.humidity    = h;
        }

        int raw = 0;
        if (adc_oneshot_read(adc1_handle, LDR_ADC_CHANNEL, &raw) == ESP_OK) {
            data.lightLevel = (raw * 100) / 4095;
        }

        xQueueOverwrite(displayQueue, &data);
        xQueueOverwrite(alarmQueue, &data);

        xSemaphoreTake(serialMutex, portMAX_DELAY);
        printf("Temperature: %.2f C\nHumidity: %.2f %%\nLight: %d %%\n",
               data.temperature, data.humidity, data.lightLevel);
        xSemaphoreGive(serialMutex);

        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(SENSOR_PERIOD_MS));
    }
}