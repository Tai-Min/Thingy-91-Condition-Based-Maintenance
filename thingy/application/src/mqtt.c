#include "mqtt.h"

#include <zephyr.h>
#include <stdint.h>
#include <random/rand32.h>
#include <net/mqtt.h>
#include <net/socket.h>
#include <nrf_modem_at.h>
#include <modem/lte_lc.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

void mqtt_setClassification(enum Classification c);

#define MQTT_STOPPED_MSG "Motor stopped!"
#define MQTT_BEARING_MSG "Bearing failure!"
#define MQTT_THROTTLE_MSG "Motor throttles!"

static enum Classification classif = NOT_SURE;
static uint8_t rx_buffer[MQTT_BUFFER_SIZE];
static uint8_t tx_buffer[MQTT_BUFFER_SIZE];

static struct mqtt_client client;
static struct sockaddr_storage broker;
static struct pollfd fds;

static int data_publish(uint8_t *data, size_t len);
void mqtt_evt_handler(struct mqtt_client *const c, const struct mqtt_evt *evt);
static int broker_init();
static const char *client_id_get();
static int modem_configure();
static int client_init();
static void poller();
static void statePublisher();

void mqtt_setClassification(enum Classification c)
{
    classif = c;
}

static int data_publish(uint8_t *data, size_t len)
{
    struct mqtt_publish_param param;

    param.message.topic.qos = MQTT_QOS_1_AT_LEAST_ONCE;
    param.message.topic.topic.utf8 = MQTT_PUB_TOPIC;
    param.message.topic.topic.size = strlen(MQTT_PUB_TOPIC);
    param.message.payload.data = data;
    param.message.payload.len = len;
    param.message_id = sys_rand32_get();
    param.dup_flag = 0;
    param.retain_flag = 0;

    int res = mqtt_publish(&client, &param);
    return res;
}

void mqtt_evt_handler(struct mqtt_client *const c, const struct mqtt_evt *evt)
{
    switch (evt->type)
    {
    case MQTT_EVT_CONNACK:
        break;
    case MQTT_EVT_DISCONNECT:
        break;
    case MQTT_EVT_PUBACK:
        break;
    default:
        break;
    }
}

static int broker_init()
{
    int err;
    struct addrinfo *result;
    struct addrinfo *addr;
    struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM};

    err = getaddrinfo(MQTT_BROKER_HOSTNAME, NULL, &hints, &result);
    if (err)
    {
        return -ECHILD;
    }

    addr = result;

    /* Look for address of the broker. */
    while (addr != NULL)
    {
        /* IPv4 Address. */
        if (addr->ai_addrlen == sizeof(struct sockaddr_in))
        {
            struct sockaddr_in *broker4 = ((struct sockaddr_in *)&broker);
            char ipv4_addr[NET_IPV4_ADDR_LEN];

            broker4->sin_addr.s_addr = ((struct sockaddr_in *)addr->ai_addr)->sin_addr.s_addr;
            broker4->sin_family = AF_INET;
            broker4->sin_port = htons(MQTT_BROKER_PORT);

            inet_ntop(AF_INET, &broker4->sin_addr.s_addr,
                      ipv4_addr, sizeof(ipv4_addr));

            break;
        }

        addr = addr->ai_next;
    }

    /* Free the address. */
    freeaddrinfo(result);

    return err;
}

#define IMEI_LEN 15
#define CGSN_RESPONSE_LENGTH (IMEI_LEN + 6 + 1) /* Add 6 for \r\nOK\r\n and 1 for \0 */
#define CLIENT_ID_LEN sizeof("nrf-") + IMEI_LEN

static const char *client_id_get()
{
    static char client_id[MAX(sizeof(MQTT_CLIENT_ID), CLIENT_ID_LEN)];

    if (strlen(MQTT_CLIENT_ID) > 0)
    {
        snprintf(client_id, sizeof(client_id), "%s",
                 MQTT_CLIENT_ID);
        return client_id;
    }

    char imei_buf[CGSN_RESPONSE_LENGTH + 1];
    int err;

    err = nrf_modem_at_cmd(imei_buf, sizeof(imei_buf), "AT+CGSN");
    if (err)
    {
        return client_id;
    }

    imei_buf[IMEI_LEN] = '\0';

    snprintf(client_id, sizeof(client_id), "nrf-%.*s", IMEI_LEN, imei_buf);

    return client_id;
}

static int modem_configure()
{
    lte_lc_psm_req(false);
    lte_lc_edrx_req(false);

    if (!(IS_ENABLED(CONFIG_LTE_AUTO_INIT_AND_CONNECT)))
    {
        int err;

        err = lte_lc_init_and_connect();
        if (err)
            return err;
    }
    return 0;
}

static int client_init()
{
    int err;

    mqtt_client_init(&client);

    err = broker_init();
    if (err)
        return err;

    /* MQTT client configuration */
    client.broker = &broker;
    client.evt_cb = mqtt_evt_handler;
    client.client_id.utf8 = (const uint8_t *)client_id_get();
    client.client_id.size = strlen((const char *)client.client_id.utf8);
    client.password = NULL;
    client.user_name = NULL;
    client.protocol_version = MQTT_VERSION_3_1_1;

    /* MQTT buffers configuration */
    client.rx_buf = rx_buffer;
    client.rx_buf_size = sizeof(rx_buffer);
    client.tx_buf = tx_buffer;
    client.tx_buf_size = sizeof(tx_buffer);

    /* MQTT transport configuration */
    client.transport.type = MQTT_TRANSPORT_NON_SECURE;

    return err;
}

static void poller()
{
    int err;

    do
    {
        err = modem_configure();
        if (err)
        {
            printf("Modem configure\n");
            k_sleep(K_SECONDS(1));
        }

    } while (err);

    do
    {
        err = client_init();
        if (err)
        {
            printf("Client init\n");
            k_sleep(K_SECONDS(1));
        }

    } while (err);

    while (true)
    {
        err = mqtt_connect(&client);
        if (err)
        {
            printf("MQTT connect\n");
            continue;
        }

        fds.fd = client.transport.tcp.sock;
        fds.events = POLLIN;

        while (true)
        {
            err = poll(&fds, 1, mqtt_keepalive_time_left(&client));
            if (err < 0)
            {
                printf("Poll\n");
                break;
            }

            err = mqtt_live(&client);
            if ((err != 0) && (err != -EAGAIN))
            {
                printf("MQTT live\n");
                break;
            }

            if ((fds.revents & POLLIN) == POLLIN)
            {
                err = mqtt_input(&client);
                if (err != 0)
                {
                    printf("MQTT input\n");
                    break;
                }
            }

            if ((fds.revents & POLLERR) == POLLERR || (fds.revents & POLLNVAL) == POLLNVAL)
            {
                printf("fds\n");
                break;
            }

            k_sleep(K_SECONDS(10));
        }

        err = mqtt_disconnect(&client);

        k_sleep(K_SECONDS(1));
    }
}

static void statePublisher()
{
    while (true)
    {
        switch (classif)
        {
        case RUNNING:
        case NOT_SURE:
            break;
        case STOPPED:
            data_publish((uint8_t *)MQTT_STOPPED_MSG, sizeof(MQTT_STOPPED_MSG) - 1);
            k_sleep(K_SECONDS(MQTT_FAULT_WAIT_TIME_SECONDS));
            break;
        case BEARING:
            data_publish((uint8_t *)MQTT_BEARING_MSG, sizeof(MQTT_BEARING_MSG) - 1);
            k_sleep(K_SECONDS(MQTT_FAULT_WAIT_TIME_SECONDS));
            break;
        case THROTTLE:
            data_publish((uint8_t *)MQTT_THROTTLE_MSG, sizeof(MQTT_THROTTLE_MSG) - 1);
            k_sleep(K_SECONDS(MQTT_FAULT_WAIT_TIME_SECONDS));
            break;
        }

        k_sleep(K_MSEC(10));
    }
}

K_THREAD_DEFINE(mqtt_poller_thread, 4096, poller, NULL, NULL, NULL, 0, 0, 0);
K_THREAD_DEFINE(mqtt_publisher_thread, 2048, statePublisher, NULL, NULL, NULL, 0, 0, 0);