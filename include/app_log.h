#ifndef APP_LOG_H
#define APP_LOG_H

#include <stdbool.h>

/*
 * Mutex-protected serial output.
 *
 * Shared resource : the console UART (stdout).
 * Competing tasks : every task that logs (Sensor, Motion, State, Alarm, Input, Display).
 * Failure mode    : one log line is emitted as several printf() calls (prefix, body, newline).
 *                   Without the mutex a higher-priority task can preempt in the middle,
 *                   so the prefix of task A gets glued to the body of task B -> garbled lines.
 */

/* Create the mutex. Call once, at the top of app_main(), before any task is created. */
bool app_log_init(void);

/* Thread-safe "[TAG] message\n". Never call from an ISR. */
void app_log(const char *tag, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

#endif /* APP_LOG_H */