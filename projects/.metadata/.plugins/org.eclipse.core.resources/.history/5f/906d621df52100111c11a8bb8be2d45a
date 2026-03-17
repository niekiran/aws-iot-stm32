/*
 * app_util.h
 *
 *  Created on: Jan 8, 2025
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */


#ifndef INC_APP_H_
#define INC_APP_H_

#include <stdint.h>

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Include logging header files and define logging macros in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define the LIBRARY_LOG_NAME and LIBRARY_LOG_LEVEL macros depending on
 * the logging configuration for MQTT.
 * 3. Include the header file "logging_stack.h", if logging is enabled for MQTT.
 */

#include "logging_levels.h"

/* Logging configuration for the MQTT library. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME    "UTIL"
#endif

#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_INFO
#endif

#include "logging_stack.h"

/************ End of logging configuration ****************/

int init_temperature_humidity_sensor(void);
void get_temperature_reading(float *temperature);
void get_humidity_reading(uint8_t *humidity) ;

#endif /* INC_APP_H_ */
