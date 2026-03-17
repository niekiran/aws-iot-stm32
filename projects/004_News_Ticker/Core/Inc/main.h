/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "application_config.h"

typedef enum {
  MQTT_OPERATION_PUBLISH,
  MQTT_OPERATION_RECEIVE,
  MQTT_OPERATION_SUBSCRIBE
} mqtt_operation_t;

typedef struct {
  mqtt_operation_t operation;
  char topic[MAX_MQTT_TOPIC_SIZE];
  size_t topic_length;
  uint8_t payload[MAX_MQTT_PAYLOAD_SIZE];
  size_t payload_length;
} mqtt_queue_item_t;

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#undef USE_OTA
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define PURPLE_LED_Pin GPIO_PIN_0
#define PURPLE_LED_GPIO_Port GPIOC
#define LEFT_BTN_Pin GPIO_PIN_1
#define LEFT_BTN_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define OLED_RST_Pin GPIO_PIN_8
#define OLED_RST_GPIO_Port GPIOA
#define OLED_DC_Pin GPIO_PIN_10
#define OLED_DC_GPIO_Port GPIOA
#define OLED_CS_Pin GPIO_PIN_3
#define OLED_CS_GPIO_Port GPIOB
#define RIGHT_BTN_Pin GPIO_PIN_4
#define RIGHT_BTN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
