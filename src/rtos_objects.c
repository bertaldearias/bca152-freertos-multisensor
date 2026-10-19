#include "rtos_objects.h"
#include "sensors.h"

QueueHandle_t     displayQueue;
QueueHandle_t     alarmQueue;
SemaphoreHandle_t serialMutex;

void rtos_objects_init(void)
{
    displayQueue = xQueueCreate(1, sizeof(SensorData));
    alarmQueue   = xQueueCreate(1, sizeof(SensorData));
    serialMutex  = xSemaphoreCreateMutex();
    configASSERT(displayQueue && alarmQueue && serialMutex);
}
