 /*
 * esp32_at.c
 *
 *  Created on: Dec 27, 2024
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */

#include "esp32_at.h"
#include "esp32_at_io.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char at_cmd [ MAX_AT_CMD_SIZE ];
static char rx_buffer [ MAX_BUFFER_SIZE ];

/* Private function prototypes -----------------------------------------------*/
static esp32_status_t run_at_cmd(uint8_t *cmd, uint32_t length,
                                  const uint8_t *token);
static esp32_status_t recv_data(uint8_t *buffer, uint32_t length,
                                uint32_t *ret_length);
static esp32_status_t recv_mqtt_data( mqtt_receive_t *p_receive_info ) ;
/* Private functions ---------------------------------------------------------*/

/**
 * @brief   Initializes the ESP32 module.
 *
 *          This function sets up the communication interface (IO) between
 *          the MCU and the ESP32 module. It then verifies the module's
 *          functionality by sending basic AT commands.
 *
 *          If the module responds with "OK", initialization is successful.
 *          Otherwise, an error is returned.
 *
 * @param   None
 * @retval  ESP32_OK on success, ESP32_ERROR on failure.
 */
esp32_status_t esp32_init(void) {
  esp32_status_t ret = ESP32_OK;

  /* Configuration the IO low layer */
  if ( esp32_io_init() < 0 ) {
    return ESP32_ERROR;
  }

#ifndef OTA_APP
  /* Switch off the echo mode */
  /* Construct the command */
  memset(at_cmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)at_cmd, "ATE%d%s", ESP32_ECHO_OFF, AT_CMD_TERMINATOR );

  /* Send the command */
  ret = run_at_cmd((uint8_t *)at_cmd, strlen((char *)at_cmd),
                    (uint8_t *)AT_OK_STRING);

  /* Exit in case of error */
  if ( ret != ESP32_OK ) {
    return ESP32_ERROR;
  }

  /* Setup the module in station mode*/
  /* Construct the command */
  memset(at_cmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)at_cmd, "AT+CWMODE=%d%s", ESP32_STATION_MODE , AT_CMD_TERMINATOR );

  /* Send the command */
  ret = run_at_cmd((uint8_t *)at_cmd, strlen((char *)at_cmd),
                    (uint8_t *)AT_OK_STRING);
#endif
  return ret;
}

/**
  * @brief   Deinitialize the esp module.
  * @details Restarts the module  and stop the IO. AT command can't be executed
             unless the module is reinitialized.
  * @param   None
  * @retval  ESP32_OK on success, ESP32_ERROR otherwise.
  */
esp32_status_t esp32_deinit(void) {
  esp32_status_t ret;

  /* Construct the command */
  sprintf((char *)at_cmd, "AT+RST%s", AT_CMD_TERMINATOR );

  /* Send the command */
  ret = run_at_cmd((uint8_t *)at_cmd, strlen((char *)at_cmd),
                    (uint8_t *)AT_OK_STRING);

  /* Free resources used by the module */
  esp32_io_deinit();

  return ret;
}

/**
 * @brief  Restarts the esp module.
 * @param  None
 * @retval ESP32_OK on success, ESP32_ERROR otherwise.
 */
esp32_status_t esp32_reset(void) {
  esp32_status_t ret;

  /* Construct the command */
  memset(at_cmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)at_cmd, "AT+RST%s", AT_CMD_TERMINATOR );

  /* Send the command */
  ret = run_at_cmd((uint8_t *)at_cmd, strlen((char *)at_cmd),
                    (uint8_t *)AT_OK_STRING);

  return ret;
}

/**
 * @brief  Join an Access point.
 * @param  ssid: the access point id.
 * @param  password the Access point password.
 * @retval returns esp_AT_COMMAND_OK on success and esp_AT_COMMAND_ERROR
 * otherwise.
 */
esp32_status_t esp32_join_ap(uint8_t *ssid, uint8_t *password) {
  esp32_status_t ret;

  /* List all the available Access points first
   then check whether the specified 'ssid' exists among them or not.*/
  memset(at_cmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)at_cmd, "AT+CWJAP=\"%s\",\"%s\"%s", ssid, password, AT_CMD_TERMINATOR);

  /* Send the command */
  ret = run_at_cmd((uint8_t *)at_cmd, strlen((char *)at_cmd),
                    (uint8_t *)AT_OK_STRING);

  return ret;
}

/**
 * @brief  Quit an Access point if any.
 * @param  None
 * @retval returns esp_AT_COMMAND_OK on success and esp_AT_COMMAND_ERROR
 * otherwise.
 */
esp32_status_t esp32_quit_ap(void) {
  esp32_status_t ret;

  /* Construct the CWQAP command */
  memset(at_cmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)at_cmd, "AT+CWQAP%s", AT_CMD_TERMINATOR );

  /* Send the command */
  ret = run_at_cmd((uint8_t *)at_cmd, strlen((char *)at_cmd),
                    (uint8_t *)AT_OK_STRING);

  return ret;
}

/**
  * @brief  Get the IP address for the esp in Station mode.
  * @param  mode: a esp_modeTypeDef to choose the Station or AccessPoint mode.
                 only the Station mode is supported.
  * @param  ip_address buffer to contain the IP address.
  * @retval returns ESP32_OK on success and ESP32_ERROR otherwise
  */
esp32_status_t esp32_get_ip(esp32_cwmode_t mode, uint8_t *ip_address) {
  esp32_status_t ret = ESP32_OK;
  char *token, *temp;

  /* Initialize the IP address and command fields */
  strcpy((char *)ip_address, "0.0.0.0");
  memset(at_cmd, '\0', MAX_AT_CMD_SIZE);

  /* Construct the CIFSR command */
  sprintf((char *)at_cmd, "AT+CIFSR%s", AT_CMD_TERMINATOR);

  /* Send the CIFSR command */
  ret = run_at_cmd((uint8_t *)at_cmd, strlen((char *)at_cmd),
                    (uint8_t *)AT_OK_STRING);

  /* If ESP32_OK is returned it means the IP Adress inside the rx_buffer
     has already been read */
  if ( ret == ESP32_OK ) {
    /* The ip_address for the Station mode is returned in the format
     ' STAIP,"ip_address" '
      look for the token "STAIP," , then read the ip address located
      between two double quotes */
    token = strstr((char *)rx_buffer, "STAIP,");
    token += 7;

    temp = strstr(token, "\"");
    *temp = '\0';

    /* Get the IP address value */
    strcpy((char *)ip_address, token);
  }

  return ret;
}

/**
 * @brief  Establish a network connection.
 * @param  Connection_info a pointer to a esp_ConnectionInfoTypeDef struct
 * containing the connection info.
 * @retval returns esp_AT_COMMAND_OK on success and esp_AT_COMMAND_ERROR
 * otherwise.
 */
esp32_status_t esp32_establish_connection(const esp32_connection_info_t *connection_info) {
  esp32_status_t ret;

  /* Check the connection mode */
  if ( connection_info->is_server ) {
    /* Server mode not supported for this version yet */
    return ESP32_ERROR;
  }

  /* Construct the CIPSTART command */
  memset(at_cmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)at_cmd, "AT+CIPSTART=\"TCP\",\"%s\",%lu%s",
          (char *)connection_info->ip_address, connection_info->port, AT_CMD_TERMINATOR );

  /* Send the CIPSTART command */
  ret = run_at_cmd((uint8_t *)at_cmd, strlen((char *)at_cmd),
                    (uint8_t *)AT_CONNECT_STRING);

  return ret;
}

/**
 * @brief   Close a network connection.
 * @details Use the ALL_CONNECTION_ID to close all connections.
 * @param   Channel_id the channel ID of the connection to close.
 * @retval  returns esp_AT_COMMAND_OK on success and esp_AT_COMMAND_ERROR
 * otherwise.
 */
esp32_status_t esp32_close_connection(const uint8_t channel_id) {
  /* Working with a single connection, no channel_id is required */
  esp32_status_t ret;

  /* Construct the CIPCLOSE command */
  memset(at_cmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)at_cmd, "AT+CIPCLOSE%s", AT_CMD_TERMINATOR );

  /* Send the CIPCLOSE command */
  ret = run_at_cmd((uint8_t *)at_cmd, strlen((char *)at_cmd),
                    (uint8_t *)AT_OK_STRING);

  return ret;
}

/* === Added Functions for AWS IoT MQTT and SNTP Commands === */

/**
 * @brief  Configure the SNTP client with predefined NTP servers.
 * @param  utc_offset: Time offset from UTC in hours.
 * @retval ESP32_OK on success, ESP32_ERROR otherwise.
 */
esp32_status_t esp32_config_sntp(int utc_offset) {
  esp32_status_t ret;

  const char *ntp_server1 = "pool.ntp.org";
  const char *ntp_server2 = "time.google.com";

  memset(at_cmd, '\0', MAX_AT_CMD_SIZE);

  // Construct AT command with all servers (ESP32 AT FW supports up to 3 servers)
  sprintf((char *)at_cmd,
          "AT+CIPSNTPCFG=1,%d,\"%s\",\"%s\"%s",
          utc_offset,
          ntp_server1,
          ntp_server2,
          AT_CMD_TERMINATOR);

  // Send the command
  ret = run_at_cmd((uint8_t *)at_cmd, strlen((char *)at_cmd), (uint8_t *)AT_OK_STRING);

  return ret;
}

/**
 * @brief  Query the SNTP time from the module.
 * @retval ESP32_OK on success, ESP32_ERROR otherwise.
 */
esp32_status_t esp32_get_sntp_time(sntp_time_t *time_data) {
  const int max_retries = 10, retry_delay_ms = 1000;
  esp32_status_t ret = ESP32_ERROR;

  memset(at_cmd, 0, MAX_AT_CMD_SIZE);
  snprintf((char *)at_cmd, sizeof(at_cmd), "AT+CIPSNTPTIME?%s", AT_CMD_TERMINATOR);

  for (int retries = 0; retries < max_retries; retries++) {
    HAL_Delay(retry_delay_ms);
    if (run_at_cmd((uint8_t *)at_cmd, strlen((char *)at_cmd), (uint8_t *)AT_OK_STRING) != ESP32_OK) {
      continue;
    }

    char *p_sntp_time = strstr(rx_buffer, "+CIPSNTPTIME:");

    // Extract SNTP time from the response
    if (!p_sntp_time || sscanf(p_sntp_time, "+CIPSNTPTIME:%3s %3s %d %d:%d:%d %d",
        time_data->day, time_data->month, &time_data->date,
        &time_data->hour, &time_data->min, &time_data->sec,
        &time_data->year) != 7){
        continue;
    }

    // Check if time is valid
    if (time_data->year != 1970) {
      return ESP32_OK;
    }
  }

  return ret;
}

/**
 * @brief  Configure MQTT client user settings.
 * @param  p_conn_info: Pointer to MQTT connection info structure.
 * @retval ESP32_OK on success, appropriate error code otherwise.
 */
esp32_status_t esp32_mqtt_configure(const mqtt_connection_info_t *p_conn_info) {
    esp32_status_t ret;

    /* Validate input parameters */
    if (p_conn_info == NULL || p_conn_info->p_host == NULL || p_conn_info->p_port == 0) {
        return ESP32_ERROR;
    }

    /* Configure MQTT User Info (Client ID, Auth, Certs) */
    memset(at_cmd, '\0', MAX_AT_CMD_SIZE);
    snprintf((char *)at_cmd, MAX_AT_CMD_SIZE,
             "AT+MQTTUSERCFG=0,%d,\"%s\",\"%s\",\"%s\",%d,%d,\"%s\"%s",
             p_conn_info->user_config.scheme,
             p_conn_info->user_config.p_client_id ? p_conn_info->user_config.p_client_id : "",
             p_conn_info->user_config.p_username ? p_conn_info->user_config.p_username : "",
             p_conn_info->user_config.p_password ? p_conn_info->user_config.p_password : "",
             p_conn_info->user_config.cert_key_id,
             p_conn_info->user_config.ca_id,
             p_conn_info->user_config.path ? p_conn_info->user_config.path : "",
             AT_CMD_TERMINATOR);

    ret = run_at_cmd((uint8_t *)at_cmd, strlen((char *)at_cmd), (uint8_t *)AT_OK_STRING);

    if (ret != ESP32_OK) {
      return ret;
    }

    /* Configure MQTT Connection Info (e.g., KeepAlive, CleanSession, LWT) */
    memset(at_cmd, '\0', MAX_AT_CMD_SIZE);
    snprintf((char *)at_cmd, MAX_AT_CMD_SIZE,
             "AT+MQTTCONNCFG=0,%d,%d,\"%s\",\"%s\",%d,%d%s",
             p_conn_info->conn_config.keep_alive,
             p_conn_info->conn_config.disable_clean_session,
             p_conn_info->conn_config.lwt_info.p_topic ? p_conn_info->conn_config.lwt_info.p_topic : "",
             (const char *)p_conn_info->conn_config.lwt_info.p_payload ? (const char *)p_conn_info->conn_config.lwt_info.p_payload : "",
             p_conn_info->conn_config.lwt_info.qos,
             p_conn_info->conn_config.lwt_info.retain,
             AT_CMD_TERMINATOR);

    ret = run_at_cmd((uint8_t *)at_cmd, strlen((char *)at_cmd), (uint8_t *)AT_OK_STRING);

    return ret;
}

/**
 * @brief  Connect to MQTT broker.
 * @param  p_handler: Pointer to MQTT handler structure.
 * @param  p_conn_info: Pointer to MQTT connection info structure.
 * @retval ESP32_OK on success, appropriate error code otherwise.
 */
esp32_status_t esp32_mqtt_connect_to_broker(const mqtt_connection_info_t *p_conn_info) {
  esp32_status_t ret;

  /* Validate input parameters */
  if ( p_conn_info == NULL) {
    return ESP32_ERROR;
  }

  /* Connect to MQTT broker */
  memset(at_cmd, '\0', MAX_AT_CMD_SIZE);
  snprintf((char *)at_cmd, MAX_AT_CMD_SIZE,
           "AT+MQTTCONN=0,\"%s\",%u,%u%s",
           p_conn_info->p_host,
           p_conn_info->p_port,
           p_conn_info->reconnect,
           AT_CMD_TERMINATOR);

  ret = run_at_cmd((uint8_t *)at_cmd, strlen((char *)at_cmd), (uint8_t *)AT_OK_STRING);

  return ret;
}

/**
 * @brief  Subscribe to an MQTT topic.
 * @param  topic: MQTT topic to subscribe to (e.g., "topic/esp32at").
 * @param  qos: Quality of Service level (typically 1).
 * @retval ESP32_OK on success, ESP32_ERROR otherwise.
 */
esp32_status_t esp32_mqtt_subscribe(const mqtt_subscribe_t *sub_args) {
  esp32_status_t ret;

  /* Construct the MQTTSUB command */
  memset(at_cmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)at_cmd, "AT+MQTTSUB=0,\"%s\",%u%s",
          sub_args->p_topic_filter, sub_args->qos, AT_CMD_TERMINATOR);

  ret = run_at_cmd((uint8_t *)at_cmd, strlen((char *)at_cmd),
                   (uint8_t *)AT_OK_STRING);

  return ret;
}

esp32_status_t esp32_mqtt_unsubscribe(const mqtt_subscribe_t *sub_args) {
    esp32_status_t ret;

    /* Construct the MQTTUNSUB command */
    memset(at_cmd, '\0', MAX_AT_CMD_SIZE);
    sprintf((char *)at_cmd, "AT+MQTTUNSUB=0,\"%s\"%s", sub_args->p_topic_filter, AT_CMD_TERMINATOR);

    ret = run_at_cmd((uint8_t *)at_cmd, strlen((char *)at_cmd),
                     (uint8_t *)AT_OK_STRING);

    return ret;
}

/**
 * @brief  Publishes an MQTT message using the AT+MQTTPUBRAW command.
 *
 * This function sends a raw MQTT publish command to the ESP32 AT module.
 * It first sends the command header and waits for a prompt ('>') before
 * sending the actual payload. If a response is expected, it processes it.
 *
 * @param  pub_args  Pointer to mqtt_receive_t containing topic, payload, QoS, etc.
 * @retval ESP32_OK on success, ESP32_ERROR on failure.
 */
esp32_status_t esp32_mqtt_publish(const mqtt_publish_t *pub_args) {
  assert(pub_args && pub_args->p_topic && pub_args->p_payload);

  esp32_status_t ret = ESP32_ERROR;

  /* Construct the MQTT PUBLISH command */
  memset(at_cmd, '\0', MAX_AT_CMD_SIZE);
  sprintf((char *)at_cmd, "AT+MQTTPUBRAW=0,\"%s\",%u,%u,%u%s",
      pub_args->p_topic, pub_args->payload_length, pub_args->qos, pub_args->retain, AT_CMD_TERMINATOR);

  /* The MQTT RAW publish command doesn't have a return command
         until the data is actually sent. Thus we check here whether
         we got the '>' prompt or not. */
  ret = run_at_cmd((uint8_t *)at_cmd, strlen((char *)at_cmd), (uint8_t *)AT_SEND_PROMPT_STRING);

  if (ret != ESP32_OK) {
    return ret;
  }

  /* Send actual payload and check response */
  ret = run_at_cmd((uint8_t *)pub_args->p_payload, pub_args->payload_length, (uint8_t *)AT_OK_STRING);

  return ret;
}

/**
 * @brief  receive data over the wifi connection.
 * @param  p_data the buffer to fill will the received data.
 * @param  length the maximum data size to receive.
 * @param ret_length the actual data received.
 * @retval returns ESP32_OK on success and ESP32_ERROR otherwise.
 */
esp32_status_t esp32_recv_mqtt_data(mqtt_receive_t *p_receive_info) {
  esp32_status_t ret;

  /* Validate input parameter */
  assert(p_receive_info != NULL);
  assert(p_receive_info->p_topic != NULL);
  assert(p_receive_info->p_payload != NULL);

  /* Receive the data from the host */
  ret = recv_mqtt_data(p_receive_info);

  return ret;
}
#if 1
/**
 * @brief Receives MQTT publish message from the WiFi module.
 * @param p_receive_info Pointer to the MQTT receive info structure.
 * @retval ESP32_OK on success, ESP32_ERROR otherwise.
 */
static esp32_status_t recv_mqtt_data(mqtt_receive_t *p_receive_info) {
  uint8_t rx_char;
  uint32_t idx = 0;
  char length_string[10] = {0};
  uint32_t length_value = 0;
  uint8_t i = 0;
  esp32_boolean new_chunk = ESP32_FALSE;

  /* Validate input parameter */
  assert(p_receive_info != NULL);
  assert(p_receive_info->p_topic != NULL);
  assert(p_receive_info->p_payload != NULL);

  /* Reset temporary buffer and output structure fields */
  memset(rx_buffer, 0, MAX_BUFFER_SIZE);
  memset(p_receive_info->p_topic, 0, p_receive_info->topic_length);
  memset(p_receive_info->p_payload, 0, p_receive_info->payload_length);
  p_receive_info->topic_length = 0;
  p_receive_info->payload_length = 0;

  while (true) {
    if (esp32_io_recv_nb(&rx_char, 1) != 0) {
      // If a new MQTT chunk is expected, fill the payload
      if (new_chunk == ESP32_TRUE) {
        if (length_value > 0) {
          p_receive_info->p_payload[p_receive_info->payload_length++] = rx_char;
          length_value--;
        } else {
          // Payload complete, reset for next message
          new_chunk = ESP32_FALSE;
          memset(rx_buffer, 0, MAX_BUFFER_SIZE);
          idx = 0;
        }
      }

      // Store character in receive buffer for pattern matching
      if (idx < MAX_BUFFER_SIZE) {
        rx_buffer[idx++] = rx_char;
      } else {
        return ESP32_ERROR;
      }
    } else {
      // No data received
      if (new_chunk == ESP32_TRUE && length_value != 0) {
        return ESP32_ERROR;
      }
      break;
    }

    // Check for start of MQTT received message
    if ((strstr((char *)rx_buffer, "+MQTTSUBRECV:0,") != NULL) && (new_chunk == ESP32_FALSE)) {
      // Read separator character (should be `"` for topic start)
      esp32_io_recv(&rx_char, 1);

      // Read topic name until next quote (`"`)
      p_receive_info->topic_length = 0;
      while (esp32_io_recv_nb(&rx_char, 1) && rx_char != '"') {

        p_receive_info->p_topic[p_receive_info->topic_length++] = rx_char;
      }
      p_receive_info->p_topic[p_receive_info->topic_length] = '\0';

      // Read separator character before length
      esp32_io_recv(&rx_char, 1);

      // Read payload length as string
      i = 0;
      memset(length_string, 0, sizeof(length_string));
      while (esp32_io_recv_nb(&rx_char, 1) && rx_char != ',' &&
             i < sizeof(length_string) - 1) {

        length_string[i++] = rx_char;
      }
      length_string[i] = '\0';

      // Convert length string to integer
      length_value = atoi(length_string);
      if (length_value > MAX_BUFFER_SIZE) {
        return ESP32_ERROR;
      }

      // Now ready to read payload
      new_chunk = ESP32_TRUE;
    }

    // Check for any error response in the stream
    if (strstr((char *)rx_buffer, "ERROR") != NULL) {
      return ESP32_ERROR;
    }
  }

  return ESP32_OK;
}
#else

static esp32_status_t recv_mqtt_data(mqtt_receive_t *p_receive_info) {
    static char last_topic[80+1] = {0};
    static uint32_t last_payload_offset = 0;
    static esp32_boolean buffering = ESP32_FALSE;

    uint8_t rx_char;
    uint32_t idx = 0;
    char length_string[10] = {0};
    uint32_t length_value = 0;
    uint8_t i = 0;
    esp32_boolean new_chunk = ESP32_FALSE;

    assert(p_receive_info != NULL);
    assert(p_receive_info->p_topic != NULL);
    assert(p_receive_info->p_payload != NULL);

    memset(rx_buffer, 0, MAX_BUFFER_SIZE);

    while (true) {
        if (esp32_io_recv_nb(&rx_char, 1) != 0) {
            if (new_chunk == ESP32_TRUE) {
                if (length_value > 0) {
                    if ((last_payload_offset + p_receive_info->payload_length) < MAX_BUFFER_SIZE) {
                        p_receive_info->p_payload[last_payload_offset++] = rx_char;
                    }
                    length_value--;
                } else {
                    new_chunk = ESP32_FALSE;
                    break;
                }
            }

            if (idx < MAX_BUFFER_SIZE) {
                rx_buffer[idx++] = rx_char;
            } else {
                return ESP32_ERROR;
            }
        } else {
            if (new_chunk == ESP32_TRUE && length_value != 0) {
                return ESP32_ERROR;
            }
            break;
        }

        // Match incoming MQTT message
        if ((strstr((char *)rx_buffer, "+MQTTSUBRECV:0,") != NULL) && (new_chunk == ESP32_FALSE)) {
            // Skip '"' before topic
            esp32_io_recv(&rx_char, 1);

            char current_topic[80 + 1] = {0};
            size_t topic_len = 0;

            while (esp32_io_recv_nb(&rx_char, 1) && rx_char != '"' && topic_len < 80) {
                current_topic[topic_len++] = rx_char;
            }
            current_topic[topic_len] = '\0';

            // Skip separator
            esp32_io_recv(&rx_char, 1);

            // Read payload length
            i = 0;
            memset(length_string, 0, sizeof(length_string));
            while (esp32_io_recv_nb(&rx_char, 1) && rx_char != ',' && i < sizeof(length_string) - 1) {
                length_string[i++] = rx_char;
            }
            length_string[i] = '\0';

            length_value = atoi(length_string);
            if (length_value > MAX_BUFFER_SIZE - last_payload_offset) {
                return ESP32_ERROR;
            }

            // If topic changed while buffering, finalize old message and return it
            if (buffering == ESP32_TRUE && strcmp(current_topic, last_topic) != 0) {
                // Return previous message
                memcpy(p_receive_info->p_topic, last_topic, strlen(last_topic));
                p_receive_info->topic_length = strlen(last_topic);
                p_receive_info->payload_length = last_payload_offset;

                // Prepare for next round
                memset(last_topic, 0, sizeof(last_topic));
                last_payload_offset = 0;
                buffering = ESP32_FALSE;

                // Rewind current read position to re-process this new chunk on next call
                return ESP32_OK;
            }

            // Same topic - continue appending
            strcpy(last_topic, current_topic);
            new_chunk = ESP32_TRUE;
            buffering = ESP32_TRUE;
        }

        // Handle "ERROR"
        if (strstr((char *)rx_buffer, "ERROR") != NULL) {
            return ESP32_ERROR;
        }
    }

    // Finished processing a full payload
    if (new_chunk == ESP32_FALSE && buffering == ESP32_TRUE && length_value == 0) {
        // Return buffered message
        memcpy(p_receive_info->p_topic, last_topic, strlen(last_topic));
        p_receive_info->topic_length = strlen(last_topic);
        p_receive_info->payload_length = last_payload_offset;

        // Reset buffer
        memset(last_topic, 0, sizeof(last_topic));
        last_payload_offset = 0;
        buffering = ESP32_FALSE;

        return ESP32_OK;
    }

    return ESP32_ERROR;  // Default fallback
}
#endif
/**
 * @brief  Send data over the wifi connection.
 * @param  buffer: the buffer to send
 * @param  length: the buffer's data size.
 * @retval returns ESP32_OK on success and ESP32_ERROR otherwise.
 */
esp32_status_t esp32_send_data(uint8_t *buffer, uint32_t length) {
  // uart_dma_restart();
  esp32_status_t ret = ESP32_OK;

  if ( buffer != NULL ) {
    // uint32_t tickStart;
    /* Construct the CIPSEND command */
    memset(at_cmd, '\0', MAX_AT_CMD_SIZE);
    sprintf((char *)at_cmd, "AT+CIPSEND=%lu%s", length, AT_CMD_TERMINATOR );

    /* The CIPSEND command doesn't have a return command
       until the data is actually sent. Thus we check here whether
       we got the '>' prompt or not. */
    ret = run_at_cmd((uint8_t *)at_cmd, strlen((char *)at_cmd),
                      (uint8_t *)AT_SEND_PROMPT_STRING);

    /* return Error */
    if ( ret != ESP32_OK ) {
      return ESP32_ERROR;
    }

    /* Wait before sending data. */
    //    tickStart = HAL_GetTick();
    //    while (HAL_GetTick() - tickStart < 500)
    //    {
    //    }

    /* Send the data */
    ret = run_at_cmd(buffer, length, (uint8_t *)AT_SEND_OK_STRING);
  }

  return ret;
}

/**
 * @brief  receive data over the wifi connection.
 * @param  p_data the buffer to fill will the received data.
 * @param  length the maximum data size to receive.
 * @param ret_length the actual data received.
 * @retval returns ESP32_OK on success and ESP32_ERROR otherwise.
 */
esp32_status_t esp32_recv_data(uint8_t *p_data, uint32_t length,
                               uint32_t *ret_length) {
  esp32_status_t ret;

  /* Receive the data from the host */
  ret = recv_data(p_data, length, ret_length);
  return ret;
}

/**
 * @brief  Run the AT command
 * @param  cmd the buffer to fill will the received data.
 * @param  length the maximum data size to receive.
 * @param  token the expected output if command runs successfully
 * @retval returns ESP32_OK on success and ESP32_ERROR otherwise.
 */
static esp32_status_t run_at_cmd(uint8_t *cmd, uint32_t length, const uint8_t *token) {
  uint32_t idx = 0;
  uint8_t rx_char;

  /* Reset the Rx buffer to make sure no previous data exist */
  memset(rx_buffer, '\0', MAX_BUFFER_SIZE);

  if ( esp32_io_send(cmd, length) < 0 )
  /* Send the command */
  {
    return ESP32_ERROR;
  }

  /* Wait for reception */
  while ( true ) {
    /* Wait to receive data */
    if ( esp32_io_recv(&rx_char, 1) != 0 ) {
      rx_buffer [ idx++ ] = rx_char;
    } else {
      break;
    }

    /* Check that max buffer size has not been reached */
    if ( idx == MAX_BUFFER_SIZE ) {
      break;
    }

    /* Extract the token */
    if ( strstr((char *)rx_buffer, (char *)token) != NULL ) {
      return ESP32_OK;
    }

    /* Check if the message contains error code */
    if ( strstr((char *)rx_buffer, AT_ERROR_STRING) != NULL ) {
      return ESP32_ERROR;
    }
  }

  return ESP32_ERROR;
}

/**
 * @brief  Receive data from the WiFi module
 * @param  buffer The buffer where to fill the received data
 * @param  length the maximum data size to receive.
 * @param  ret_length length of received data
 * @retval returns ESP32_OK on success and ESP32_ERROR otherwise.
 */
static esp32_status_t recv_data(uint8_t *buffer, uint32_t length,
                                uint32_t *ret_length) {
  uint8_t rx_char;
  uint32_t idx = 0;
  uint8_t length_string [ 4 ];
  uint32_t length_value;
  uint8_t i = 0;
  esp32_boolean new_chunk = ESP32_FALSE;

  /* Reset the reception data length */
  *ret_length = 0;

  /* Reset the reception buffer */
  memset(rx_buffer, '\0', MAX_BUFFER_SIZE);

  /* When reading data over a wifi connection the esp
     splits it into chunks of 1460 bytes maximum each, each chunk is preceded
     by the string "+IPD,<chunk_size>:". Thus to get the actual data we need to:
       - Receive data until getting the "+IPD," token, a new chunk is marked.
       - Extract the 'chunk_size' then read the next 'chunk_size' bytes as
     actual data
       - Mark end of the chunk.
       - Repeat steps above until no more data is available. */
  while ( true ) {
    if ( esp32_io_recv(&rx_char, 1) != 0 ) {
      /* The data chunk starts with +IPD,<chunk length>: */
      if ( new_chunk == ESP32_TRUE ) {
        /* Read the next length_value bytes as part from the actual data. */
        if ( length_value-- ) {
          *buffer++ = rx_char;
          (*ret_length)++;
        } else {
          /* Clear the buffer as the new chunk has ended. */
          new_chunk = ESP32_FALSE;
          memset(rx_buffer, '\0', MAX_BUFFER_SIZE);
          idx = 0;
        }
      }
      rx_buffer [ idx++ ] = rx_char;
    } else {
      /* Errors while reading return an error. */
      if ( (new_chunk == ESP32_TRUE) && (length_value != 0) ) {
        return ESP32_ERROR;
      } else {
        break;
      }
    }

    if ( idx == MAX_BUFFER_SIZE ) {
      /* In case of buffer overflow, return error */
      if ( (new_chunk == ESP32_TRUE) && (length_value != 0) ) {
        return ESP32_ERROR;
      } else {
        break;
      }
    }

    /* When a new chunk is met, extract its size */
    if ( (strstr((char *)rx_buffer, AT_IPD_STRING) != NULL) &&
         (new_chunk == ESP32_FALSE) ) {
      i = 0;
      memset(length_string, '\0', 4);
      do {
        esp32_io_recv(&rx_char, 1);
        length_string [ i++ ] = rx_char;
      } while ( rx_char != ':' );

      /* Get the buffer length */
      length_value = atoi((char *)length_string);
      new_chunk = ESP32_TRUE;
    }

    /* Check if message contains error code */
    if ( strstr((char *)rx_buffer, AT_ERROR_STRING) != NULL ) {
      return ESP32_ERROR;
    }

    /* Check for the chunk end */
    if ( strstr((char *)rx_buffer, AT_IPD_OK_STRING) != NULL ) {
      new_chunk = ESP32_FALSE;
    }
  }

  return ESP32_OK;
}
