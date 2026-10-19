#include "app_events.h"

EventGroupHandle_t g_system_events = NULL;

bool app_events_init(void)
{
    if (g_system_events == NULL) {
        g_system_events = xEventGroupCreate();
    }
    return g_system_events != NULL;
}