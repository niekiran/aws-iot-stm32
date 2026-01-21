/*
 * custom_job_parser.h
 *
 *  Created on: Apr 15, 2025
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */

#ifndef INC_CUSTOM_JOB_PARSER_H_
#define INC_CUSTOM_JOB_PARSER_H_


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
    #define LIBRARY_LOG_NAME     "JOB"
#endif
#ifndef LIBRARY_LOG_LEVEL
    #define LIBRARY_LOG_LEVEL    LOG_DEBUG
#endif

#include "logging_stack.h"

/************ End of logging configuration ****************/

#include "custom_job_doc.h"

int8_t custom_parser_parse_job_doc_file( const char * job_doc,
                                         const size_t job_doc_length,
                                         custom_job_doc_fields_t * result );

#endif /* INC_CUSTOM_JOB_PARSER_H_ */
