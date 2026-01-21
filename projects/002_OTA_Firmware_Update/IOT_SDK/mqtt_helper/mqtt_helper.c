/*
 * mqtt_helper.h
 *
 *  Created on: Jan 8, 2025
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */


#include "mqtt_helper.h"
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "esp32_at.h"
#include "application_config.h"

static mqtt_connection_status_t mqtt_conn_status = MQTT_DISCONNECTED;


mqtt_status_t mqtt_connect(char *thingname, char *host, uint16_t port) {
    mqtt_connection_info_t connect_info = {0};
    esp32_status_t mqtt_status = ESP32_OK;

    /* Fill user config */
    connect_info.user_config.scheme = MQTT_SCHEME_TLS_BOTH_CERTS;
    connect_info.user_config.p_client_id = thingname;
    connect_info.user_config.p_username = NULL;
    connect_info.user_config.p_password = NULL;
    connect_info.user_config.cert_key_id = 0;
    connect_info.user_config.ca_id = 0;
    connect_info.user_config.path = NULL;

    /* Fill connection config */
    const char *lwt_payload = "offline";
    const char *lwt_topic = "device/status";

    connect_info.conn_config.keep_alive = 60;
    connect_info.conn_config.disable_clean_session = 0;
    connect_info.conn_config.lwt_info.p_payload = lwt_payload;
    connect_info.conn_config.lwt_info.payload_length = strlen(lwt_payload);
    connect_info.conn_config.lwt_info.p_topic = lwt_topic;
    connect_info.conn_config.lwt_info.topic_length = strlen(lwt_topic);
    connect_info.conn_config.lwt_info.qos = MQTT_QOS0;
    connect_info.conn_config.lwt_info.retain = 0;

    /* Fill host and port */
    connect_info.p_host = host;
    connect_info.p_port = port;
    connect_info.reconnect = MQTT_RECONNECT_ENABLE;

    /* Step 1: Configure MQTT user settings */
    mqtt_status = esp32_mqtt_configure(&connect_info);
    if (mqtt_status != ESP32_OK) {
        return false;
    }

    /* Step 2: Connect to MQTT broker */
    mqtt_status = esp32_mqtt_connect_to_broker( &connect_info);
    if (mqtt_status != ESP32_OK) {
        mqtt_conn_status = MQTT_DISCONNECTED;
        return false;
    }

    mqtt_conn_status = MQTT_CONNECTED;
    return true;
}

mqtt_status_t mqtt_publish(char *topic, size_t topic_length, uint8_t *message, size_t message_length) {
    if (!topic || !message || topic_length == 0 || message_length == 0) {
        return false;
    }

    mqtt_publish_t pub_args = {0};
    esp32_status_t status = ESP32_OK;
    pub_args.p_payload = (const char *)message;
    pub_args.payload_length = message_length;
    pub_args.qos = 0;
    pub_args.retain = 0;
    pub_args.p_topic = topic;
    pub_args.topic_length = topic_length;

    status = esp32_mqtt_publish(&pub_args);
    if (status != ESP32_OK) {
        return false;
    }
    return true;
}

mqtt_status_t mqtt_subscribe(char *topic, size_t topic_length) {
    if (!topic || topic_length == 0) {
        return false;
    }

    mqtt_subscribe_t sub_args = {0};
    sub_args.p_topic_filter = topic;
    sub_args.qos = 1;
    sub_args.topic_filter_length = topic_length;

    esp32_status_t status = esp32_mqtt_subscribe(&sub_args);
    if (status != ESP32_OK) {
        return false;
    }
    return true;
}
