
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define PCA_RESULT_SIZE 70

#define PCA_ROWS 387
#define PCA_COLS PCA_RESULT_SIZE

#define PCA_NUM_INPUT_BUFS 3
#define PCA_SINGLE_BUF_SIZE (PCA_ROWS / PCA_NUM_INPUT_BUFS)

void pca(float *bufs[PCA_NUM_INPUT_BUFS]);
float *pca_getResultBuf();

#ifdef __cplusplus
}
#endif