#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "dht.h"

#define DHT_GPIO GPIO_NUM_4
#define LDR_ADC_CHANNEL ADC_CHANNEL_6  // GPIO34

adc_oneshot_unit_handle_t adc1_handle;

void TaskA(void *pvParameters)
{
    while (1)
    {
        printf("Task A running\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void TaskB(void *pvParameters)
{
    while (1)
    {
        printf("Task B running\n");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void readSensors(void)
{
    float temperature = 0, humidity = 0;

    if (dht_read_float_data(DHT_TYPE_AM2301, DHT_GPIO, &humidity, &temperature) == ESP_OK)
    {
        printf("Temperature: %.2f C\n", temperature);
        printf("Humidity: %.2f %%\n", humidity);
    }
    else
    {
        printf("DHT22 read failed\n");
    }

    int raw = 0;
    adc_oneshot_read(adc1_handle, LDR_ADC_CHANNEL, &raw);
    int ldr_percent = (raw * 100) / 4095;  // documented raw-to-percent conversion
    printf("Light Level: %d %% (raw ADC: %d)\n", ldr_percent, raw);
}

void SensorTask(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(2000));  // warm-up delay para sa DHT22
    TickType_t lastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        readSensors();
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(2000));
    }
}

void app_main(void)
{
    printf("BCA152 FreeRTOS Multisensor\n");
    printf("System starting...\n");

    // ADC init for LDR
    adc_oneshot_unit_init_cfg_t init_config = { .unit_id = ADC_UNIT_1 };
    adc_oneshot_new_unit(&init_config, &adc1_handle);

    adc_oneshot_chan_cfg_t chan_config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };
    adc_oneshot_config_channel(adc1_handle, LDR_ADC_CHANNEL, &chan_config);

    xTaskCreate(TaskA, "TaskA", 2048, NULL, 1, NULL);
    xTaskCreate(TaskB, "TaskB", 2048, NULL, 1, NULL);
    xTaskCreate(SensorTask, "SensorTask", 4096, NULL, 2, NULL);
}