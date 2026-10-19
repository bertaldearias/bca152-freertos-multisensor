#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

/*
 * Hardware-independent ACTIVE/INACTIVE state machine decision logic.
 * No FreeRTOS / ESP-IDF includes on purpose, so it can be unit-tested on the host (pio test -e native).
 *
 *            motion
 *        +-------------+
 *        v             |
 *    +--------+   no motion for >= timeout   +----------+
 *    | ACTIVE | ---------------------------> | INACTIVE |
 *    +--------+ <--------------------------- +----------+
 *                         motion
 */

#include <stdbool.h>
#include <stdint.h>

#define SYSTEM_INACTIVITY_TIMEOUT_MS 15000U   /* short value for lab testing (spec item 31) */

typedef enum {
    SYSTEM_STATE_ACTIVE = 0,
    SYSTEM_STATE_INACTIVE
} SystemState;

/*
 * current          : state right now
 * motion_detected  : motion was seen since the last evaluation
 * ms_since_motion  : time since the last motion (0 when motion_detected)
 * timeout_ms       : inactivity timeout
 * returns          : the next state
 */
SystemState evaluate_system_state(SystemState current,
                                  bool motion_detected,
                                  uint32_t ms_since_motion,
                                  uint32_t timeout_ms);

const char *system_state_name(SystemState state);

#endif /* SYSTEM_STATE_H */