#pragma once

#include "pca.h"

#define INFERENCE_WINDOW_SIZE 10

enum Classification
{
    BEARING = 0,
    RUNNING,
    STOPPED,
    THROTTLE,
    NOT_SURE
};

enum Classification inference(float buf[PCA_RESULT_SIZE]);
