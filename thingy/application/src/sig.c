#include "../include/sig.h"

#include <math.h>

void sig_init();

void sig_center(float signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL]);
void sig_lowPassFilter(float signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL]);
void sig_highPassFilter(float signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL]);
void sig_integrate(float signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL]);
void sig_hamming(float signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL]);
void sig_fftMagnitude(float in[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL]);
void sig_RSS(float magnitudes[ACCEL_NUM_MAGNITUDES]);

#define M_2PI 6.28318530718

#define SIG_LOW_PASS_RC (1 / (M_2PI * SIG_LOW_PASS_CUTOFF_FREQ))
#define SIG_LOW_PASS_ALPHA (ACCEL_DELTA_T / (SIG_LOW_PASS_RC + ACCEL_DELTA_T))

#define SIG_HIGH_PASS_RC (1 / (M_2PI * SIG_HIGH_PASS_CUTOFF_FREQ))
#define SIG_HIGH_PASS_ALPHA (SIG_HIGH_PASS_RC / (SIG_HIGH_PASS_RC + ACCEL_DELTA_T))

static float hammingLookup[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL / 2];

static float signalTempBuf[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL];

static float evenReal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL];
static float evenImag[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL];
static float oddReal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL];
static float oddImag[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL];

static void fftRec(float *real, float *imag, uint16_t size, uint16_t memOffset);

void sig_init()
{
    for (uint16_t k = 0; k < ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL / 2; k++)
        hammingLookup[k] = 0.54 - 0.46 * cos(M_2PI * (float)k / (float)(ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL - 1));
}

void sig_center(float signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL])
{
    float avg = 0;
    for (uint16_t i = 0; i < ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL; i++)
        avg += signal[i];
    avg /= (float)ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL;

    for (uint16_t i = 0; i < ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL; i++)
        signal[i] -= avg;
}

void sig_lowPassFilter(float signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL])
{
    for (uint16_t i = 1; i < ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL; i++)
        signal[i] = signal[i - 1] + SIG_LOW_PASS_ALPHA * (signal[i] - signal[i - 1]);
}

void sig_highPassFilter(float signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL])
{
    signalTempBuf[0] = signal[0];
    for (uint16_t i = 1; i < ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL; i++)
    {
        signalTempBuf[i] = signal[i];
        signal[i] = SIG_HIGH_PASS_ALPHA * (signal[i - 1] + signalTempBuf[i] - signalTempBuf[i - 1]);
    }
}

void sig_integrate(float signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL])
{
    for (uint16_t i = 1; i < ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL; i++)
        signal[i] = signal[i - 1] + signal[i] * (ACCEL_DELAY_BETWEEN_SAMPLES_US / (float)1000000);
}

void sig_hamming(float signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL])
{
    for (uint16_t i = 0; i < ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL / 2; i++)
    {
        signal[i] = signal[i] * hammingLookup[i];
        signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL / 2 + i] = signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL / 2 + i] * hammingLookup[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL / 2 - 1 - i];
    }
}

void sig_fftMagnitude(float signal[ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL])
{
    for (uint16_t i = 0; i < ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL; i++)
        signalTempBuf[i] = 0;

    fftRec(signal, signalTempBuf, ACCEL_TOTAL_SAMPLES_PER_ACCEL_CHANNEL, 0);

    for (uint16_t k = 0; k < ACCEL_NUM_MAGNITUDES; k++)
        signal[k] = sqrt(signal[k] * signal[k] + signalTempBuf[k] * signalTempBuf[k]);
}

void sig_RSS(float magnitudes[ACCEL_NUM_MAGNITUDES])
{
    for (uint16_t i = 0; i < ACCEL_NUM_MAGNITUDES; i++)
        signalTempBuf[i] = magnitudes[i];

    for (uint16_t i = 0; i < ACCEL_NUM_MAGNITUDES; i++)
    {
        magnitudes[i] = 0;
        for (int16_t j = -SIG_RSS_WINDOW_LENGTH / 2; j <= SIG_RSS_WINDOW_LENGTH / 2; j++)
        {
            int16_t idx = i + j / 2;
            if (idx < 0 || idx >= ACCEL_NUM_MAGNITUDES)
                continue;

            magnitudes[i] += signalTempBuf[idx] * signalTempBuf[idx];
        }
        magnitudes[i] = sqrt(magnitudes[i]);
    }
}

static void fftRec(float *real, float *imag, uint16_t size, uint16_t memOffset)
{
    if (size <= 1)
        return;

    for (uint16_t i = 0; i < size / 2; i++)
    {
        evenReal[i + memOffset] = real[i * 2];
        evenImag[i + memOffset] = imag[i * 2];
        oddReal[i + memOffset] = real[i * 2 + 1];
        oddImag[i + memOffset] = imag[i * 2 + 1];
    }

    fftRec(evenReal + memOffset, evenImag + memOffset, size / 2, memOffset + size / 2);
    fftRec(oddReal + memOffset, oddImag + memOffset, size / 2, memOffset + size / 2);

    for (uint16_t k = 0; k < size / 2; k++)
    {
        float ang = -M_2PI * (float)k / (float)size;

        float realCos = cos(ang);
        float imagSin = sin(ang);

        float realTemp = realCos * oddReal[k + memOffset] - imagSin * oddImag[k + memOffset];
        float imagTemp = realCos * oddImag[k + memOffset] + imagSin * oddReal[k + memOffset];

        real[k] = evenReal[k + memOffset] + realTemp;
        imag[k] = evenImag[k + memOffset] + imagTemp;

        real[size / 2 + k] = evenReal[k + memOffset] - realTemp;
        imag[size / 2 + k] = evenImag[k + memOffset] - imagTemp;
    }
}