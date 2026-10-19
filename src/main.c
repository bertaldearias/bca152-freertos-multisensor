#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rtos_objects.h"
#include "sensors.h"
#include "display.h"
#include "input.h"

void app_main(void)
{
    printf("BCA152 FreeRTOS Multisensor\n");
    printf("System starting...\n");

    rtos_objects_init();
    sensors_init();
    display_init();
    input_init();

    xTaskCreate(InputTask,   "InputTask",   3072, NULL, 3, NULL);
    xTaskCreate(SensorTask,  "SensorTask",  4096, NULL, 2, NULL);
    xTaskCreate(DisplayTask, "DisplayTask", 4096, NULL, 1, NULL);
}