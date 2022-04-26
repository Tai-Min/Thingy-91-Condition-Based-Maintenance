
#pragma once

#define PCA_RESULT_SIZE 215

#define PCA_ROWS 1539
#define PCA_COLS PCA_RESULT_SIZE

#define PCA_NUM_INPUT_BUFS 3
#define PCA_SINGLE_BUF_SIZE (PCA_ROWS / PCA_NUM_INPUT_BUFS)

void pca(float *bufs[PCA_NUM_INPUT_BUFS]);
float *pca_getResultBuf();
