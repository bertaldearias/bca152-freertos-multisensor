#include "app_log.h"

#include <stdarg.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static SemaphoreHandle_t s_serial_mutex = NULL;

bool app_log_init(void)
{
    if (s_serial_mutex == NULL) {
        s_serial_mutex = xSemaphoreCreateMutex();
    }
    return s_serial_mutex != NULL;
}

void app_log(const char *tag, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    const bool locked = (s_serial_mutex != NULL) &&
                        (xSemaphoreTake(s_serial_mutex, portMAX_DELAY) == pdTRUE);

    /* Three separate writes on purpose: this is the critical section the mutex protects. */
    printf("[%s] ", tag);
    vprintf(fmt, args);
    printf("\n");

    if (locked) {
        xSemaphoreGive(s_serial_mutex);
    }

    va_end(args);
}