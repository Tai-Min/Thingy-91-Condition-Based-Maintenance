#pragma once

#define PCA_RESULT_SIZE 19

#define PCA_ROWS 54
#define PCA_COLS PCA_RESULT_SIZE

#define PCA_NUM_INPUT_BUFS 6
#define PCA_SINGLE_BUF_SIZE (PCA_ROWS / PCA_NUM_INPUT_BUFS)

void pca_doTransform(float *bufs[PCA_NUM_INPUT_BUFS]);
float *pca_getResultBuf();