#pragma once

#include <stdint.h>
#include <stdbool.h>

#define ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL 256                           // MUST BE POWER OF 2!
#define ACCEL_WINDOW_SHIFT_IN_SAMPLES ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL // Must be less or equal to ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL.
#define ACCEL_DELAY_BETWEEN_SAMPLES_US 500

#define ACCEL_DELTA_T (ACCEL_DELAY_BETWEEN_SAMPLES_US / (float)1000000)
#define ACCEL_NUM_MAGNITUDES (ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL / 2 + 1)

#define ACCEL_SAMPLING_FREQ (1 / ACCEL_DELTA_T)
#define ACCEL_MAG_FREQ_START 0
#define ACCEL_MAG_FREQ_STEP (ACCEL_SAMPLING_FREQ / (float)ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL)
#define ACCEL_MAG_FREQ_END ((ACCEL_NUM_MAGNITUDES - 1) * ACCEL_MAG_FREQ_STEP)

#define ACCEL_NUM_AXES 3
#define ACCEL_X_IDX 0
#define ACCEL_Y_IDX 1
#define ACCEL_Z_IDX 2

enum AccelType
{
    ACCEL_LOW_POWER = 0,
    ACCEL_HIGH_G
};

bool accels_init();
bool accels_sample();
const float *accels_getVelocityMagnitudes(enum AccelType accel, uint8_t idx);
