/*
 * app.c
 *
 *  Created on: Feb 19, 2026
 *      Author: bhara
 */

#include "main.h"
#include "app.h"
#include "application_config.h"
#include "esp32_at.h"
#include "sh1106.h"
#include "fonts.h"
#include "parser.h"
#include "scroll.h"

/** UART interface used for ESP32 communication */
extern UART_HandleTypeDef huart1;

/** SPI interface used for OLED display */
extern SPI_HandleTypeDef  hspi1;

/**
 * @brief Application state machine states.
 */
typedef enum {
    STATE_STARTUP = 0, /**< Display startup screen */
    STATE_MENU,        /**< Section selection menu */
    STATE_FETCH,       /**< Fetch news from server */
    STATE_SCROLL,      /**< Display scrolling news ticker */
    STATE_ERROR        /**< Error state */
} app_state_t;

/**
 * @brief Menu selection states representing news sections.
 */
typedef enum {
    MENU_HOME = 0,   /**< Home news */
    MENU_US,         /**< US news */
    MENU_SCIENCE,    /**< Science news */
    MENU_ARTS,       /**< Arts news */
    MENU_WORLD,      /**< World news */
} menu_state_t;

/** Total number of menu items */
#define MENU_COUNT 5

/** Starting Y position for menu rendering */
#define MENU_START_Y         5U

/**
 * @brief Display names for each news section.
 */
static const char *const g_sections[MENU_COUNT] = {
    "HOME","US","SCIENCE","ARTS","WORLD"
};

/**
 * @brief API endpoints corresponding to each section.
 */
static const char *const g_endpoints[MENU_COUNT] = {
    "home","us","science","arts","world"
};

/** Current application state */
static app_state_t  g_app_state  = STATE_STARTUP;

/** Current selected menu item */
static menu_state_t g_menu_state = MENU_HOME;

/** Button debounce time in milliseconds */
#define BUTTON_DEBOUNCE_MS   200U

/** Last tick time for LEFT button press */
static uint32_t g_last_tick_left  = 0;

/** Last tick time for RIGHT button press */
static uint32_t g_last_tick_right = 0;

/** Receive buffer size for HTTP response */
#define RECV_BUF_SIZE             6144U

/** Length of received data */
static uint32_t g_ret_length = 0U;

/** HTTP response buffer */
static char g_recv_buf[RECV_BUF_SIZE];


/* -------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS                                                       */
/* -------------------------------------------------------------------------- */

/**
 * @brief Display the startup screen.
 */
static void show_startup_screen(void);

/**
 * @brief Draw a single menu item.
 *
 * @param text Menu item text
 * @param y Vertical display position
 * @param font Font used for rendering
 * @param selected Selection flag
 * @param box_width Width of selection box
 */
static void draw_menu_item(const char *text, uint8_t y, FontDef_t font,
                           uint8_t selected, uint8_t box_width);

/**
 * @brief Fetch news for the selected section.
 */
static void fetch_news_section(void);

/**
 * @brief Send HTTP request and receive news data.
 *
 * @param request HTTP request string
 * @return true if successful, false otherwise
 */
static bool get_news_from(const char *request);

/**
 * @brief Display selected section title before fetching news.
 */
static void display_section_title(void);


/* -------------------------------------------------------------------------- */
/* SERVER CONNECTION                                                          */
/* -------------------------------------------------------------------------- */

/**
 * @brief Establish TCP connection to the news API server.
 *
 * @return 0 if successful, -1 otherwise
 */
int connect_to_server(void) {
  esp32_connection_info_t connection_info = {
    .connection_type = ESP32_TCP_CONNECTION,
    .ip_address = (uint8_t *)API_SERVER,
    .is_server = ESP32_FALSE,
    .port = (uint32_t)API_PORT
  };
  return (esp32_establish_connection(&connection_info) == ESP32_OK) ? 0 : -1;
}


/* -------------------------------------------------------------------------- */
/* STARTUP                                                                    */
/* -------------------------------------------------------------------------- */

/**
 * @brief Display the startup splash screen.
 *
 * Shows the application title for 2 seconds.
 */
static void show_startup_screen(void)
{
    sh1106_clear();
    sh1106_gotoXY(4, 24);
    sh1106_puts("News Ticker", &Font_11x18, 1);
    sh1106_update_screen(&hspi1);
    HAL_Delay(2000);
}


/* -------------------------------------------------------------------------- */
/* MENU                                                                       */
/* -------------------------------------------------------------------------- */

/**
 * @brief Draw a selectable menu item on the OLED display.
 */
static void draw_menu_item(const char *text,
                           uint8_t y,
                           FontDef_t font,
                           uint8_t selected,
                           uint8_t box_width)
{
    uint8_t box_height = font.FontHeight;
    uint8_t x = (SH1106_WIDTH - box_width) / 2;

    if (selected) {
        for (uint8_t i = 0; i < box_width; i++)
            for (uint8_t j = 0; j < box_height; j++)
                sh1106_draw_pixel(x + i, y + j, 1);
    }

    uint8_t text_width = strlen(text) * font.FontWidth;
    uint8_t text_x = x + (box_width - text_width) / 2;

    sh1106_gotoXY(text_x, y);
    sh1106_puts((char*)text, &font, selected ? 0 : 1);
}



/**
 * @brief Display the section selection menu.
 */
static void show_menu(void)
{
    sh1106_clear();

    uint8_t max_chars = 0;
    for (uint8_t i = 0; i < MENU_COUNT; i++) {
        uint8_t len = strlen(g_sections[i]);
        if (len > max_chars) max_chars = len;
    }

    max_chars += 2;
    uint8_t box_width = max_chars * Font_7x10.FontWidth;

    for (uint8_t i = 0; i < MENU_COUNT; i++) {
        uint8_t y = MENU_START_Y + i * Font_7x10.FontHeight;
        draw_menu_item(g_sections[i], y, Font_7x10,
                       (g_menu_state == i), box_width);
    }

    sh1106_update_screen(&hspi1);
}


/* -------------------------------------------------------------------------- */
/* FETCH                                                                      */
/* -------------------------------------------------------------------------- */

/**
 * @brief Display selected section name before fetching news.
 */
static void display_section_title(void)
{
    sh1106_clear();

    const char *text = g_sections[g_menu_state];

    uint8_t text_width  = (uint8_t)(strlen(text) * Font_11x18.FontWidth);
    uint8_t text_height = Font_11x18.FontHeight;

    uint8_t x = (uint8_t)((SH1106_WIDTH  - text_width)  / 2U);
    uint8_t y = (uint8_t)((SH1106_HEIGHT - text_height) / 2U);

    sh1106_gotoXY(x, y);
    sh1106_puts((char*)text, &Font_11x18, 1);
    sh1106_update_screen(&hspi1);
}


/**
 * @brief Construct HTTP GET request for the NYTimes API.
 *
 * @param buffer Output request buffer
 * @param buffer_size Size of buffer
 * @param endpoint API endpoint (home/us/science etc.)
 */
void construct_api_request(char *buffer,
                           size_t buffer_size,
                           const char *endpoint)
{
    snprintf(buffer, buffer_size,
        "GET /svc/topstories/v2/%s.json?api-key=%s HTTP/1.1\r\n"
        "Host: api.nytimes.com\r\n"
        "User-Agent: MyNewsApp/1.0\r\n"
        "Accept: application/json\r\n"
        "Range: bytes=0-5000\r\n\r\n",
        endpoint, API_KEY);
}


/**
 * @brief Send HTTP request and receive response from server.
 *
 * @param request HTTP request string
 * @return true if communication succeeds
 */
static bool get_news_from(const char *request)
{
    if (esp32_send_data((uint8_t*)request, strlen(request)) != ESP32_OK)
        return false;

    if (esp32_recv_data((uint8_t*)g_recv_buf,
                        sizeof(g_recv_buf),
                        &g_ret_length) != ESP32_OK)
        return false;

    return true;
}


/**
 * @brief Fetch news for the currently selected menu section.
 */
static void fetch_news_section(void)
{
    char request_buffer[512];
    display_section_title();

    construct_api_request(request_buffer,
                              sizeof(request_buffer),
                              g_endpoints[g_menu_state]);

    if (!get_news_from(request_buffer)) {
      g_app_state = STATE_ERROR;
      return;
    }

    parse_titles_and_abstracts(g_recv_buf);
    scroll_set_section(g_sections[g_menu_state]);

    scroll_reinit();

    g_app_state = (title_count > 0) ? STATE_SCROLL : STATE_ERROR;
}


/* -------------------------------------------------------------------------- */
/* BUTTON HANDLING                                                            */
/* -------------------------------------------------------------------------- */

/**
 * @brief Handle button input with debounce logic.
 *
 * LEFT button: Navigate menu or exit scroll screen
 * RIGHT button: Select menu item or exit scroll screen
 */
static void handle_buttons(void)
{
    uint32_t now = HAL_GetTick();

    /* LEFT button */
    if (HAL_GPIO_ReadPin(LEFT_BTN_GPIO_Port, LEFT_BTN_Pin) == GPIO_PIN_RESET)
    {
        if ((now - g_last_tick_left) > BUTTON_DEBOUNCE_MS) {

            g_last_tick_left = now;

            if (g_app_state == STATE_MENU) {
                g_menu_state = (g_menu_state + 1) % MENU_COUNT;
                show_menu();
            }
            else if (g_app_state == STATE_SCROLL) {
                g_app_state = STATE_MENU;
            }
        }
    }

    /* RIGHT button */
    if (HAL_GPIO_ReadPin(RIGHT_BTN_GPIO_Port, RIGHT_BTN_Pin) == GPIO_PIN_RESET)
    {
        if ((now - g_last_tick_right) > BUTTON_DEBOUNCE_MS) {

            g_last_tick_right = now;

            if (g_app_state == STATE_MENU) {
                g_app_state = STATE_FETCH;
            }
            else if (g_app_state == STATE_SCROLL) {
                g_app_state = STATE_MENU;
            }
        }
    }
}


/* -------------------------------------------------------------------------- */
/* MAIN APPLICATION FSM                                                       */
/* -------------------------------------------------------------------------- */

/**
 * @brief Main application task.
 *
 * This function runs the application finite state machine (FSM).
 * It should be called continuously inside the main loop.
 */
void main_app(void)
{
    handle_buttons();

    switch(g_app_state)
    {
      case STATE_STARTUP:
          show_startup_screen();
          g_app_state=STATE_MENU;
          break;

      case STATE_MENU:
          show_menu();
          break;

      case STATE_FETCH:
          fetch_news_section();
          break;

      case STATE_SCROLL:
          scroll_update();
          break;

      case STATE_ERROR:
          sh1106_clear();
          sh1106_gotoXY(10, 24);
          sh1106_puts("No News", &Font_11x18, 1);
          sh1106_update_screen(&hspi1);
          HAL_Delay(2000);
          g_app_state = STATE_MENU;
          break;

      default:
          g_app_state = STATE_MENU;
          break;
    }
}
