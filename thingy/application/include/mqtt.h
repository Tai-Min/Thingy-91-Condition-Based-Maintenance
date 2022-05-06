#pragma once

#include "inference.hpp"

#ifdef __cplusplus
extern "C"
{
#endif

#define MQTT_BUFFER_SIZE 512
#define MQTT_PUB_TOPIC "sample_motor/maintenance"
#define MQTT_CLIENT_ID "sample_motor_cbs"
#define MQTT_BROKER_HOSTNAME "mqtt.eclipseprojects.io"
#define MQTT_BROKER_PORT 1883
#define MQTT_FAULT_WAIT_TIME_SECONDS 600

    void mqtt_setClassification(enum Classification c);

#ifdef __cplusplus
}
#endif