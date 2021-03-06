#include "accels.h"
#include "sig.h"

#include <stdio.h>
#include <device.h>
#include <drivers/sensor.h>
#include <math.h>
#include <timing/timing.h>

bool accles_init();
bool accles_sample();
float *accels_getVelocityMagnitudes(enum AccelType accel, uint8_t idx);

// static float accels_lowPowerAccelSamples[ACCEL_NUM_AXES][ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL];
static float accels_highGAccelSamples[ACCEL_NUM_AXES][ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL];

static const struct device *lowPowerAccel = DEVICE_DT_GET_ANY(adi_adxl362);
static const struct device *highGAccel = DEVICE_DT_GET_ANY(adi_adxl372);

static void shiftSamples();
static bool fetchSingleAccel(const struct device *accel, float buf[ACCEL_NUM_AXES][ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL], uint8_t sample_idx, const char *name);

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

    sig_init();

    return true;
}

bool accels_sample()
{
    timing_t startTime, endTime372;
    uint64_t totalCycles372;
    uint64_t totalNs372;
    int64_t remainingTimeUs;

    shiftSamples();

    for (uint16_t i = ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL - ACCEL_WINDOW_SHIFT_IN_SAMPLES; i < ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL; i++)
    {
        timing_init();
        timing_start();

        startTime = timing_counter_get();

        /*if (!fetchSingleAccel(lowPowerAccel, accels_lowPowerAccelSamples, i, "ADXL362"))
            return false;*/

        if (!fetchSingleAccel(highGAccel, accels_highGAccelSamples, i, "ADXL372"))
            return false;

        endTime372 = timing_counter_get();

        timing_stop();

        totalCycles372 = timing_cycles_get(&startTime, &endTime372);
        totalNs372 = timing_cycles_to_ns(totalCycles372);

        remainingTimeUs = ACCEL_DELAY_BETWEEN_SAMPLES_US - totalNs372 / 1000;

        if (remainingTimeUs > 0)
            k_sleep(K_USEC(remainingTimeUs));
    }

    for (uint8_t i = 0; i < ACCEL_NUM_AXES; i++)
    {
        /*sig_lowPassFilter(accels_lowPowerAccelSamples[i]);
        sig_highPassFilter(accels_lowPowerAccelSamples[i]);
        sig_integrate(accels_lowPowerAccelSamples[i]);
        sig_center(accels_lowPowerAccelSamples[i]);
        sig_hamming(accels_highGAccelSamples[i]);
        sig_fftMagnitude(accels_lowPowerAccelSamples[i]);
        sig_RSS(accels_lowPowerAccelSamples[i]);*/

        //sig_lowPassFilter(accels_highGAccelSamples[i]);
        sig_highPassFilter(accels_highGAccelSamples[i]);
        sig_integrate(accels_highGAccelSamples[i]);
        sig_center(accels_highGAccelSamples[i]);
        sig_hamming(accels_highGAccelSamples[i]);
        sig_fftMagnitude(accels_highGAccelSamples[i]);
        sig_RSS(accels_highGAccelSamples[i]);
    }

    return true;
}

float *accels_getVelocityMagnitudes(enum AccelType accel, uint8_t idx)
{
    /*if (accel == ACCEL_LOW_POWER)
        return accels_lowPowerAccelSamples[idx];*/

    if (accel == ACCEL_HIGH_G)
        return accels_highGAccelSamples[idx];

    return NULL;
}

static void shiftSamples()
{
    for (uint8_t i = 0; i < ACCEL_NUM_AXES; i++)
    {
        for (uint16_t j = ACCEL_WINDOW_SHIFT_IN_SAMPLES; j < ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL; j++)
        {
            // accels_lowPowerAccelSamples[i][j - ACCEL_WINDOW_SHIFT_IN_SAMPLES] = accels_lowPowerAccelSamples[i][j];
            accels_highGAccelSamples[i][j - ACCEL_WINDOW_SHIFT_IN_SAMPLES] = accels_highGAccelSamples[i][j];
        }
    }
}

static bool fetchSingleAccel(const struct device *accel, float buf[ACCEL_NUM_AXES][ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL], uint8_t sample_idx, const char *name)
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

    buf[ACCEL_X_IDX][sample_idx] = fabs(sensor_value_to_double(&vals[ACCEL_X_IDX]));
    buf[ACCEL_Y_IDX][sample_idx] = fabs(sensor_value_to_double(&vals[ACCEL_Y_IDX]));
    buf[ACCEL_Z_IDX][sample_idx] = fabs(sensor_value_to_double(&vals[ACCEL_Z_IDX]));

    return true;
}