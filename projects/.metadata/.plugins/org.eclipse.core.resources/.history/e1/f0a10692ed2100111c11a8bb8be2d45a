/*
 * oled_util.c
 *
 *  Created on: Feb 18, 2025
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */

/* Includes ------------------------------------------------------------------*/
#include "oled_util.h"
#include "sh1106.h"
#include "fonts.h"

extern SPI_HandleTypeDef hspi1;

void oled_show_message(const char *line1, const char *line2)
{
  sh1106_clear();
  sh1106_gotoXY(0, 1);
  sh1106_puts((char *)line1, &Font_7x10, 1);
  sh1106_gotoXY(0, 12);
  sh1106_puts((char *)line2, &Font_7x10, 1);
  sh1106_update_screen(&hspi1);
}

void oled_show_status(const char *status)
{
  sh1106_clear();
  sh1106_gotoXY(0, 1);
  sh1106_puts("Status:", &Font_7x10, 1);
  sh1106_gotoXY(0, 16);
  sh1106_puts((char *)status, &Font_7x10, 1);
  sh1106_update_screen(&hspi1);
}
