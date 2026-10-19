#include "display_mode.h"

DisplayMode nextDisplayMode(DisplayMode mode)
{
    return (DisplayMode)((mode + 1) % DISPLAY_MODE_COUNT);
}

DisplayMode previousDisplayMode(DisplayMode mode)
{
    return (DisplayMode)((mode + DISPLAY_MODE_COUNT - 1) % DISPLAY_MODE_COUNT);
}