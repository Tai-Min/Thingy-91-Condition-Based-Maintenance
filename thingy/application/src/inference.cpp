#include "inference.hpp"
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "edge-impulse-sdk/dsp/numpy.hpp"

Classification inference(float buf[PCA_RESULT_SIZE]);

static signal_t featureSig;
static uint8_t lastBuf = 0;
static float bufs[INFERENCE_WINDOW_SIZE][PCA_RESULT_SIZE];
static ei_impulse_result_t classificationResult = {0};

static int loadData(size_t offset, size_t length, float *out_ptr);

Classification inference(float buf[PCA_RESULT_SIZE])
{
    for (uint16_t i = 0; i < PCA_RESULT_SIZE; i++)
        bufs[lastBuf][i] = buf[i];

    featureSig.total_length = PCA_RESULT_SIZE * INFERENCE_WINDOW_SIZE;
    featureSig.get_data = &loadData;

    run_classifier(&featureSig, &classificationResult);

    lastBuf++;
    if (lastBuf >= INFERENCE_WINDOW_SIZE)
        lastBuf -= INFERENCE_WINDOW_SIZE;

    if (classificationResult.classification[BEARING].value > 0.8)
        return BEARING;

    if (classificationResult.classification[RUNNING].value > 0.8)
        return RUNNING;

    if (classificationResult.classification[STOPPED].value > 0.8)
        return STOPPED;

    if (classificationResult.classification[THROTTLE].value > 0.8)
        return THROTTLE;

    return NOT_SURE;
}

static int loadData(size_t offset, size_t length, float *out_ptr)
{
    uint8_t firstBuf = lastBuf;

    for (size_t i = 0; i < length; i++)
    {
        uint8_t currBuf = firstBuf + offset / PCA_RESULT_SIZE + i / PCA_RESULT_SIZE;

        if (currBuf >= INFERENCE_WINDOW_SIZE)
            currBuf -= INFERENCE_WINDOW_SIZE;

        out_ptr[i] = bufs[currBuf][i % PCA_RESULT_SIZE];
    }

    /*for(uint8_t currBuf = lastBuf; currBuf < lastBuf + INFERENCE_AVG_WINDOW_SIZE; currBuf++){
        uint8_t currBufFixed = currBuf;
        if (currBufFixed >= INFERENCE_AVG_WINDOW_SIZE)
            currBufFixed -= INFERENCE_AVG_WINDOW_SIZE;

        //size_t off =

        //memcpy(out_ptr, bufs[currBuf] + bufOffset, length * sizeof(float));
    }*/

    return 0;
}