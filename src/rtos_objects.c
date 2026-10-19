#include "rtos_objects.h"
#include "sensors.h"
#include "display_mode.h"

QueueHandle_t     modeQueue;
QueueHandle_t     displayQueue;
QueueHandle_t     alarmQueue;
SemaphoreHandle_t serialMutex;

void rtos_objects_init(void)
{
    displayQueue = xQueueCreate(1, sizeof(SensorData));
    alarmQueue   = xQueueCreate(1, sizeof(SensorData));
    serialMutex  = xSemaphoreCreateMutex();
    configASSERT(displayQueue && alarmQueue && serialMutex);
    modeQueue    = xQueueCreate(1, sizeof(DisplayMode));
    configASSERT(displayQueue && alarmQueue && serialMutex && modeQueue);
}




// sulod sa rtos_objects_init():
