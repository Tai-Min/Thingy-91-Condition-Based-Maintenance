#pragma once

#include <stdbool.h>

enum Alert {
    OKAY = 0,
    WARNING,
    CRITICAL
};

bool alerts_init();
void alerts_set(enum Alert a);
void alerts_transmitting(bool t);