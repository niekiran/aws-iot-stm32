/*
 * sh1106.c
 *
 * Created on: Oct 2, 2024
 * Author: Shreyas Acharya, BHARATI SOFTWARE
 */

#include <sh1106.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"

// SH1106 object
static SH1106_t SH1106;

#if !defined(SH1106_USE_I2C) && !defined(SH1106_USE_SPI)
#error "Define either SH1106_USE_I2C or SH1106_USE_SPI in project settings or sh1106.h"
#endif

#ifdef SH1106_USE_SPI

// SPI pin definitions (adjust as per your hardware)
#define SH1106_CS_PORT   GPIOB
#define SH1106_CS_PIN    GPIO_PIN_3
#define SH1106_DC_PORT   GPIOA
#define SH1106_DC_PIN    GPIO_PIN_10
#define SH1106_RST_PORT  GPIOA
#define SH1106_RST_PIN   GPIO_PIN_8

// Helper macros for GPIO control
#define SH1106_CS_LOW()  HAL_GPIO_WritePin(SH1106_CS_PORT, SH1106_CS_PIN, GPIO_PIN_RESET)
#define SH1106_CS_HIGH() HAL_GPIO_WritePin(SH1106_CS_PORT, SH1106_CS_PIN, GPIO_PIN_SET)
#define SH1106_DC_LOW()  HAL_GPIO_WritePin(SH1106_DC_PORT, SH1106_DC_PIN, GPIO_PIN_RESET)
#define SH1106_DC_HIGH() HAL_GPIO_WritePin(SH1106_DC_PORT, SH1106_DC_PIN, GPIO_PIN_SET)
#define SH1106_RST_LOW() HAL_GPIO_WritePin(SH1106_RST_PORT, SH1106_RST_PIN, GPIO_PIN_RESET)
#define SH1106_RST_HIGH() HAL_GPIO_WritePin(SH1106_RST_PORT, SH1106_RST_PIN, GPIO_PIN_SET)
#endif

// Initialization commands (placed early for clarity)
static const uint8_t init_cmds[] = {
    0xAE,         /* Display off */
    0xD5, 0x80,   /* Set display clock divide ratio, suggested value 0x80 */
    0xA8, 0x3F,   /* Set multiplex, 1/64 duty */
    0xD3, 0x00,   /* Set display offset, no offset */
    0x40,         /* Set start line at 0 */
    0x8D, 0x14,   /* Charge pump, enable charge pump */
    0x20, 0x00,   /* Memory mode, horizontal addressing mode */
    0xA1,         /* Set segment remap */
    0xC8,         /* Set COM output scan direction */
    0xDA, 0x12,   /* Set COM pins hardware configuration */
    0x81, 0x7F,   /* Set contrast control */
    0xD9, 0xF1,   /* Set pre-charge period */
    0xDB, 0x40,   /* Set VCOMH deselect level */
    0xA4,         /* Enable display all on */
    0xA6,         /* Normal display */
    0xAF          /* Display on */
};

// Function to write a command to the SH1106
sh1106_t sh1106_write_command(void* sh_handle, uint8_t cmd) {
#ifdef SH1106_USE_I2C
    I2C_HandleTypeDef* sh_hi2c = (I2C_HandleTypeDef*)sh_handle;
    uint8_t buffer[2] = {0x00, cmd};
    if (HAL_I2C_Master_Transmit(sh_hi2c, (uint16_t)(0x3C << 1), buffer, 2, HAL_MAX_DELAY) != HAL_OK) {
        return SH1106_I2C_ERR;
    }
#endif

#ifdef SH1106_USE_SPI
    SPI_HandleTypeDef* sh_hspi = (SPI_HandleTypeDef*)sh_handle;
    SH1106_CS_LOW();
    SH1106_DC_LOW();
    if (HAL_SPI_Transmit(sh_hspi, &cmd, 1, HAL_MAX_DELAY) != HAL_OK) {
        SH1106_CS_HIGH();
        return SH1106_SPI_ERR;
    }
    SH1106_CS_HIGH();
#endif
    return SH1106_OK;
}

// Function to write data to the SH1106
static sh1106_t sh1106_write_data(void* sh_handle, uint8_t* data, uint16_t len) {
#ifdef SH1106_USE_I2C
    I2C_HandleTypeDef* sh_hi2c = (I2C_HandleTypeDef*)sh_handle;
    if (HAL_I2C_Mem_Write(sh_hi2c, (uint16_t)(0x3C << 1), 0x40, 1, data, len, HAL_MAX_DELAY) != HAL_OK) {
        return SH1106_I2C_ERR;
    }
#endif

#ifdef SH1106_USE_SPI
    SPI_HandleTypeDef* sh_hspi = (SPI_HandleTypeDef*)sh_handle;
    SH1106_CS_LOW();
    SH1106_DC_HIGH();
    if (HAL_SPI_Transmit(sh_hspi, data, len, HAL_MAX_DELAY) != HAL_OK) {
        SH1106_CS_HIGH();
        return SH1106_SPI_ERR;
    }
    SH1106_CS_HIGH();
#endif
    return SH1106_OK;
}

// Helper function to send multiple commands
static sh1106_t sh1106_send_commands(void* sh_handle, const uint8_t* cmds, uint8_t cmd_count) {
    for (uint8_t i = 0; i < cmd_count; i++) {
        if (sh1106_write_command(sh_handle, cmds[i]) != SH1106_OK) {
            return SH1106_ERR;
        }
    }
    return SH1106_OK;
}

// Function to initialize the SH1106 display
sh1106_t sh1106_init(void* sh_handle) {
  HAL_Delay(100);

#ifdef SH1106_USE_SPI
  // Hardware reset (if RST pin is used)
  SH1106_RST_LOW();
  HAL_Delay(10);
  SH1106_RST_HIGH();
  HAL_Delay(10);
#endif

  // Initialize SH1106 structure
  SH1106.CurrentX = 0;
  SH1106.CurrentY = 0;
  SH1106.Inverted = 0;
  SH1106.Initialized = 1;

  // Send initialization commands
  return sh1106_send_commands(sh_handle, init_cmds, sizeof(init_cmds));
}

// Function to clear the display buffer
sh1106_t sh1106_clear(void) {
  memset(SH1106.Buffer, 0, sizeof(SH1106.Buffer));
  SH1106.CurrentX = 0;
  SH1106.CurrentY = 0;
  return SH1106_OK;
}

// Function to update the display with the buffer
sh1106_t sh1106_update_screen(void* sh_handle) {
  for (uint8_t page = 0; page < 8; page++) {
    // Set page and column address
    if (sh1106_write_command(sh_handle, 0xB0 + page) != SH1106_OK) return SH1106_ERR;
    if (sh1106_write_command(sh_handle, 0x02) != SH1106_OK) return SH1106_ERR;
    if (sh1106_write_command(sh_handle, 0x10) != SH1106_OK) return SH1106_ERR;

    // Write buffer to display
    if (sh1106_write_data(sh_handle, &SH1106.Buffer[SH1106_WIDTH * page], SH1106_WIDTH) != SH1106_OK) {
#ifdef SH1106_USE_I2C
            return SH1106_I2C_ERR;
#else
            return SH1106_SPI_ERR;
#endif
        }
    }
    return SH1106_OK;
}

// Draw a pixel on the buffer
sh1106_t sh1106_draw_pixel(uint8_t x, uint8_t y, sh1106_color_t color) {
    if (x >= SH1106_WIDTH || y >= SH1106_HEIGHT) {
        return SH1106_ERR;  // Out of bounds
    }

    if (color == SH1106_COLOR_WHITE) {
        SH1106.Buffer[x + (y / 8) * SH1106_WIDTH] |= (1 << (y % 8));
    } else {
        SH1106.Buffer[x + (y / 8) * SH1106_WIDTH] &= ~(1 << (y % 8));
    }
    return SH1106_OK;
}

// Set cursor position
sh1106_t sh1106_gotoXY(uint8_t x, uint8_t y) {
    if (x >= SH1106_WIDTH || y >= SH1106_HEIGHT) {
        return SH1106_ERR;  // Out of bounds
    }

    SH1106.CurrentX = x;
    SH1106.CurrentY = y;
    return SH1106_OK;
}

// Draw a line (Bresenham's algorithm)
sh1106_t sh1106_draw_line(void* sh_handle, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, sh1106_color_t color) {
    int16_t dx = abs(x2 - x1);
    int16_t dy = -abs(y2 - y1);
    int16_t sx = x1 < x2 ? 1 : -1;
    int16_t sy = y1 < y2 ? 1 : -1;
    int16_t err = dx + dy;

    while (1) {
        sh1106_draw_pixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int16_t e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
    return SH1106_OK;
}

// Draw a rectangle
sh1106_t sh1106_draw_rect(void* sh_handle, uint8_t x, uint8_t y, uint8_t w, uint8_t h, sh1106_color_t color) {
    sh1106_draw_line(sh_handle, x, y, x + w, y, color);
    sh1106_draw_line(sh_handle, x + w, y, x + w, y + h, color);
    sh1106_draw_line(sh_handle, x + w, y + h, x, y + h, color);
    sh1106_draw_line(sh_handle, x, y + h, x, y, color);
    return SH1106_OK;
}

// Draw a circle (midpoint circle algorithm)
sh1106_t sh1106_draw_circle(void* sh_handle, uint8_t x0, uint8_t y0, uint8_t r, sh1106_color_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    sh1106_draw_pixel(x0, y0 + r, color);
    sh1106_draw_pixel(x0, y0 - r, color);
    sh1106_draw_pixel(x0 + r, y0, color);
    sh1106_draw_pixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        sh1106_draw_pixel(x0 + x, y0 + y, color);
        sh1106_draw_pixel(x0 - x, y0 + y, color);
        sh1106_draw_pixel(x0 + x, y0 - y, color);
        sh1106_draw_pixel(x0 - x, y0 - y, color);
        sh1106_draw_pixel(x0 + y, y0 + x, color);
        sh1106_draw_pixel(x0 - y, y0 + x, color);
        sh1106_draw_pixel(x0 + y, y0 - x, color);
        sh1106_draw_pixel(x0 - y, y0 - x, color);
    }
    return SH1106_OK;
}

// Write a single character
char sh1106_putc(char ch, FontDef_t* Font, sh1106_color_t color) {
    uint32_t i, b, j;

    // Check available space in LCD
    if (SH1106_WIDTH <= (SH1106.CurrentX + Font->FontWidth) ||
        SH1106_HEIGHT <= (SH1106.CurrentY + Font->FontHeight)) {
        return 0;
    }

    for (i = 0; i < Font->FontHeight; i++) {
        b = Font->data[(ch - 32) * Font->FontHeight + i];
        for (j = 0; j < Font->FontWidth; j++) {
            if ((b << j) & 0x8000) {
                sh1106_draw_pixel(SH1106.CurrentX + j, SH1106.CurrentY + i, (sh1106_color_t)color);
            } else {
                sh1106_draw_pixel(SH1106.CurrentX + j, SH1106.CurrentY + i, (sh1106_color_t)!color);
            }
        }
    }

    SH1106.CurrentX += Font->FontWidth;

    return ch;
}

// Write a string
char sh1106_puts(char* str, FontDef_t* Font, sh1106_color_t color) {
  while (*str) {
    if (sh1106_putc(*str, Font, color) != *str) {
      return *str;
    }
    str++;
  }
  return *str;
}

void sh1106_draw_bitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color)
{

  int16_t byteWidth = (w + 7) / 8;
  uint8_t byte = 0;

  for(int16_t j=0; j<h; j++, y++)
  {
    for(int16_t i=0; i<w; i++)
    {
      if(i & 7)
      {
        byte <<= 1;
      }
      else
      {
        byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
      }
      if(byte & 0x80) sh1106_draw_pixel(x+i, y, color);
    }
  }
}
