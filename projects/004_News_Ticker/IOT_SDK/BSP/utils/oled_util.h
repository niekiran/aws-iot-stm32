/*
 * oled_util.h
 *
 *  Created on: Feb 18, 2025
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */

#ifndef __OLED_UTIL_H
#define __OLED_UTIL_H

/* Includes ------------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  Display a two-line message on the OLED screen.
  * @note   Each line can fit approximately 18 characters (Font_7x10).
  * @note   Example usage:
  *         oled_show_message("Subscribing...", "razorpay/payments");
  *         // Line1: "Subscribing..."
  *         // Line2: "razorpay/payments"
  *
  * @param  line1: Pointer to the string for the first line.
  * @param  line2: Pointer to the string for the second line.
  * @retval None
  */
void oled_show_message(const char *line1, const char *line2);

/**
  * @brief  Display a single status message on the OLED, prefixed with "Status:".
  * @note   The display clears before updating.
  * @note   Example usage:
  *         oled_show_status("Wi-Fi Init...");
  *         oled_show_status("MQTT Connected");
  *
  * @note   Output on OLED (example):
  *         Line 1: Status:
  *         Line 2: Wi-Fi Init...
  *
  *         Output on OLED (example):
  *         Line 1: Status:
  *         Line 2: MQTT Connected
  *
  * @param  status: Pointer to the status string to be displayed.
  * @retval None
  */

void oled_show_status(const char *status);

#endif /* __OLED_UTIL_H */
