#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

extern QueueHandle_t     displayQueue;
extern QueueHandle_t     alarmQueue;
extern SemaphoreHandle_t serialMutex;

void rtos_objects_init(void);