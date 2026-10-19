#pragma once
#include <stdbool.h>

typedef struct {
    float temperature;
    float humidity;
    int   lightLevel;      // 0-100 %
    bool  motionDetected;
} SensorData;

void sensors_init(void);
void SensorTask(void *pvParameters);