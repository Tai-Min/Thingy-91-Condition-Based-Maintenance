#include "../include/accels.h"

#include <stdio.h>
#include <device.h>
#include <drivers/sensor.h>
#include <math.h>
#include <timing/timing.h>

bool accles_init();
bool accles_sample();
const float *accles_getMagnitudes(enum AccelType accel, uint8_t idx);

#define M_PI 3.14159265358979323846

static float accels_lowPowerAccelSamples[3][ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL] = {0};
static float accels_highGAccelSamples[3][ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL] = {0};

static float accels_lowPowerAccelFreqMags[3][ACCEL_NUM_MAGNITUDES] = {0};
static float accels_highGAccelFreqMags[3][ACCEL_NUM_MAGNITUDES] = {0};

static const struct device *lowPowerAccel = DEVICE_DT_GET_ANY(adi_adxl362);
static const struct device *highGAccel = DEVICE_DT_GET_ANY(adi_adxl372);

static void shiftSamples();
static bool fetchSingleAccel(const struct device *accel, float buf[3][ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL], uint8_t sample_idx, const char *name);
static void toFrequencyMagnitude(float in[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL], float out[ACCEL_NUM_MAGNITUDES]);

bool accels_init()
{
    if (!device_is_ready(lowPowerAccel))
    {
        printf("GET ADXL362 failed!\n");
        return false;
    }

    if (!device_is_ready(highGAccel))
    {
        printf("GET ADXL372 failed!\n");
        return false;
    }

    return true;
}

bool accels_sample()
{
    timing_t startTime, endTime362, endTime372;
    uint64_t totalCycles362, totalCycles372;
    uint64_t totalNs362, totalNs372;
    int64_t remainingTimeUs;

    shiftSamples();

    for (uint8_t i = ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL - ACCEL_WINDOW_SHIFT_IN_SAMPLES; i < ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL; i++)
    {
        timing_init();
        timing_start();

        startTime = timing_counter_get();

        if (!fetchSingleAccel(lowPowerAccel, accels_lowPowerAccelSamples, i, "ADXL362"))
            return false;

        endTime362 = timing_counter_get();

        if (!fetchSingleAccel(highGAccel, accels_highGAccelSamples, i, "ADXL372"))
            return false;

        endTime372 = timing_counter_get();

        timing_stop();

        totalCycles362 = timing_cycles_get(&startTime, &endTime362);
        totalNs362 = timing_cycles_to_ns(totalCycles362);

        totalCycles372 = timing_cycles_get(&endTime362, &endTime372);
        totalNs372 = timing_cycles_to_ns(totalCycles372);

        if (totalNs362 > totalNs372)
            remainingTimeUs = ACCEL_DELAY_BETWEEN_SAMPLES_US - totalNs362 / 1000;
        else
            remainingTimeUs = ACCEL_DELAY_BETWEEN_SAMPLES_US - totalNs372 / 1000;

        if (remainingTimeUs > 0)
            k_sleep(K_USEC(remainingTimeUs));
    }

    toFrequencyMagnitude(accels_lowPowerAccelSamples[ACCEL_X_IDX], accels_lowPowerAccelFreqMags[ACCEL_X_IDX]);
    toFrequencyMagnitude(accels_lowPowerAccelSamples[ACCEL_Y_IDX], accels_lowPowerAccelFreqMags[ACCEL_Y_IDX]);
    toFrequencyMagnitude(accels_lowPowerAccelSamples[ACCEL_Z_IDX], accels_lowPowerAccelFreqMags[ACCEL_Z_IDX]);

    toFrequencyMagnitude(accels_highGAccelSamples[ACCEL_X_IDX], accels_highGAccelFreqMags[ACCEL_X_IDX]);
    toFrequencyMagnitude(accels_highGAccelSamples[ACCEL_Y_IDX], accels_highGAccelFreqMags[ACCEL_Y_IDX]);
    toFrequencyMagnitude(accels_highGAccelSamples[ACCEL_Z_IDX], accels_highGAccelFreqMags[ACCEL_Z_IDX]);

    return true;
}

float *accels_getMagnitudes(enum AccelType accel, uint8_t idx)
{
    if (accel == ACCEL_LOW_POWER)
        return accels_lowPowerAccelFreqMags[idx];
    else if (accel == ACCEL_HIGH_G)
        return accels_highGAccelFreqMags[idx];
    return NULL;
}

static void shiftSamples()
{
    for (uint8_t i = ACCEL_WINDOW_SHIFT_IN_SAMPLES; i < ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL; i++)
    {
        accels_lowPowerAccelSamples[ACCEL_X_IDX][i - ACCEL_DELAY_BETWEEN_SAMPLES_US] = accels_lowPowerAccelSamples[ACCEL_X_IDX][i];
        accels_lowPowerAccelSamples[ACCEL_Y_IDX][i - ACCEL_DELAY_BETWEEN_SAMPLES_US] = accels_lowPowerAccelSamples[ACCEL_Y_IDX][i];
        accels_lowPowerAccelSamples[ACCEL_Z_IDX][i - ACCEL_DELAY_BETWEEN_SAMPLES_US] = accels_lowPowerAccelSamples[ACCEL_Z_IDX][i];

        accels_highGAccelSamples[ACCEL_X_IDX][i - ACCEL_DELAY_BETWEEN_SAMPLES_US] = accels_highGAccelSamples[ACCEL_X_IDX][i];
        accels_highGAccelSamples[ACCEL_Y_IDX][i - ACCEL_DELAY_BETWEEN_SAMPLES_US] = accels_highGAccelSamples[ACCEL_Y_IDX][i];
        accels_highGAccelSamples[ACCEL_Z_IDX][i - ACCEL_DELAY_BETWEEN_SAMPLES_US] = accels_highGAccelSamples[ACCEL_Z_IDX][i];
    }
}

static bool fetchSingleAccel(const struct device *accel, float buf[3][ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL], uint8_t sample_idx, const char *name)
{
    struct sensor_value vals[3];

    if (sensor_sample_fetch(accel) < 0)
    {
        printf("%s fetch error!\n", name);
        return false;
    }

    if (sensor_channel_get(accel, SENSOR_CHAN_ACCEL_X, &vals[ACCEL_X_IDX]) < 0)
    {
        printf("%s's X get error\n", name);
        return false;
    }

    if (sensor_channel_get(accel, SENSOR_CHAN_ACCEL_Y, &vals[ACCEL_Y_IDX]) < 0)
    {
        printf("%s's Y get error\n", name);
        return false;
    }

    if (sensor_channel_get(accel, SENSOR_CHAN_ACCEL_Z, &vals[ACCEL_Z_IDX]) < 0)
    {
        printf("%s's Z get error\n", name);
        return false;
    }

    buf[ACCEL_X_IDX][sample_idx] = sensor_value_to_double(&vals[ACCEL_X_IDX]);
    buf[ACCEL_Y_IDX][sample_idx] = sensor_value_to_double(&vals[ACCEL_Y_IDX]);
    buf[ACCEL_Z_IDX][sample_idx] = sensor_value_to_double(&vals[ACCEL_Z_IDX]);

    return true;
}

static void toFrequencyMagnitude(float in[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL], float out[ACCEL_NUM_MAGNITUDES])
{
    // Naive FFT implementation. Assuming that input imag part is always 0.
    for (uint8_t k = 0; k < ACCEL_NUM_MAGNITUDES; k++)
    {
        float outReal = 0;
        float outImag = 0;

        for (uint8_t t = 0; t < ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL; t++)
        {
            float ang = 2 * M_PI * t * k / (float)ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL;
            outReal += in[t] * cos(ang);
            outImag += -in[t] * sin(ang);
        }

        out[k] = sqrt(outReal * outReal + outImag * outImag);
    }
}