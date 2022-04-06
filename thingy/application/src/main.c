#include <zephyr.h>
#include <stdio.h>
#include "../include/led_alerts.h"
#include "../include/accels.h"
#include "../include/pca.h"

// Select only one!
#define PCA_DATASET_COLLECTOR // Collect magnitude data to create dataset for PCA model.
//#define NEURAL_DATASET_COLLECTOR // Collect PCA processed data to create dataset for neural network.
//#define NORMAL_OP // Perform condition check.

#ifdef PCA_DATASET_COLLECTOR
void printMag(const float *buf)
{
	for (uint8_t k = 0; k < ACCEL_NUM_MAGNITUDES; k++)
	{
		if (k < ACCEL_NUM_MAGNITUDES - 1)
			printf("%.3f, ", buf[k]);
		else
			printf("%.3f", buf[k]);
	}
	printf("\n");
}
#endif

void mainHandler()
{
}

void main(void)
{
	// Disable buffering.
	setvbuf(stdout, NULL, _IONBF, 0);

	k_sleep(K_MSEC(2000));

	if (!alerts_init())
		return;

	if (!accels_init())
		return;

	while (true)
	{

#ifdef PCA_DATASET_COLLECTOR
		if (!accels_sample())
			return;

		printf("---\n");

		printMag(accels_getMagnitudes(ACCEL_LOW_POWER, ACCEL_X_IDX));
		printMag(accels_getMagnitudes(ACCEL_LOW_POWER, ACCEL_Y_IDX));
		printMag(accels_getMagnitudes(ACCEL_LOW_POWER, ACCEL_Z_IDX));

		printMag(accels_getMagnitudes(ACCEL_HIGH_G, ACCEL_X_IDX));
		printMag(accels_getMagnitudes(ACCEL_HIGH_G, ACCEL_Y_IDX));
		printMag(accels_getMagnitudes(ACCEL_HIGH_G, ACCEL_Z_IDX));

		float *mags[PCA_NUM_INPUT_BUFS] = {accels_getMagnitudes(ACCEL_LOW_POWER, ACCEL_X_IDX), accels_getMagnitudes(ACCEL_LOW_POWER, ACCEL_Y_IDX),
										   accels_getMagnitudes(ACCEL_LOW_POWER, ACCEL_Z_IDX), accels_getMagnitudes(ACCEL_HIGH_G, ACCEL_X_IDX),
										   accels_getMagnitudes(ACCEL_HIGH_G, ACCEL_Y_IDX), accels_getMagnitudes(ACCEL_HIGH_G, ACCEL_Z_IDX)};
		pca_doTransform(mags);
		float *pca = pca_getResultBuf();

		for (int i = 0; i < PCA_RESULT_SIZE; i++)
			printf("%f, ", pca[i]);
#endif
	}
}
