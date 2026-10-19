#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rtos_objects.h"
#include "app_events.h"
#include "sensors.h"
#include "display.h"
#include "input.h"
#include "alarm.h"

void app_main(void)
{
    printf("BCA152 FreeRTOS Multisensor\n");
    printf("System starting...\n");

    // Objects must exist BEFORE any task that uses them is created
    app_events_init();
    rtos_objects_init();
    sensors_init();
    display_init();
    input_init();
    alarm_init();

    xTaskCreate(InputTask,   "InputTask",   3072, NULL, 3, NULL);
    xTaskCreate(SensorTask,  "SensorTask",  4096, NULL, 2, NULL);
    xTaskCreate(AlarmTask,   "AlarmTask",   3072, NULL, 2, NULL);
    xTaskCreate(DisplayTask, "DisplayTask", 4096, NULL, 1, NULL);
}