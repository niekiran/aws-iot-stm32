/**
 * @file scroll.c
 * @brief News ticker scrolling and parsing logic for OLED display.
 *
 * @author Shreyas Acharya, BHARATI SOFTWARE
 * @date Jun 16, 2025
 */

#include "scroll.h"
#include "sh1106.h"
#include "fonts.h"
#include "parser.h"
#include "main.h"
#include "app.h"

#include <string.h>
#include <stdio.h>

/* ---------------- CONFIG ---------------- */

/** Scroll update interval in milliseconds */
#define SCROLL_INTERVAL_MS        25U

/** Maximum buffer size for the scrolling text */
#define SCROLL_TEXT_BUF_SIZE      4096U

/** Separator used between titles */
#define TITLE_SEP                 "     |     "

/** Separator between title and abstract */
#define TITLE_ABS_SEP             " - "

/** Separator between news items */
#define ITEM_SEP                  "     "

/** Ticker mode: show titles only */
#define TICKER_MODE_TITLES_ONLY   0

/** Ticker mode: show title and abstract */
#define TICKER_MODE_TITLE_ABSTRACT 1

#ifndef TICKER_MODE
#define TICKER_MODE TICKER_MODE_TITLES_ONLY
#endif

/* ------------- INTERNAL STATE ------------ */

/** Last scroll update timestamp */
static uint32_t s_last_scroll_tick = 0;

/** Flag requesting ticker reinitialization */
static uint8_t  s_reinit_request = 1;

/** Buffer containing the full scroll text */
static char     s_scroll_text[SCROLL_TEXT_BUF_SIZE];

/** Total pixel length of the scroll text */
static int16_t  s_pixel_len = 0;

/** Current x position of scrolling text */
static int16_t  s_x_pos = SH1106_WIDTH;

/** Initialization flag */
static uint8_t  s_initialized = 0;

/** Current news section name (HOME / US / SCIENCE etc.) */
static char s_section[16] = "HOME";

/** SPI handle used for OLED display communication */
extern SPI_HandleTypeDef hspi1;

/* ------------------------------------------------ */

/**
 * @brief Set the current news section name.
 *
 * This section name is displayed in the header
 * of the OLED screen.
 *
 * @param section_name Pointer to section name string
 */
void scroll_set_section(const char *section_name)
{
    if (!section_name) return;

    strncpy(s_section, section_name, sizeof(s_section) - 1);
    s_section[sizeof(s_section) - 1] = '\0';
}

/* ------------------------------------------------ */

/**
 * @brief Draw the header on the OLED display.
 *
 * Displays the current section name followed by
 * the word "NEWS".
 */
static void draw_header(void)
{
    char header[24];

    snprintf(header, sizeof(header), "%s NEWS:", s_section);

    sh1106_gotoXY(2, 10);
    sh1106_puts(header, &Font_7x10, 1);
}

/* ------------------------------------------------ */

/**
 * @brief Build the scrolling text string.
 *
 * This function concatenates article titles (and optionally
 * abstracts) into a single buffer used by the ticker.
 *
 * The output is stored in the global buffer `s_scroll_text`.
 */
static void build_scroll_string(void)
{
    s_scroll_text[0] = '\0';
    size_t total_len = 0;

    for (int i = 0; i < title_count; i++) {

        const char *t = articles[i].title;
        const char *a = articles[i].abstract;

        if (!t || t[0] == '\0')
            continue;

        if (!a) a = "";

        size_t remaining =
            (SCROLL_TEXT_BUF_SIZE - 1 > total_len) ?
            (SCROLL_TEXT_BUF_SIZE - 1 - total_len) : 0;

        if (remaining == 0)
            break;

#if (TICKER_MODE == TICKER_MODE_TITLES_ONLY)

        int n = snprintf(s_scroll_text + total_len,
                         remaining + 1,
                         "%s%s", t, TITLE_SEP);

#else

        int n = snprintf(s_scroll_text + total_len,
                         remaining + 1,
                         "%s%s%s%s",
                         t, TITLE_ABS_SEP,
                         a, ITEM_SEP);

#endif

        if (n <= 0) break;

        if ((size_t)n > remaining) {
            total_len = SCROLL_TEXT_BUF_SIZE - 1;
            break;
        }

        total_len += n;
    }

    /* Calculate pixel length for scrolling */
    s_pixel_len = strlen(s_scroll_text) * Font_11x18.FontWidth;
}

/* ------------------------------------------------ */

/**
 * @brief Draw visible portion of scrolling text.
 *
 * Only characters that fall inside the OLED display
 * boundaries are rendered. Also implements seamless
 * wrapping when the text reaches the end.
 */
static void draw_visible_text(void)
{
    int16_t text_len = strlen(s_scroll_text);

    int16_t start_idx =
        (s_x_pos < 0) ? ((-s_x_pos) / Font_11x18.FontWidth) : 0;

    /* visible characters */
    for (int16_t i = start_idx; i < text_len; i++) {

        int16_t char_x =
            s_x_pos + i * Font_11x18.FontWidth;

        if ((char_x >= 0) &&
            (char_x < (SH1106_WIDTH - Font_11x18.FontWidth))) {

            sh1106_gotoXY(char_x + 1, 24);
            sh1106_putc(s_scroll_text[i], &Font_11x18, 1);
        }
    }

    /* seamless loop */
    int16_t next_x = s_x_pos + s_pixel_len;

    if (next_x < SH1106_WIDTH) {

        for (int16_t i = 0; i < text_len; i++) {

            int16_t char_x =
                next_x + i * Font_11x18.FontWidth;

            if ((char_x >= 0) &&
                (char_x < (SH1106_WIDTH - Font_11x18.FontWidth))) {

                sh1106_gotoXY(char_x + 1, 24);
                sh1106_putc(s_scroll_text[i], &Font_11x18, 1);
            }
        }
    }
}

/* ------------------------------------------------ */

/**
 * @brief Request ticker reinitialization.
 *
 * Forces the scroll module to rebuild the scroll string
 * during the next update cycle.
 */
void scroll_reinit(void)
{
    s_reinit_request = 1;
}

/* ------------------------------------------------ */

/**
 * @brief Update the scrolling ticker.
 *
 * This function should be called periodically inside the
 * main application loop. It:
 *
 * - Rebuilds the scroll string when needed
 * - Updates the scrolling position
 * - Draws header and visible text
 * - Refreshes the OLED display
 */
void scroll_update(void)
{
    uint32_t now = HAL_GetTick();

    if (s_reinit_request) {
        s_reinit_request = 0;
        s_initialized = 0;
    }

    if (!s_initialized) {
        build_scroll_string();
        s_x_pos = SH1106_WIDTH;
        s_initialized = 1;
    }

    if ((now - s_last_scroll_tick) >= SCROLL_INTERVAL_MS) {

        s_last_scroll_tick = now;

        sh1106_clear();

        draw_header();
        draw_visible_text();

        sh1106_update_screen(&hspi1);

        /* move left 1 pixel */
        s_x_pos--;

        if (s_x_pos <= -s_pixel_len) {
            s_x_pos = SH1106_WIDTH - Font_11x18.FontWidth;
        }
    }
}
