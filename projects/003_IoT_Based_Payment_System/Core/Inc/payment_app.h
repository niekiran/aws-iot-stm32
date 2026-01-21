/*
 * payment_app.h
 *
 *  Created on: Dec 27, 2025
 *      Author: bhara
 */

#ifndef INC_PAYMENT_APP_H_
#define INC_PAYMENT_APP_H_

typedef struct {
    char payment_id[50];
    size_t payment_id_len;
    float amount;
    char status[20];
    char timestamp[50];
} payment_data_t;

void display_qr(void);
void handle_payment_info( char * topic,
                          size_t topic_length,
                          uint8_t * payload,
                          size_t payload_length );

#endif /* INC_PAYMENT_APP_H_ */
