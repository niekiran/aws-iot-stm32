/*
 * sensor_util.h
 *
 *  Created on: Jan 8, 2025
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */

#include "sensor_util.h"
#include <string.h>
#include <stdio.h>
#include "esp32_at.h"
#include "hdc1080.h"
#include <stdlib.h>
#include <stdarg.h>

extern I2C_HandleTypeDef hi2c1;

/**
 * @brief Initializes the temperature and humidity sensor.
 *
 * This function initializes the HDC1080 sensor with a resolution of 14 bits
 * for both temperature and humidity readings.
 *
 * @return hdc_status_t Returns the status of the initialization process.
 */
int init_temperature_humidity_sensor(void) {
  hdc_status_t ret;
  LogInfo( ("Initializing temperature and humidity sensor...") );
  hdc1080_init(&hi2c1, Temperature_Resolution_14_bit, Humidity_Resolution_14_bit);
  LogInfo( ("Sensor initialization complete.") );
  ret = hdc1080_init(&hi2c1, Temperature_Resolution_14_bit, Humidity_Resolution_14_bit);

  return ret;;
}

/**
 * @brief Reads the current temperature from the HDC1080 sensor.
 *
 * @param[out] temperature Pointer to store the read temperature value.
 */
void get_temperature_reading(float *temperature) {
  if (temperature == NULL ) {
    LogError( ("Invalid NULL pointer for temperature read.") );
    return;
  }
  hdc1080_read_temperature(&hi2c1, temperature);
  LogInfo( ("Temperature: %.2f C,", *temperature) );
}


/**
 * @brief Reads the current humidity from the HDC1080 sensor.
 *
 * @param[out] humidity Pointer to store the read humidity value.
 */
void get_humidity_reading(uint8_t *humidity) {
  if ( humidity == NULL) {
    LogError( ("Invalid NULL pointer for humidity read.") );
    return;
  }
  hdc1080_read_humidity(&hi2c1, humidity);
  LogInfo( ("Humidity: %u%%", *humidity) );
}
