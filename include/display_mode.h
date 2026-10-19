#pragma once

typedef enum {
    DISPLAY_MODE_TEMPERATURE = 0,
    DISPLAY_MODE_HUMIDITY,
    DISPLAY_MODE_LIGHT,
    DISPLAY_MODE_MOTION,
    DISPLAY_MODE_COUNT
} DisplayMode;

DisplayMode nextDisplayMode(DisplayMode mode);      // clockwise
DisplayMode previousDisplayMode(DisplayMode mode);  // counterclockwise