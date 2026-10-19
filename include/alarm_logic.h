#pragma once

#define TEMP_LOW_LIMIT_C   18.0f
#define TEMP_HIGH_LIMIT_C  30.0f

typedef enum {
    ALARM_NORMAL = 0,
    ALARM_LOW_TEMPERATURE,
    ALARM_HIGH_TEMPERATURE
} AlarmState;

// Pure function: no hardware, no FreeRTOS. Safe to unit test on the host.
AlarmState evaluateTemperature(float temperature);