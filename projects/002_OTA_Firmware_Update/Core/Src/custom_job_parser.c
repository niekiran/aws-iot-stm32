/*
 * custom_job_doc.c
 *
 *  Created on: Apr 15, 2025
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */
#include "custom_job_parser.h"

#include "core_json.h"

int8_t custom_parser_parse_job_doc_file( const char * job_doc,
                                         const size_t job_doc_length,
                                         custom_job_doc_fields_t * result ) {

  JSONStatus_t json_result = JSONNotFound;
  const char *json_value = NULL;
  size_t json_value_length = 0U;

  /* Parse "streamID" */
  json_result = JSON_SearchConst(job_doc,
                                 job_doc_length,
                                 "streamID",
                                 8U,
                                 &json_value,
                                 &json_value_length,
                                 NULL);
  if (json_result != JSONSuccess) return -1;

  result->image_ref = json_value;
  result->image_ref_len = json_value_length;

  /* Parse "files[0].fileSize" */
  json_result = JSON_SearchConst(job_doc,
                                 job_doc_length,
                                 "files[0].fileSize",
                                 17U,
                                 &json_value,
                                 &json_value_length,
                                 NULL);
  if (json_result != JSONSuccess) return -1;

  char size_buf[16] = {0};
  memcpy(size_buf, json_value, json_value_length);
  size_buf[json_value_length] = '\0';
  result->file_size = (uint32_t)atoi(size_buf);

  return 0;
}
