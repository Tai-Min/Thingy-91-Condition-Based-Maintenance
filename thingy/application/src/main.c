#include <zephyr.h>
#include <stdio.h>
#include "../include/led_alerts.h"
#include "../include/sensors.h"

// Select only one!
//#define PCA_DATASET_COLLECTOR // Collect magnitude data to create dataset for PCA model.
//#define NEURAL_DATASET_COLLECTOR // Collect PCA processed data to create dataset for neural network.
//#define NORMAL_OP // Perform condition check.

#ifdef PCA_DATASET_COLLECTOR
void printMag(const float *buf)
{
	for (uint8_t k = 0; k < NUM_MAGNITUDES; k++)
	{
		if (k < NUM_MAGNITUDES - 1)
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

	if (!sensors_init())
		return;

	while (true)
	{

#ifdef PCA_DATASET_COLLECTOR
		int start = k_uptime_get();
		if (!sensors_sample())
			return;

		printf("---\n");

		printMag(sensors_getMagnitudes(LOW_POWER, X_IDX));
		printMag(sensors_getMagnitudes(LOW_POWER, Y_IDX));
		printMag(sensors_getMagnitudes(LOW_POWER, Z_IDX));

		printMag(sensors_getMagnitudes(HIGH_G, X_IDX));
		printMag(sensors_getMagnitudes(HIGH_G, Y_IDX));
		printMag(sensors_getMagnitudes(HIGH_G, Z_IDX));
		printf("Exec time: %d\n", k_uptime_delta(start));
#endif
	}
}
