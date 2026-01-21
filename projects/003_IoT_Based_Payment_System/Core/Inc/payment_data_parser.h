/*
 * payment_data_parser.h
 *
 *  Created on: Jun 25, 2025
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */

#ifndef INC_PAYMENT_DATA_PARSER_H_
#define INC_PAYMENT_DATA_PARSER_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/

/* Logging related header files are required to be included in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define LIBRARY_LOG_NAME and  LIBRARY_LOG_LEVEL.
 * 3. Include the header file "logging_stack.h".
 */
/* Include header that defines log levels. */
#include "logging_levels.h"

/* Configure name and log level for the OTA library. */
#ifndef LIBRARY_LOG_NAME
    #define LIBRARY_LOG_NAME     "PARSER"
#endif
#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_DEBUG
#endif

#include "logging_stack.h"

/************ End of logging configuration ****************/

#include "payment_app.h"
#include <stdint.h>

/**
 * @brief Parse Razorpay payment event JSON received over MQTT.
 *
 * Parses the raw MQTT payload (JSON) and extracts:
 *  - payment_id
 *  - amount
 *  - status
 *  - timestamp ( converted to local time if UTC offset is configured)
 * The extracted values are stored into the provided payment_data_t structure.
 *
 * @param payload         Pointer to the raw MQTT payload buffer (may not be null-terminated).
 * @param payload_length  Length of the payload buffer in bytes.
 * @param result          Output structure to be populated with parsed fields.
 *
 * @return 0 on success; negative value on failure (e.g., invalid JSON, missing key, or buffer overflow).
 */
int8_t parse_payment_data(const char *payload,
                          const size_t payload_length,
                          payment_data_t *result);


#endif /* INC_PAYMENT_DATA_PARSER_H_ */
