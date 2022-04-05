#include "../include/led_alerts.h"
#include <device.h>
#include <zephyr.h>
#include <drivers/pwm.h>
#include <stdio.h>

bool alerts_init();
void alerts_set(enum Alert a);
void alerts_transmitting(bool t);

#define RED 0
#define GREEN 1
#define BLUE 2

#define RED_GPIO 29
#define GREEN_GPIO 30
#define BLUE_GPIO 31

#define THREAD_WAIT_TIME 50

#define PWM_NODE DT_ALIAS(rgb_pwm)
#define PWM_CTRL PWM_NODE
#define PWM_FLAGS DT_PWMS_FLAGS(PWM_NODE)

static const struct device *pwm = DEVICE_DT_GET(PWM_CTRL);
static struct k_mutex mtx;
static enum Alert currAlert = OKAY;
static bool transmitting = false;

bool alerts_init()
{
    if (!device_is_ready(pwm))
    {
        printf("GET PWM failed!\n");
        return false;
    }
    k_mutex_init(&mtx);
    return true;
}

void alerts_set(enum Alert a)
{
    k_mutex_lock(&mtx, K_FOREVER);
    currAlert = a;
    k_mutex_unlock(&mtx);
}

void alerts_transmitting(bool t)
{
    k_mutex_lock(&mtx, K_FOREVER);
    transmitting = t;
    k_mutex_unlock(&mtx);
}

void alerts_update()
{
    static uint8_t yellow_toggler = 0;
    static uint8_t red_toggler = 0;
    static uint8_t blue_toggler = false;

    uint8_t pulses[3];

    while (true)
    {
        k_mutex_lock(&mtx, K_FOREVER);

        switch (currAlert)
        {
        case OKAY:
            pulses[RED] = 0;
            pulses[GREEN] = 100;
            break;
        case WARNING:
            pulses[RED] = yellow_toggler < 12 ? 100 : 0;
            pulses[GREEN] = yellow_toggler < 12 ? 50 : 0;
            break;
        case CRITICAL:
            pulses[RED] = red_toggler < 4 ? 100 : 0;
            pulses[GREEN] = 0;
            break;
        default:
            pulses[RED] = 0;
            pulses[GREEN] = 0;
            break;
        }

        pulses[BLUE] = transmitting && blue_toggler < 2 ? 100 : 0;
        k_mutex_unlock(&mtx);

        pwm_pin_set_usec(pwm, RED_GPIO, 100, pulses[RED], PWM_FLAGS);
        pwm_pin_set_usec(pwm, GREEN_GPIO, 100, pulses[GREEN], PWM_FLAGS);
        pwm_pin_set_usec(pwm, BLUE_GPIO, 100, pulses[BLUE], PWM_FLAGS);

        yellow_toggler++;
        if (yellow_toggler >= 16)
            yellow_toggler = 0;

        red_toggler++;
        if (red_toggler >= 8)
            red_toggler = 0;

        blue_toggler++;
        if (blue_toggler >= 4)
            blue_toggler = 0;

        k_sleep(K_MSEC(THREAD_WAIT_TIME));
    }
}

K_THREAD_DEFINE(alerts_thread, 256, alerts_update, NULL, NULL, NULL, 0, 0, 0);
