#include "../include/sensors.h"

#include <stdio.h>
#include <device.h>
#include <drivers/sensor.h>
#include <math.h>

float sensors_lowPowerAccelSamples[3][TOTAL_SAMPLES_PER_ACCEL_CHANNEL] = {0};
float sensors_highGAccelSamples[3][TOTAL_SAMPLES_PER_ACCEL_CHANNEL] = {0};

float sensors_lowPowerAccelFreqMags[3][NUM_MAGNITUDES] = {0};
float sensors_highGAccelFreqMags[3][NUM_MAGNITUDES] = {0};

bool sensors_init();
bool sensors_sample();
const float *sensors_getMagnitudes(enum AccelType accel, uint8_t idx);

#define M_PI 3.14159265358979323846

static const struct device *lowPowerAccel = DEVICE_DT_GET_ANY(adi_adxl362);
static const struct device *highGAccel = DEVICE_DT_GET_ANY(adi_adxl372);

static void shiftSamples();
static bool fetchSingleAccel(const struct device *accel, float buf[3][TOTAL_SAMPLES_PER_ACCEL_CHANNEL], uint8_t sample_idx, const char *name);
static void toFrequencyMagnitude(float in[TOTAL_SAMPLES_PER_ACCEL_CHANNEL], float out[NUM_MAGNITUDES]);

bool sensors_init()
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

bool sensors_sample()
{
    shiftSamples();

    for (uint8_t i = TOTAL_SAMPLES_PER_ACCEL_CHANNEL - WINDOW_SHIFT_IN_SAMPLES; i < TOTAL_SAMPLES_PER_ACCEL_CHANNEL; i++)
    {
        if (!fetchSingleAccel(lowPowerAccel, sensors_lowPowerAccelSamples, i, "ADXL362"))
            return false;

        if (!fetchSingleAccel(highGAccel, sensors_highGAccelSamples, i, "ADXL372"))
            return false;

        k_sleep(K_USEC(DELAY_BETWEEN_SAMPLES_US));
    }

    toFrequencyMagnitude(sensors_lowPowerAccelSamples[X_IDX], sensors_lowPowerAccelFreqMags[X_IDX]);
    toFrequencyMagnitude(sensors_lowPowerAccelSamples[Y_IDX], sensors_lowPowerAccelFreqMags[Y_IDX]);
    toFrequencyMagnitude(sensors_lowPowerAccelSamples[Z_IDX], sensors_lowPowerAccelFreqMags[Z_IDX]);

    toFrequencyMagnitude(sensors_highGAccelSamples[X_IDX], sensors_highGAccelFreqMags[X_IDX]);
    toFrequencyMagnitude(sensors_highGAccelSamples[Y_IDX], sensors_highGAccelFreqMags[Y_IDX]);
    toFrequencyMagnitude(sensors_highGAccelSamples[Z_IDX], sensors_highGAccelFreqMags[Z_IDX]);

    return true;
}

const float *sensors_getMagnitudes(enum AccelType accel, uint8_t idx)
{
    if (accel == LOW_POWER)
        return sensors_lowPowerAccelFreqMags[idx];
    else if (accel == HIGH_G)
        return sensors_highGAccelFreqMags[idx];
    return NULL;
}

static void shiftSamples()
{
    for (uint8_t i = WINDOW_SHIFT_IN_SAMPLES; i < TOTAL_SAMPLES_PER_ACCEL_CHANNEL; i++)
    {
        sensors_lowPowerAccelSamples[X_IDX][i - WINDOW_SHIFT_IN_SAMPLES] = sensors_lowPowerAccelSamples[X_IDX][i];
        sensors_lowPowerAccelSamples[Y_IDX][i - WINDOW_SHIFT_IN_SAMPLES] = sensors_lowPowerAccelSamples[Y_IDX][i];
        sensors_lowPowerAccelSamples[Z_IDX][i - WINDOW_SHIFT_IN_SAMPLES] = sensors_lowPowerAccelSamples[Z_IDX][i];

        sensors_highGAccelSamples[X_IDX][i - WINDOW_SHIFT_IN_SAMPLES] = sensors_highGAccelSamples[X_IDX][i];
        sensors_highGAccelSamples[Y_IDX][i - WINDOW_SHIFT_IN_SAMPLES] = sensors_highGAccelSamples[Y_IDX][i];
        sensors_highGAccelSamples[Z_IDX][i - WINDOW_SHIFT_IN_SAMPLES] = sensors_highGAccelSamples[Z_IDX][i];
    }
}

static bool fetchSingleAccel(const struct device *accel, float buf[3][TOTAL_SAMPLES_PER_ACCEL_CHANNEL], uint8_t sample_idx, const char *name)
{
    struct sensor_value vals[3];

    if (sensor_sample_fetch(accel) < 0)
    {
        printf("%s fetch error!\n", name);
        return false;
    }

    if (sensor_channel_get(accel, SENSOR_CHAN_ACCEL_X, &vals[X_IDX]) < 0)
    {
        printf("%s's X get error\n", name);
        return false;
    }

    if (sensor_channel_get(accel, SENSOR_CHAN_ACCEL_Y, &vals[Y_IDX]) < 0)
    {
        printf("%s's Y get error\n", name);
        return false;
    }

    if (sensor_channel_get(accel, SENSOR_CHAN_ACCEL_Z, &vals[Z_IDX]) < 0)
    {
        printf("%s's Z get error\n", name);
        return false;
    }

    buf[X_IDX][sample_idx] = sensor_value_to_double(&vals[X_IDX]);
    buf[Y_IDX][sample_idx] = sensor_value_to_double(&vals[Y_IDX]);
    buf[Z_IDX][sample_idx] = sensor_value_to_double(&vals[Z_IDX]);

    return true;
}

static void toFrequencyMagnitude(float in[TOTAL_SAMPLES_PER_ACCEL_CHANNEL], float out[NUM_MAGNITUDES])
{
    // Naive FFT implementation. Assuming that input imag part is always 0.
    for (uint8_t k = 0; k < NUM_MAGNITUDES; k++)
    {
        float outReal = 0;
        float outImag = 0;

        for (uint8_t t = 0; t < TOTAL_SAMPLES_PER_ACCEL_CHANNEL; t++)
        {
            float ang = 2 * M_PI * t * k / (float)TOTAL_SAMPLES_PER_ACCEL_CHANNEL;
            outReal += in[t] * cos(ang);
            outImag += -in[t] * sin(ang);
        }

        out[k] = sqrt(outReal * outReal + outImag * outImag);
    }
}