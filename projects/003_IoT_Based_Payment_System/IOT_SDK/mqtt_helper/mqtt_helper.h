/*
 * mqtt_helper.h
 *
 *  Created on: Jan 8, 2025
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */


#ifndef MQTT_WRAPPER_H
#define MQTT_WRAPPER_H

#include <stdint.h>
#include "esp32_at.h"


mqtt_status_t mqtt_connect( char * thingname, char * host, uint16_t port );
mqtt_status_t mqtt_publish(char *topic,  size_t topic_length, uint8_t *message, size_t message_length);

mqtt_status_t mqtt_subscribe( char * topic,  size_t topic_length);

#endif
