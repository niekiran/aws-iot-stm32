/*
 * hdc1080.c
 *
 *  Created on: Feb 17, 2025
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */

#include "hdc1080.h"
#include "main.h"

/**
 * @brief Initializes the HDC1080 temperature and humidity sensor.
 *
 * Configures the sensor with the specified temperature and humidity resolution.
 *
 * @param[in] hi2c_x Pointer to the I2C handle.
 * @param[in] temp_resolution_x_bit Temperature resolution (11-bit or 14-bit).
 * @param[in] humi_resolution_x_bit Humidity resolution (8-bit, 11-bit, or 14-bit).
 * @return hdc_status_t Returns HDC_OK on success, HDC_ERROR on failure.
 */
hdc_status_t hdc1080_init(I2C_HandleTypeDef *hi2c_x,
    temp_resolution_t temp_resolution_x_bit,
    humi_resolution_t humi_resolution_x_bit) {
  /* Temperature and Humidity are acquired in sequence, Temperature first
   * Default: Temperature resolution = 14-bit, Humidity resolution = 14-bit
   */

  /* Set the acquisition mode to measure both temperature and humidity by setting Bit[12] to 1 */
  uint16_t config_reg_value = 0x1000;
  uint8_t data_send[2];

  /* Set temperature resolution (default: 14-bit) */
  if (temp_resolution_x_bit == Temperature_Resolution_11_bit) {
    config_reg_value |= (1 << 10); // Set to 11-bit resolution
  }

  /* Set humidity resolution */
  switch (humi_resolution_x_bit) {
  case Humidity_Resolution_14_bit:
    break;
  case Humidity_Resolution_11_bit:
    config_reg_value |= (1 << 8);
    break;
  case Humidity_Resolution_8_bit:
    config_reg_value |= (1 << 9);
    break;
  }

  /* Prepare data to send */
  data_send[0] = (config_reg_value >> 8);
  data_send[1] = (config_reg_value & 0x00FF);

  /* Write configuration to HDC1080 sensor */
  if (HAL_I2C_Mem_Write(hi2c_x, HDC_1080_ADD << 1, Configuration_register_add,
      I2C_MEMADD_SIZE_8BIT, data_send, 2, 1000) != HAL_OK) {
    return HDC_ERROR;
  }

  return HDC_OK;
}

/**
 * @brief Reads the current temperature from the HDC1080 sensor.
 *
 * @param[in] hi2c_x Pointer to the I2C handle.
 * @param[out] temperature Pointer to store the temperature value in Celsius.
 * @return hdc_status_t Returns HDC_OK on success, HDC_ERROR on failure.
 */
hdc_status_t hdc1080_read_temperature(I2C_HandleTypeDef *hi2c_x,
    float *temperature) {
  if (temperature == NULL) {
    return HDC_ERROR;
  }

  uint8_t receive_data[2];
  uint8_t send_data = Temperature_register_add;
  uint16_t temp_x;

  /* Request temperature measurement */
  if (HAL_I2C_Master_Transmit(hi2c_x, HDC_1080_ADD << 1, &send_data, 1, 1000)
      != HAL_OK) {
    return HDC_ERROR;
  }

  /* Delay to allow measurement to complete (Max 7ms, but 15ms for accuracy) */
  HAL_Delay(15);

  /* Read 2 bytes for temperature */
  if (HAL_I2C_Master_Receive(hi2c_x, HDC_1080_ADD << 1, receive_data, 2, 1000)
      != HAL_OK) {
    return HDC_ERROR;
  }

  /* Convert raw ADC value to temperature in Celsius */
  temp_x = ((receive_data[0] << 8) | receive_data[1]);
  *temperature = ((temp_x / 65536.0) * 165.0) - 40.0;

  return HDC_OK;
}

/**
 * @brief Reads the current humidity from the HDC1080 sensor.
 *
 * @param[in] hi2c_x Pointer to the I2C handle.
 * @param[out] humidity Pointer to store the humidity value in percentage (%RH).
 * @return hdc_status_t Returns HDC_OK on success, HDC_ERROR on failure.
 */
hdc_status_t hdc1080_read_humidity(I2C_HandleTypeDef *hi2c_x, uint8_t *humidity) {
  if (humidity == NULL) {
    return HDC_ERROR;
  }

  uint8_t receive_data[2];
  uint8_t send_data = Humidity_register_add;
  uint16_t humi_x;

  /* Request humidity measurement */
  if (HAL_I2C_Master_Transmit(hi2c_x, HDC_1080_ADD << 1, &send_data, 1, 1000)
      != HAL_OK) {
    return HDC_ERROR;
  }

  /* Delay to allow measurement to complete */
  HAL_Delay(15);

  /* Read 2 bytes for humidity */
  if (HAL_I2C_Master_Receive(hi2c_x, HDC_1080_ADD << 1, receive_data, 2, 1000)
      != HAL_OK) {
    return HDC_ERROR;
  }

  /* Convert raw ADC value to humidity in %RH */
  humi_x = ((receive_data[0] << 8) | receive_data[1]);
  *humidity = (uint8_t) ((humi_x / 65536.0) * 100.0);

  return HDC_OK;
}

/**
 * @brief Starts a combined temperature and humidity measurement.
 *
 * This function triggers a temperature and humidity measurement and reads both values in a single I2C transaction.
 *
 * @param[in] hi2c_x Pointer to the I2C handle.
 * @param[out] temperature Pointer to store the temperature value in Celsius.
 * @param[out] humidity Pointer to store the humidity value in percentage (%RH).
 * @return hdc_status_t Returns HDC_OK on success, HDC_ERROR on failure.
 */
hdc_status_t hdc1080_start_measurement(I2C_HandleTypeDef *hi2c_x,
    float *temperature, uint8_t *humidity) {
  uint8_t receive_data[4];
  uint16_t temp_x, humi_x;
  uint8_t send_data = Temperature_register_add;

  /* Request combined temperature and humidity measurement */
  if (HAL_I2C_Master_Transmit(hi2c_x, HDC_1080_ADD << 1, &send_data, 1, 1000)
      != HAL_OK) {
    return HDC_ERROR;
  }

  /* Delay for conversion to complete (Datasheet max: 7ms, but using 15ms for accuracy) */
  HAL_Delay(15);

  /* Read 4 bytes (2 for temperature, 2 for humidity) */
  if (HAL_I2C_Master_Receive(hi2c_x, HDC_1080_ADD << 1, receive_data, 4, 1000)
      != HAL_OK) {
    return HDC_ERROR;
  }

  /* Convert raw ADC values */
  temp_x = ((receive_data[0] << 8) | receive_data[1]);
  humi_x = ((receive_data[2] << 8) | receive_data[3]);

  /* Convert to human-readable values */
  *temperature = ((temp_x / 65536.0) * 165.0) - 40.0;
  *humidity = (uint8_t) ((humi_x / 65536.0) * 100.0);

  return HDC_OK;
}

