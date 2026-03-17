/*
 * esp32_at.h
 *
 *  Created on: Dec 27, 2024
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */

#ifndef INC_ESP32_H_
#define INC_ESP32_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "main.h"

/* Private define ------------------------------------------------------------*/
#define MAX_AT_CMD_SIZE         256
#define MAX_BUFFER_SIZE         (1024*3)
#define AT_CMD_TERMINATOR       "\r\n"
#define AT_OK_STRING            "OK\r\n"
#define AT_CONNECT_STRING       "CONNECT\r\n"
#define AT_IPD_OK_STRING        "OK\r\n\r\n"
#define AT_SEND_OK_STRING       "SEND OK\r\n"
#define AT_SEND_PROMPT_STRING   "OK\r\n\r\n>"
#define AT_ERROR_STRING         "ERROR\r\n"
#define AT_IPD_STRING           "+IPD,"
#define AT_HTTPCGET_STRING      "+HTTPCGET:"

/* Exported types ------------------------------------------------------------*/
typedef enum {
  ESP32_FALSE                   = 0,
  ESP32_TRUE                    = 1
} esp32_boolean;

typedef enum {
  ESP32_OK                      = 0,
  ESP32_ERROR                   = 1,
  ESP32_BUSY                    = 2,
  ESP32_ALREADY_CONNECTED       = 3,
  ESP32_CONNECTION_CLOSED       = 4,
  ESP32_TIMEOUT                 = 5,
  ESP32_IO_ERROR                = 6,
} esp32_status_t;

typedef enum {
  ESP32_ENCRYPTION_OPEN         = 0,
  ESP32_ENCRYPTION_WEP          = 1,
  ESP32_ENCRYPTION_WPA_PSK      = 2,
  ESP32_ENCRYPTION_WPA2_PSK     = 3,
  ESP32_ENCRYPTION_WPA_WPA2_PSK = 4,
} esp32_encryption_t;

typedef enum {
  ESP32_NULL_MODE               = 0,  /* Wi-Fi RF will be disabled */
  ESP32_STATION_MODE            = 1,
  ESP32_SOFT_AP_MODE            = 2,
  ESP32_MIXED_MODE              = 3,
} esp32_cwmode_t;

typedef enum {
  NORMAL_MODE                   = 0,
  UNVARNISHED_MODE              = 1
} esp32_transfer_mode_t;

typedef enum {
  ESP32_GOT_IP_STATUS           = 1,
  ESP32_CONNECTED_STATUS        = 2,
  ESP32_DISCONNECTED_STATUS     = 3,
} esp32_connection_status_t;

typedef enum {
  ESP32_TCP_CONNECTION          = 0,
  ESP32_UDP_CONNECTION          = 1,
} esp32_connection_mode_t;

typedef enum {
  UDP_PEER_NO_CHANGE            = 0,
  UDP_PEER_CHANGE_ONCE          = 1,
  UDP_PEER_CHANGE_ALLOWED       = 2,
  UDP_PEER_CHANGE_INVALID       = -1,
} esp32_connection_policy_t;

typedef enum {
  ESP32_ECHO_OFF = 0,
  ESP32_ECHO_ON  = 1
} esp32_echo_state_t;

typedef struct {
  esp32_connection_status_t     connection_status;
  esp32_connection_mode_t       connection_type;
  esp32_connection_policy_t     connection_mode;  /* For UDP connections only */
  uint8_t                       connection_id;
  uint8_t*                      ip_address;
  uint32_t                      port;
  uint32_t                      local_port;       /* For UDP connection only */
  esp32_boolean                 is_server;
} esp32_connection_info_t;

typedef struct {
  uint8_t*                      ssid;
  uint8_t*                      password;
  uint16_t                      channel_id;
  esp32_encryption_t            encryption_mode;
} esp32_ap_config_t;

typedef enum {
  MQTT_ERROR                    = 0,
  MQTT_SUCCESS                  = 1
}mqtt_status_t;

typedef enum {
  MQTT_DISCONNECTED             = 0,          /**< MQTT connection is inactive. */
  MQTT_CONNECTED                = 1,          /**< MQTT connection is active. */
  MQTT_ALREADY_CONNECTED        = 2           /**< Client tried to connect, but was already connected. */
} mqtt_connection_status_t;

typedef enum {
  MQTT_QOS0                     = 0,          /**< At most once delivery */
  MQTT_QOS1                     = 1,          /**< At least once delivery */
  MQTT_QOS2                     = 2           /**< Exactly once delivery */
} mqtt_qos_t;

/**
 * @brief MQTT Reconnect Options
 */
typedef enum
{
  MQTT_RECONNECT_DISABLE        = 0,          /**< Reconnect is disabled */
  MQTT_RECONNECT_ENABLE         = 1           /**< Reconnect is enabled */
} mqtt_reconnect_t;

/**
 * @brief MQTT connection schemes.
 */
typedef enum {
  MQTT_SCHEME_TCP               = 1,          /**< MQTT over TCP */
  MQTT_SCHEME_TLS_NO_VERIFY     = 2,          /**< MQTT over TLS (no certificate verify) */
  MQTT_SCHEME_TLS_VERIFY_SERVER = 3,          /**< MQTT over TLS (verify server certificate) */
  MQTT_SCHEME_TLS_CLIENT_CERT   = 4,          /**< MQTT over TLS (provide client certificate) */
  MQTT_SCHEME_TLS_BOTH_CERTS    = 5,          /**< MQTT over TLS (verify server certificate and provide client certificate) */
  MQTT_SCHEME_WS_TCP            = 6,          /**< MQTT over WebSocket (based on TCP) */
  MQTT_SCHEME_WS_TLS_NO_VERIFY  = 7,          /**< MQTT over WebSocket Secure (based on TLS, no certificate verify) */
  MQTT_SCHEME_WS_TLS_VERIFY_SERVER  = 8,      /**< MQTT over WebSocket Secure (based on TLS, verify server certificate) */
  MQTT_SCHEME_WS_TLS_CLIENT_CERT    = 9,      /**< MQTT over WebSocket Secure (based on TLS, provide client certificate) */
  MQTT_SCHEME_WS_TLS_BOTH_CERTS     = 10      /**< MQTT over WebSocket Secure (based on TLS, verify server certificate and provide client certificate) */
} mqtt_scheme_t;

/** @brief Structure to hold MQTT publish message information */
typedef struct {
  const char                    *p_topic;         /**< Topic name */
  size_t                        topic_length;     /**< Length of topic */
  const char                    *p_payload;       /**< Payload data */
  size_t                        payload_length;   /**< Payload length */
  mqtt_qos_t                    qos;              /**< QoS level */
  bool                          retain;           /**< Retain flag */
} mqtt_publish_t;

/** @brief Structure to hold MQTT receive message information */
typedef struct {
  char                          *p_topic;         /**< Topic name */
  size_t                        topic_length;     /**< Length of topic */
  char                          *p_payload;       /**< Payload data */
  size_t                        payload_length;   /**< Payload length */
} mqtt_receive_t;

typedef struct {
  mqtt_scheme_t                 scheme;           /**< Authentication scheme (e.g., 5 for MQTT over TLS) */
  const char                    *p_client_id;     /**< Client ID */
  const char                    *p_username;      /**< Username (optional) */
  const char                    *p_password;      /**< Password (optional) */
  uint8_t                       cert_key_id;      /**< Client certificate key ID */
  uint8_t                       ca_id;            /**< CA certificate ID */
  const char                    *path;            /**< Path for certificate storage */
} mqtt_user_config_t;

typedef struct {
  uint16_t                      keep_alive;            /**< Keep Alive time in seconds */
  uint8_t                       disable_clean_session; /**< Disable clean session flag */
  mqtt_publish_t                lwt_info;              /**< Last Will and Testament (LWT) info */
} mqtt_conn_config_t;

/**
 * @ingroup mqtt_struct_types
 * @brief MQTT CONNECT packet parameters.
 */
typedef struct {
  mqtt_user_config_t            user_config;      /**< MQTT user credentials and certificate settings */
  mqtt_conn_config_t            conn_config;      /**< MQTT connection-level options */
  const char                    *p_host;          /**< MQTT broker hostname */
  uint16_t                      p_port;           /**< MQTT broker port */
  mqtt_reconnect_t              reconnect;        /**< Reconnect behavior (enable/disable) */
} mqtt_connection_info_t;

/** @brief Structure to hold MQTT subscribe information */
typedef struct {
  const char                    *p_topic_filter;   /**< Topic to subscribe to */
  size_t                        topic_filter_length;
  mqtt_qos_t                    qos;               /**< QoS level */
} mqtt_subscribe_t;


/** @brief Structure for storing SNTP time information */
typedef struct {
  char day[4];   /**< Day of the week */
  char month[4]; /**< Month abbreviation */
  int date;      /**< Date */
  int year;      /**< Year */
  int hour;      /**< Hour */
  int min;       /**< Minute */
  int sec;       /**< Second */
} sntp_time_t;

/* Exported functions ------------------------------------------------------- */
esp32_status_t esp32_init(void);
esp32_status_t esp32_deinit(void);
esp32_status_t esp32_reset(void);

esp32_status_t esp32_quit_ap(void);
esp32_status_t esp32_join_ap(uint8_t* ssid, uint8_t* password);
esp32_status_t esp32_get_ip(esp32_cwmode_t mode, uint8_t* ip_address);
esp32_status_t esp32_establish_connection(const esp32_connection_info_t* connection_info);
esp32_status_t esp32_close_connection(const uint8_t channel_id);

esp32_status_t esp32_config_sntp( int utc_offset);
esp32_status_t esp32_get_sntp_time(sntp_time_t *time_data);
esp32_status_t esp32_mqtt_configure(const mqtt_connection_info_t *p_conn_info);
esp32_status_t esp32_mqtt_connect_to_broker(const mqtt_connection_info_t *p_conn_info);
esp32_status_t esp32_mqtt_subscribe(const mqtt_subscribe_t *sub_args);
esp32_status_t esp32_mqtt_unsubscribe(const mqtt_subscribe_t *sub_args);
esp32_status_t esp32_mqtt_publish(const mqtt_publish_t *pub_args);
esp32_status_t esp32_recv_mqtt_data( mqtt_receive_t *p_publish_info ) ;

esp32_status_t esp32_send_data(uint8_t* p_data, uint32_t length);
esp32_status_t esp32_recv_data(uint8_t* p_data, uint32_t length, uint32_t* ret_length);

#endif /* INC_ESP32_H_ */
