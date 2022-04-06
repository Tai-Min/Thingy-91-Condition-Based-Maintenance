from matplotlib import pyplot as plt
import pandas as pd
import numpy as np
import re
import sys
np.set_printoptions(threshold=sys.maxsize)

# Data loading.
df = pd.read_csv("samples.csv")
samples = df.to_numpy()
num_cols = samples.shape[1]
num_samples = samples.shape[0]

# Normalization.
means = np.mean(samples, axis=0)
deviations = np.std(samples, axis=0)

normalized_samples = (samples - means) / deviations

# Correlation matrix.
R = 1 / (num_samples - 1) * np.transpose(normalized_samples) @ normalized_samples

# Eigen values and vectors.
eigen_vals, eigen_vecs = np.linalg.eig(R)

# Sort them from highest values.
idx = eigen_vals.argsort()[::-1]
eigen_vals = eigen_vals[idx]
eigen_vecs = eigen_vecs[:, idx]

# Zero columns that correspond to eig values that are less than 1 (those variables are worth less than 1 variable).
eigen_vecs_prim = eigen_vecs.copy()
eigen_vecs_prim[:, eigen_vals < 1] = 0

# Remove zeroed columns.
eigen_vecs_prim_reduced = eigen_vecs_prim[:, ~np.all(
    eigen_vecs_prim == 0, axis=0)]

# Multiplying by this will scale eigen vectors to range of int8.
eigen_vecs_scaler_int = 127.0 / np.max(np.abs(eigen_vecs_prim_reduced))
eigen_vecs_reduced_scaled = (eigen_vecs_prim_reduced * eigen_vecs_scaler_int).astype(int)

# Multiplying scaled int8 eigen vectors will scale them back to original range (with some error).
eigen_vecs_scaler_float = 1 / eigen_vecs_scaler_int
eigen_vecs_reduced_rescaled = eigen_vecs_reduced_scaled * eigen_vecs_scaler_float

# Loss
rescaling_error = 100 - (eigen_vecs_reduced_rescaled / eigen_vecs_prim_reduced * 100)

print(rescaling_error)

def to_c_like_array(arr):
    res = "{"
    for col in range(arr.shape[0]):
        res += str(arr[col]) + ("" if col == arr.shape[0] - 1 else ", ")
    res += "}"
    return res


def to_c_like_matrix(arr):
    res = "{"
    for row in range(arr.shape[0]):
        res += "{"
        for col in range(arr.shape[1]):
            res += str(arr[row, col]) + ("" if col ==
                                         arr.shape[1] - 1 else ", ")
        res += "}\n" if row == arr.shape[0] - 1 else "},\n"
    res += "}"
    return res


header = """
#pragma once

#define PCA_RESULT_SIZE {0}

#define PCA_ROWS {1}
#define PCA_COLS PCA_RESULT_SIZE

#define PCA_NUM_INPUT_BUFS 6
#define PCA_SINGLE_BUF_SIZE (PCA_ROWS / PCA_NUM_INPUT_BUFS)

void pca_doTransform(float *bufs[PCA_NUM_INPUT_BUFS]);
float *pca_getResultBuf();
""" .format(eigen_vecs_reduced_scaled.shape[1],
            eigen_vecs_reduced_scaled.shape[0])
print(header)

source = """
#include "../include/pca.h"
#include <stdint.h>

void pca_doTransform(float *bufs[PCA_NUM_INPUT_BUFS]);
float *pca_getResultBuf();

static float resultBuffer[PCA_RESULT_SIZE] = {{0}};

static const float means[PCA_ROWS] = {0};

static const float deviations[PCA_ROWS] = {1};

static const int8_t eigenVecReduced[PCA_ROWS][PCA_COLS] = {2};
static const float eigenScaler = {3};

static void normalize(float *bufs[PCA_NUM_INPUT_BUFS]);

void pca_doTransform(float *bufs[PCA_NUM_INPUT_BUFS])
{{
    normalize(bufs);

    for (int col = 0; col < PCA_COLS; col++)
    {{
        resultBuffer[col] = 0;
        int idxCntr = 0;
        for (int row = 0; row < PCA_ROWS; row++)
        {{
            resultBuffer[col] += bufs[idxCntr / PCA_SINGLE_BUF_SIZE][idxCntr % PCA_SINGLE_BUF_SIZE] * (float)eigenVecReduced[row][col] * eigenScaler;
            idxCntr++;
        }}
    }}
}}

float *pca_getResultBuf()
{{
    return resultBuffer;
}}

static void normalize(float *bufs[PCA_NUM_INPUT_BUFS])
{{
    int idxCntr = 0;
    for (int buf = 0; buf < PCA_NUM_INPUT_BUFS; buf++)
    {{
        bufs[buf][idxCntr % PCA_SINGLE_BUF_SIZE] = (bufs[buf][idxCntr % PCA_SINGLE_BUF_SIZE] - means[idxCntr]) / deviations[idxCntr];
        idxCntr++;
    }}
}}
""".format(to_c_like_array(means),
       to_c_like_array(deviations),
       to_c_like_matrix(eigen_vecs_reduced_scaled),
       eigen_vecs_scaler_float)
print(source)
