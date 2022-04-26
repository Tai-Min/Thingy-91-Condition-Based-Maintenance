#include <zephyr.h>
#include <stdio.h>
#include "../include/leds.h"
#include "../include/accels.h"
#include "../include/pca.h"

#define PRINT_MAGNITUDES
//#define PRINT_PCA

#ifdef PRINT_MAGNITUDES
void printMag(const float *buf)
{
	leds_setTransmitting(true);
	for (uint16_t k = 0; k < ACCEL_NUM_MAGNITUDES; k++)
	{
		if (k < ACCEL_NUM_MAGNITUDES - 1)
			printf("%.3f, ", buf[k]);
		else
			printf("%.3f", buf[k]);
		k_sleep(K_USEC(10));
	}
	printf("\n");
	leds_setTransmitting(false);
}
#endif

#ifdef PRINT_PCA
void printPCA(const float *buf)
{
	leds_setTransmitting(true);
	for (uint16_t k = 0; k < PCA_RESULT_SIZE; k++)
	{
		if (k < PCA_RESULT_SIZE - 1)
			printf("%.3f, ", buf[k]);
		else
			printf("%.3f", buf[k]);
		k_sleep(K_USEC(10));
	}
	printf("\n");
	leds_setTransmitting(false);
}
#endif

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

		if (!accels_sample())
			return;

#ifdef PRINT_MAGNITUDES
		printf("---\n");
		for (uint8_t i = 0; i < ACCEL_NUM_AXES; i++)
			printMag(accels_getVelocityMagnitudes(ACCEL_HIGH_G, i));
#endif

		/*float *mags[PCA_NUM_INPUT_BUFS] = {accels_getVelocityMagnitudes(ACCEL_HIGH_G, ACCEL_X_IDX),
										   accels_getVelocityMagnitudes(ACCEL_HIGH_G, ACCEL_Y_IDX),
										   accels_getVelocityMagnitudes(ACCEL_HIGH_G, ACCEL_Z_IDX)};*/

		//pca(mags);

#ifdef PRINT_PCA
		//printf("---\n");
		//printPCA(pca_getResultBuf());
#endif
	}
}
