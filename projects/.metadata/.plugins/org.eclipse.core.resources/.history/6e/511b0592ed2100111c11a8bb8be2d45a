/*
 * esp32_at_io.c
 *
 * Created on: Dec 27, 2024
 * Author: Shreyas Acharya, BHARATI SOFTWARE
 */

/* Includes ------------------------------------------------------------------*/
#include "esp32_at_io.h"
#include "main.h"
#include <string.h>

/* Private define ------------------------------------------------------------*/
#define RING_BUFFER_SIZE (1024 * 10)

/* Private typedef -----------------------------------------------------------*/
typedef struct {
  const uint8_t data [ RING_BUFFER_SIZE ];
  uint16_t tail;
  uint16_t head;
} ring_buffer_t;

/* Private variables ---------------------------------------------------------*/
ring_buffer_t wifi_rx_buffer;

/* Private function prototypes -----------------------------------------------*/
static void esp32_io_error_handler(void);

/* Exported functions -------------------------------------------------------*/

/**
 * @brief  Initialize the ESP8266 UART interface with DMA and Idle Detection.
 * @retval 0 on success, -1 otherwise.
 */
int8_t esp32_io_init(void) {
  HAL_Delay(2000);

  wifi_rx_buffer.head = 0;
  wifi_rx_buffer.tail = 0;

  // Start UART in DMA mode with Idle line detection
  if ( HAL_UARTEx_ReceiveToIdle_DMA(ESP32_UART_HANDLE, (uint8_t *)wifi_rx_buffer.data,
                                    RING_BUFFER_SIZE) != HAL_OK ) {
    return -1;
  }
  return 0;
}

/**
 * @brief  Deinitialize the ESP8266 UART interface.
 * @retval None.
 */
void esp32_io_deinit(void) {
  HAL_UART_DMAStop(ESP32_UART_HANDLE);
  HAL_UART_DeInit(ESP32_UART_HANDLE);
}

/**
 * @brief  Send data to the ESP8266 module over UART.
 * @param  p_data: Pointer to the data buffer to send.
 * @param  length: Length of the data buffer.
 * @retval 0 on success, -1 otherwise.
 */
int8_t esp32_io_send(uint8_t *p_data, uint32_t length) {
  if ( HAL_UART_Transmit(ESP32_UART_HANDLE, p_data, length, DEFAULT_TIME_OUT) !=
       HAL_OK ) {
    return -1;
  }
  return 0;
}

/**
 * @brief  Receive data from the ESP8266 module over UART.
 * @param  buffer: Pointer to the buffer to store received data.
 * @param  length: Maximum length of the buffer.
 * @retval Number of bytes received.
 */
int32_t esp32_io_recv(uint8_t *buffer, uint32_t length) {
  uint32_t read_data = 0;

  while ( length-- ) {
    uint32_t tick_start = HAL_GetTick();
    do {
      if ( wifi_rx_buffer.head != wifi_rx_buffer.tail ) {
        *buffer++ = wifi_rx_buffer.data [ wifi_rx_buffer.head++ ];
        read_data++;

        if ( wifi_rx_buffer.head >= RING_BUFFER_SIZE ) {
          wifi_rx_buffer.head = 0;
        }
        break;
      }
    } while ( (HAL_GetTick() - tick_start) < DEFAULT_TIME_OUT);
  }
  return read_data;
}

/**
 * @brief  Non-blocking receive data from the ESP8266 module over UART.
 * @param  buffer: Pointer to the buffer to store received data.
 * @param  length: Maximum length of the buffer.
 * @retval Number of bytes received.
 */
int32_t esp32_io_recv_nb(uint8_t *buffer, uint32_t length) {
  uint32_t read_data = 0;

  while ( length-- ) {
      uint32_t tick_start = HAL_GetTick();
      do {
        if ( wifi_rx_buffer.head != wifi_rx_buffer.tail ) {
          *buffer++ = wifi_rx_buffer.data [ wifi_rx_buffer.head++ ];
          read_data++;

          if ( wifi_rx_buffer.head >= RING_BUFFER_SIZE ) {
            wifi_rx_buffer.head = 0;
          }
          break;
        }
      } while ( (HAL_GetTick() - tick_start) < 200);
    }
  return read_data;
}

/**
  * @brief  Reception Event Callback (Rx event notification called after use of advanced reception service).
  * @param  huart UART handle
  * @param  Size  Number of data available in application reception buffer (indicates a position in
  *               reception buffer until which, data are available)
  * @retval None
  */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size) {
  if ( huart == ESP32_UART_HANDLE ) {
    static uint16_t tail_pos = 0;

    if ( size > tail_pos ) {
      tail_pos = size % RING_BUFFER_SIZE;
      if ( tail_pos != wifi_rx_buffer.head ) {
        wifi_rx_buffer.tail = tail_pos;
      }
    }
  }
}

/**
 * @brief  UART error callback.
 * @param  huart: Pointer to the UART handle.
 * @retval None.
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
  (void)*huart;
  esp32_io_error_handler();
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Handle UART errors by deinitializing the interface.
 * @retval None.
 */
static void esp32_io_error_handler(void) {
  HAL_UART_DMAStop(ESP32_UART_HANDLE);
  HAL_UART_DeInit(ESP32_UART_HANDLE);
  while ( 1 ) {
    /* Error handling loop */
  }
}

