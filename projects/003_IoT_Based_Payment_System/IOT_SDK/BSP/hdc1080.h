/*
 * hdc1080.h
 *
 *  Created on: Feb 17, 2025
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */

#ifndef INC_HDC1080_H_
#define INC_HDC1080_H_

#include "main.h"

#define HDC_1080_ADD                    0x40
#define Configuration_register_add      0x02
#define Temperature_register_add        0x00
#define Humidity_register_add           0x01

typedef enum {
  HDC_OK = 0,
  HDC_ERROR = 1
} hdc_status_t;

typedef enum
{
  Temperature_Resolution_14_bit = 0,
  Temperature_Resolution_11_bit = 1
} temp_resolution_t;

typedef enum
{
  Humidity_Resolution_14_bit = 0,
  Humidity_Resolution_11_bit = 1,
  Humidity_Resolution_8_bit  = 2
} humi_resolution_t;

hdc_status_t hdc1080_init(I2C_HandleTypeDef* hi2c_x,
                          temp_resolution_t temp_resolution_x_bit,
                          humi_resolution_t humi_resolution_x_bit);
hdc_status_t hdc1080_read_temperature(I2C_HandleTypeDef* hi2c_x, float* temperature);
hdc_status_t hdc1080_read_humidity(I2C_HandleTypeDef* hi2c_x, uint8_t* humidity);
hdc_status_t hdc1080_start_measurement(I2C_HandleTypeDef* hi2c_x,
                                       float* temperature,
                                       uint8_t* humidity);
#endif /* INC_HDC1080_H_ */
