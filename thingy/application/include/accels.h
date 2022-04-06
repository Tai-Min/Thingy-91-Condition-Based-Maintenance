#pragma once

#include <stdint.h>
#include <stdbool.h>

#define ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL 16 // MUST BE POWER OF 2!
#define ACCEL_WINDOW_SHIFT_IN_SAMPLES 8          // Must be less or equal to ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL.
#define ACCEL_DELAY_BETWEEN_SAMPLES_US 200

#define ACCEL_SAMPLING_FREQ (1 / (DELAY_BETWEEN_SAMPLES_US / (float)1000000))
#define ACCEL_NUM_MAGNITUDES (ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL / 2 + 1)
#define ACCEL_MAG_FREQ_START 0
#define ACCEL_MAG_FREQ_STEP (SAMPLING_FREQ / (float)ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL)
#define ACCEL_MAG_FREQ_END ((NUM_MAGNITUDES - 1) * MAG_FREQ_STEP)

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
float *accels_getMagnitudes(enum AccelType accel, uint8_t idx);
