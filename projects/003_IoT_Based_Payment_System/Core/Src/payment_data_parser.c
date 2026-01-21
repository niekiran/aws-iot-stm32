/*
 * payment_data_parser.c
 *
 *  Created on: Jun 25, 2025
 *  Author: Shreyas Acharya, BHARATI SOFTWARE
 *
 */

#include "payment_data_parser.h"
#include "core_json.h"
#include "application_config.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* =========================================================
 * Timezone configuration
 * ========================================================= */
#define UTC_OFFSET_HHMM     UTC_OFFSET   //530 /* India +05:30 */

/* =========================================================
 * Internal helpers
 * ========================================================= */

/* Parse ISO-8601 UTC timestamp: YYYY-MM-DDTHH:MM:SS(.sss) */
static int parse_iso8601_utc(const char *ts,
                             int *year,
                             int *month,
                             int *day,
                             int *hour,
                             int *minute,
                             int *second)
{
    if (sscanf(ts, "%4d-%2d-%2dT%2d:%2d:%2d",
               year, month, day,
               hour, minute, second) == 6)
    {
        return 0;
    }
    return -1;
}

/* Apply timezone offset in HHMM format (e.g. 530 = +05:30) */
static void apply_utc_offset_hhmm(int *hour, int *minute)
{
    int offset_h = UTC_OFFSET_HHMM / 100;
    int offset_m = UTC_OFFSET_HHMM % 100;

    *hour   += offset_h;
    *minute += offset_m;

    if (*minute >= 60) {
        *minute -= 60;
        (*hour)++;
    }

    if (*minute < 0) {
        *minute += 60;
        (*hour)--;
    }

    if (*hour >= 24) {
        *hour -= 24;
    }

    if (*hour < 0) {
        *hour += 24;
    }
}

/* =========================================================
 * Public API
 * ========================================================= */

int8_t parse_payment_data(const char *payload,
                          const size_t payload_length,
                          payment_data_t *result)
{
    if (payload == NULL || result == NULL) {
        return -1;
    }

    JSONStatus_t json_result;
    const char *json_value = NULL;
    size_t json_value_len = 0U;

    memset(result, 0, sizeof(payment_data_t));

    //payment_id
    json_result = JSON_SearchConst(payload, payload_length,
                                   "payment_id", strlen("payment_id"),
                                   &json_value, &json_value_len, NULL);

    if (json_result != JSONSuccess ||
        json_value_len >= sizeof(result->payment_id))
    {
        return -2;
    }

    memcpy(result->payment_id, json_value, json_value_len);
    result->payment_id[json_value_len] = '\0';
    result->payment_id_len = json_value_len;

    /* -----------------------------------------------------
     * amount
     * ----------------------------------------------------- */
    json_result = JSON_SearchConst(payload, payload_length,
                                   "amount", strlen("amount"),
                                   &json_value, &json_value_len, NULL);

    if (json_result != JSONSuccess) {
        return -3;
    }

    char amount_str[32] = {0};
    size_t len = (json_value_len < sizeof(amount_str) - 1)
                 ? json_value_len
                 : sizeof(amount_str) - 1;

    memcpy(amount_str, json_value, len);
    result->amount = strtof(amount_str, NULL);

    //status
    json_result = JSON_SearchConst(payload, payload_length,
                                   "status", strlen("status"),
                                   &json_value, &json_value_len, NULL);

    if (json_result != JSONSuccess ||
        json_value_len >= sizeof(result->status))
    {
        return -4;
    }

    memcpy(result->status, json_value, json_value_len);
    result->status[json_value_len] = '\0';

    //timestamp
    json_result = JSON_SearchConst(payload, payload_length,
                                   "timestamp", strlen("timestamp"),
                                   &json_value, &json_value_len, NULL);

    if (json_result != JSONSuccess || json_value_len >= 64) {
        return -5;
    }

    char full_timestamp[64] = {0};
    memcpy(full_timestamp, json_value, json_value_len);
    full_timestamp[json_value_len] = '\0';

    int year, month, day, hour, minute, second;

    if (parse_iso8601_utc(full_timestamp,
                          &year, &month, &day,
                          &hour, &minute, &second) == 0)
    {
        apply_utc_offset_hhmm(&hour, &minute);

        snprintf(result->timestamp,
                 sizeof(result->timestamp),
                 "%02d:%02d:%02d",
                 hour, minute, second);
    }
    else
    {
        strncpy(result->timestamp,
                full_timestamp,
                sizeof(result->timestamp) - 1);
    }

    //Debug logs
    LogInfo(("Payment ID  : %s", result->payment_id));
    LogInfo(("Amount      : %.2f", result->amount));
    LogInfo(("Status      : %s", result->status));
    LogInfo(("Local Time  : %s", result->timestamp));

    return 0;
}
