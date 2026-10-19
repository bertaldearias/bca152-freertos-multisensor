#include "alarm_logic.h"

AlarmState evaluateTemperature(float temperature)
{
    if (temperature < TEMP_LOW_LIMIT_C) {
        return ALARM_LOW_TEMPERATURE;
    }
    if (temperature > TEMP_HIGH_LIMIT_C) {
        return ALARM_HIGH_TEMPERATURE;
    }
    return ALARM_NORMAL;   // 18.0 and 30.0 exactly are inside the normal range
}