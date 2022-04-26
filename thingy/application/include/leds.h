#pragma once

#include <stdbool.h>

enum VisualAlert {
    OKAY = 0,
    WARNING,
    CRITICAL
};

bool leds_init();
void leds_setState(enum VisualAlert a);
void leds_setTransmitting(bool t);