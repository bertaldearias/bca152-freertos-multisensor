#include "system_state.h"

SystemState evaluate_system_state(SystemState current,
                                  bool motion_detected,
                                  uint32_t ms_since_motion,
                                  uint32_t timeout_ms)
{
    /* Motion always wins: ACTIVE stays ACTIVE, INACTIVE returns to ACTIVE. */
    if (motion_detected) {
        return SYSTEM_STATE_ACTIVE;
    }

    /* No motion: only ACTIVE can time out; INACTIVE stays INACTIVE. */
    if (current == SYSTEM_STATE_ACTIVE && ms_since_motion >= timeout_ms) {
        return SYSTEM_STATE_INACTIVE;
    }

    return current;
}

const char *system_state_name(SystemState state)
{
    switch (state) {
    case SYSTEM_STATE_ACTIVE:   return "ACTIVE";
    case SYSTEM_STATE_INACTIVE: return "INACTIVE";
    default:                    return "UNKNOWN";
    }
}