#pragma once

#include <stdint.h>
#include <stdbool.h>

#define TOTAL_SAMPLES_PER_ACCEL_CHANNEL 16 // MUST BE POWER OF 2!
#define WINDOW_SHIFT_IN_SAMPLES 16         // Must be less or equal to TOTAL_SAMPLES_PER_ACCEL_CHANNEL.
#define DELAY_BETWEEN_SAMPLES_US 1000

#define SAMPLING_FREQ (1 / (DELAY_BETWEEN_SAMPLES_US / (float)1000000))
#define NUM_MAGNITUDES (TOTAL_SAMPLES_PER_ACCEL_CHANNEL / 2 + 1)
#define MAG_FREQ_START 0
#define MAG_FREQ_STEP (SAMPLING_FREQ / (float)TOTAL_SAMPLES_PER_ACCEL_CHANNEL)
#define MAG_FREQ_END ((NUM_MAGNITUDES - 1) * MAG_FREQ_STEP)

#define X_IDX 0
#define Y_IDX 1
#define Z_IDX 2

enum AccelType
{
    LOW_POWER = 0,
    HIGH_G
};

bool sensors_init();
bool sensors_sample();
const float *sensors_getMagnitudes(enum AccelType accel, uint8_t idx);
