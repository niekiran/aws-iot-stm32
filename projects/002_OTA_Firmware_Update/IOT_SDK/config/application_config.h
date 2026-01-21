/*
 * application_config.h
 *
 *  Created on: Mar 28, 2025
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */

#ifndef INC_APPLICATION_CONFIG_H_
#define INC_APPLICATION_CONFIG_H_

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Include logging header files and define logging macros in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define the LIBRARY_LOG_NAME and LIBRARY_LOG_LEVEL macros depending on
 * the logging configuration.
 * 3. Include the header file "logging_stack.h", if logging is enabled.
 */

#include "logging_levels.h"

#ifndef LIBRARY_LOG_NAME
#define LIBRARY_LOG_NAME               "MAIN_APP"
#endif

#ifndef LIBRARY_LOG_LEVEL
#define LIBRARY_LOG_LEVEL              LOG_INFO
#endif

#include "logging_stack.h"

#define WIFI_SSID                          "BHARATISOFT 2025"
#define WIFI_PASSWORD                      "*********"

#define MQTT_BROKER                        "****************.iot.ap-south-1.amazonaws.com"

#define MQTT_PORT                          8883

#define CLIENT_ID                          "my_iot_device_1"

#define UTC_OFFSET                         530

#define SENSOR_DATA_TOPIC                  "sensors/room1/temp_humidity"

#define MQTT_PUBLISH_TIME_BETWEEN_MS       ( 5000U )

#define MAX_MQTT_PAYLOAD_SIZE              1600
#define MAX_MQTT_TOPIC_SIZE                150

/* OTA Application: Define flash memory regions for OTA update operations */
#define USER_FLASH_FIRST_SECTOR_ADDRESS    0x08020000
#define USER_FLASH_SECOND_SECTOR_ADDRESS   0x08040000

#define USER_FLASH_LAST_SECTOR_ADDRESS     0x081C0000
#define USER_FLASH_END_ADDRESS             0x081FFFFF

#endif /* INC_APPLICATION_CONFIG_H_ */
