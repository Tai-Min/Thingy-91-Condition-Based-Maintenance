#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "accels.h"

#define SIG_LOW_PASS_CUTOFF_FREQ 240
#define SIG_HIGH_PASS_CUTOFF_FREQ 100
#define SIG_RSS_WINDOW_LENGTH 16

void sig_init();

void sig_center(float signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL]);
void sig_lowPassFilter(float signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL]);
void sig_highPassFilter(float signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL]);
void sig_integrate(float signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL]);
void sig_hamming(float signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL]);
void sig_fftMagnitude(float signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL]);
void sig_RSS(float magnitudes[ACCEL_NUM_MAGNITUDES]);

#ifdef __cplusplus
}
#endif