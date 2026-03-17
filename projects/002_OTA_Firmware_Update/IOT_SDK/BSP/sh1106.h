/*
 * sh1106.h
 *
 * Created on: Oct 2, 2024
 * Author: Shreyas Acharya, BHARATI SOFTWARE
 *
 */

#ifndef SH1106_H_
#define SH1106_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include "fonts.h"

// Select the communication interface.
// Exactly one should be enabled (either by uncommenting here or via project defines).
//#define SH1106_USE_I2C        // Use I2C transport for SH1106
#define SH1106_USE_SPI        // Use SPI transport for SH1106

#define SH1106_WIDTH  128
#define SH1106_HEIGHT 64

// Framebuffer size in bytes: 1 bit per pixel => (W * H) / 8
#define SH1106_BUFFER_SIZE (SH1106_WIDTH * SH1106_HEIGHT / 8)

/**
 * @brief Driver return/status codes.
 */
typedef enum {
    SH1106_OK = 0,
    SH1106_ERR = 1,
    SH1106_I2C_ERR = 2,
    SH1106_SPI_ERR = 3
} sh1106_t;

/**
 * @brief Pixel color definition for 1-bit framebuffer.
 * BLACK = 0 clears pixel, WHITE = 1 sets pixel.
 */
typedef enum {
    SH1106_COLOR_BLACK = 0,
    SH1106_COLOR_WHITE = 1
} sh1106_color_t;

/**
 * @brief SH1106 context/state structure.
 *
 * CurrentX/CurrentY : cursor position for text printing
 * Inverted          : non-zero => invert drawn pixels (implementation-dependent)
 * Initialized       : non-zero => init sequence has been executed
 * Buffer            : local framebuffer; drawing APIs modify this buffer
 *                   then sh1106_update_screen() pushes it to the display.
 */
typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t  Inverted;
    uint8_t  Initialized;
    uint8_t  Buffer[SH1106_BUFFER_SIZE];
} SH1106_t;

/**
 * @brief Initialize SH1106 display and internal state.
 * @param sh_handle Pointer to transport handle/context (e.g., I2C/SPI handle or a custom struct)
 * @return sh1106_t status code
 */
sh1106_t sh1106_init(void* sh_handle);

/**
 * @brief Draw a monochrome bitmap into the local framebuffer.
 * @param x,y Top-left destination in pixels
 * @param bitmap Pointer to bitmap data (format depends on implementation)
 * @param w,h Bitmap width/height in pixels
 * @param color Foreground color (typically WHITE draws set bits, BLACK may draw cleared bits)
 */
void sh1106_draw_bitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color);

/**
 * @brief Transfer the local framebuffer to the display over the selected interface.
 * @param sh_handle Pointer to transport handle/context used by the low-level write routine
 * @return sh1106_t status code
 */
sh1106_t sh1106_update_screen(void* sh_handle);

/**
 * @brief Draw a rectangle outline into the local framebuffer.
 * @param sh_handle Pointer to transport/context (may be unused by pure buffer draw)
 * @param x,y Top-left corner
 * @param w,h Width and height in pixels
 * @param color Pixel color
 * @return sh1106_t status code
 */
sh1106_t sh1106_draw_rect(void* sh_handle, uint8_t x, uint8_t y, uint8_t w, uint8_t h, sh1106_color_t color);

/**
 * @brief Set/clear a single pixel in the local framebuffer.
 * @param x Pixel X coordinate (0..127)
 * @param y Pixel Y coordinate (0..63)
 * @param color SH1106_COLOR_WHITE to set, SH1106_COLOR_BLACK to clear
 * @return sh1106_t status code
 */
sh1106_t sh1106_draw_pixel(uint8_t x, uint8_t y, sh1106_color_t color);

/**
 * @brief Set the cursor position used by sh1106_putc()/sh1106_puts().
 * @param x X position in pixels
 * @param y Y position in pixels
 * @return sh1106_t status code
 */
sh1106_t sh1106_gotoXY(uint8_t x, uint8_t y);

/**
 * @brief Draw a line into the local framebuffer.
 * @param sh_handle Pointer to transport/context (may be unused by pure buffer draw, but kept for API symmetry)
 * @param x1,y1 Line start point
 * @param x2,y2 Line end point
 * @param color Pixel color
 * @return sh1106_t status code
 */
sh1106_t sh1106_draw_line(void* sh_handle, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, sh1106_color_t color);

/**
 * @brief Draw a circle outline into the local framebuffer.
 * @param sh_handle Pointer to transport/context (may be unused by pure buffer draw)
 * @param x0,y0 Circle center
 * @param r Circle radius
 * @param color Pixel color
 * @return sh1106_t status code
 */
sh1106_t sh1106_draw_circle(void* sh_handle, uint8_t x0, uint8_t y0, uint8_t r, sh1106_color_t color);

/**
 * @brief Render a single character using the given font at the current cursor position.
 *        Advances the cursor after drawing.
 * @param ch Character to print
 * @param Font Pointer to font definition (width/height/bitmaps)
 * @param color Pixel color
 * @return Printed character (commonly ch or 0 on failure, depending on implementation)
 */
char sh1106_putc(char ch, FontDef_t* Font, sh1106_color_t color);

/**
 * @brief Render a null-terminated string starting at the current cursor position.
 *        Advances cursor as characters are drawn (may wrap depending on implementation).
 * @param str Null-terminated ASCII string
 * @param Font Pointer to font definition
 * @param color Pixel color
 * @return Last printed character or status-like char (implementation-dependent)
 */
char sh1106_puts(char* str, FontDef_t* Font, sh1106_color_t color);


/**
 * @brief Clear the local framebuffer (does NOT automatically update the display).
 *        Call sh1106_update_screen() after clearing to reflect changes on the panel.
 * @return sh1106_t status code
 */
sh1106_t sh1106_clear(void);

#endif /* SH1106_H_ */
