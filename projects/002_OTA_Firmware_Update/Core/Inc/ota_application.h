/*
 * ota_application.h
 *
 *  Created on: Mar 25, 2025
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */

#ifndef SRC_OTA_APPLICATION_H_
#define SRC_OTA_APPLICATION_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum {
  OTA_SUCCESS = 0, /* Operation completed without error */

  /* Buffer-building errors */
  OTA_ERR_TOPIC_TRUNC = -1, /* snprintf() or Jobs_StartNext() overflowed topic buffer   */
  OTA_ERR_MSG_TRUNC = -2, /* Jobs_StartNextMsg() overflowed message buffer            */
  OTA_ERR_TOPIC_BUILD = -3, /* Jobs_StartNext() returned failure                        */

  /* Queue / transport errors */
  OTA_ERR_QUEUE_FULL = -4,/* xQueueSend() timed out or queue was full                 */
  OTA_ERR_SUBSCRIBE = -5,/* MQTT subscribe failed (optional, if you detect it later) */
  OTA_ERR_PUBLISH = -6,/* MQTT publish failed (optional)                           */

  /* Catch-all */
  OTA_ERR_UNKNOWN = -7/* Any other unclassified error                             */
} OTA_Status_t;

#define OTA_CLIENT_TOKEN                    "test"

OTA_Status_t ota_start(void);
void run_application();

bool ota_handle_incoming_mqtt_message(char *topic, size_t topicLength,
    char *message, size_t messageLength);

#endif /* SRC_OTA_APPLICATION_H_ */
