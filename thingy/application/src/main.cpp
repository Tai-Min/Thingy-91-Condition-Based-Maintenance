#include <zephyr.h>
#include <cstdio>

#include "leds.h"
#include "accels.h"
#include "pca.h"
#include "inference.hpp"
#include "mqtt.h"

//#define PRINT_MAGNITUDES
//#define PRINT_PCA
//#define PRINT_INFERENCE

void printBuf(const float *buf, uint16_t size)
{
	leds_setTransmitting(true);
	for (uint16_t k = 0; k < size; k++)
	{
		if (k < size - 1)
			printf("%.3f, ", buf[k]);
		else
			printf("%.3f", buf[k]);
		k_sleep(K_USEC(10));
	}
	printf("\n");
	leds_setTransmitting(false);
}

void main(void)
{
	// Disable buffering.
	setvbuf(stdout, NULL, _IONBF, 0);

	k_sleep(K_MSEC(2000));

	if (!leds_init())
		return;

	leds_setState(OKAY);

	if (!accels_init())
		return;

	while (true)
	{

		k_sleep(K_MSEC(10));

		if (!accels_sample())
			return;

#ifdef PRINT_MAGNITUDES
		printf("---\n");
		for (uint8_t i = 0; i < ACCEL_NUM_AXES; i++)
			printBuf(accels_getVelocityMagnitudes(ACCEL_HIGH_G, i), ACCEL_NUM_MAGNITUDES);
		continue;
#endif

		float *mags[PCA_NUM_INPUT_BUFS] = {accels_getVelocityMagnitudes(ACCEL_HIGH_G, ACCEL_X_IDX),
										   accels_getVelocityMagnitudes(ACCEL_HIGH_G, ACCEL_Y_IDX),
										   accels_getVelocityMagnitudes(ACCEL_HIGH_G, ACCEL_Z_IDX)};

		pca(mags);

#ifdef PRINT_PCA
		printf("---\n");
		printBuf(pca_getResultBuf(), PCA_RESULT_SIZE);
		continue;
#endif

		Classification classification = inference(pca_getResultBuf());

#ifdef PRINT_INFERENCE
		switch (classification)
		{
		case BEARING:
			printf("BEARING\n");
			leds_setState(CRITICAL);
			break;
		case RUNNING:
			printf("RUNNING\n");
			leds_setState(OKAY);
			break;
		case STOPPED:
			printf("STOPPED\n");
			leds_setState(WARNING);
			break;
		case THROTTLE:
			printf("THROTTLE\n");
			leds_setState(CRITICAL);
			break;
		case NOT_SURE:
			printf("NOT_SURE\n");
			leds_setState(WARNING);
			break;
		}
		continue;
#endif

		mqtt_setClassification(classification);
		switch (classification)
		{
		case RUNNING:
			leds_setState(OKAY);
			break;
		case STOPPED:
			leds_setState(WARNING);
			break;
		case NOT_SURE:
			leds_setState(WARNING);
			break;
		case BEARING:
			leds_setState(CRITICAL);
			break;
		case THROTTLE:
			leds_setState(CRITICAL);
			break;
		}
		continue;
	}
}
