/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "esp32_at.h"
#include "sensor_util.h"
#include "mqtt_helper.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "application_config.h"
#include "ota_application.h"
#include "semphr.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_uart4_rx;

/* USER CODE BEGIN PV */

QueueHandle_t mqtt_tx_queue;
QueueHandle_t mqtt_rx_queue;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_UART4_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int _write(int file, char *ptr, int len) {
  HAL_UART_Transmit(&huart2, (uint8_t*) ptr, len, HAL_MAX_DELAY);
  return len;
}

/**
 * @brief OTA Task
 */
static void ota_task(void *pvParameters) {
  (void) pvParameters;

  OTA_Status_t rc = ota_start();

  if (rc != OTA_SUCCESS) {
    LogError(("OTA start failed: %d", rc));
    /* TODO: notify a supervisor task or set an event here if needed */
  } else {
    LogInfo(("OTA start succeeded"));
  }

  vTaskDelete( NULL);
}

// this task receives unsolicited MQTT messages from the AWS server
void mqtt_receive_task(void *parameters) {
  mqtt_queue_item_t item = { 0 };

  while (1) {
    BaseType_t rc = xQueueReceive(mqtt_rx_queue, &item, portMAX_DELAY);

    configASSERT(rc == pdPASS);

    // Check if both topic and payload contain valid data
    if (item.operation == MQTT_OPERATION_RECEIVE && item.topic_length > 0
        && item.payload_length > 0) {
      // Extract topic and payload
      const char *topic = item.topic;
      size_t topic_len = item.topic_length;
      const uint8_t *payload = item.payload;
      size_t payload_len = item.payload_length;

      // Pass to OTA handler
      bool message_handled = ota_handle_incoming_mqtt_message((char*) topic,
          topic_len, (char*) payload, payload_len);

      if (!message_handled) {
        LogError(
            ("mqtt_receive_task: unhandled message on %.*s", (int)topic_len, topic));
      }
    } else {
      LogWarn(
          ("mqtt_receive_task: op=%d ignored or empty message", item.operation));
    }
  }
}

void at_cmd_handle_task(void *parameters) {
  (void) parameters;

  mqtt_queue_item_t tx_item, rx_item = { 0 };
  mqtt_status_t mqtt_result = MQTT_ERROR;
  mqtt_receive_t incoming_data = { 0 };

  /* Static buffers reused for every incoming MQTT frame */
  static char topic_buf[MAX_MQTT_TOPIC_SIZE] = { 0 };
  static char payload_buf[MAX_MQTT_PAYLOAD_SIZE] = { 0 };

  incoming_data.p_payload = payload_buf;
  incoming_data.p_topic = topic_buf;
  incoming_data.topic_length = 0;
  incoming_data.payload_length = 0;

  for (;;) {
    /* 1. De-queue MQTT TX queue */
    if (xQueueReceive(mqtt_tx_queue, &tx_item, 0) == pdPASS) {
      if (tx_item.operation == MQTT_OPERATION_PUBLISH) {
        /* 2. Issue MQTT PUBLISH via AT command */
        mqtt_result = mqtt_publish(tx_item.topic, tx_item.topic_length,
            tx_item.payload, tx_item.payload_length);

        if (mqtt_result == MQTT_SUCCESS) {
          LogInfo(
              ("MQTT Publish successful: Topic='%.*s', \r\nPayload='%.*s'", (int)tx_item.topic_length, tx_item.topic, (int)tx_item.payload_length, (char *)tx_item.payload));
        } else {
          LogError(
              ("MQTT Publish failed: Topic='%.*s'", (int)tx_item.topic_length, tx_item.topic));
        }
      } else if (tx_item.operation == MQTT_OPERATION_SUBSCRIBE) {
        /* 2. Issue MQTT SUBSCRIBE via AT command */
        mqtt_result = mqtt_subscribe(tx_item.topic, tx_item.topic_length);

        if (mqtt_result == MQTT_SUCCESS) {
          LogInfo(
              ("MQTT Subscribe successful: Topic='%.*s'", (int)tx_item.topic_length, tx_item.topic));
        } else {
          LogError(
              ("MQTT Subscribe failed: Topic='%.*s'", (int)tx_item.topic_length, tx_item.topic));
        }
      }
    }

    /* 3. Check for unsolicited MQTT data from ESP32 */
    esp32_status_t esp32_result = esp32_recv_mqtt_data(&incoming_data);

    if (incoming_data.topic_length >= sizeof(rx_item.topic)
        && incoming_data.payload_length >= sizeof(rx_item.payload)) {
      LogError(("ESP32 frame too large – discarded"));
      continue;
    }

    if (esp32_result != ESP32_ERROR && incoming_data.payload_length > 0
        && incoming_data.topic_length > 0) {
      rx_item.operation = MQTT_OPERATION_RECEIVE;
      rx_item.payload_length = incoming_data.payload_length;
      rx_item.topic_length = incoming_data.topic_length;

      memcpy(rx_item.payload, incoming_data.p_payload,
          incoming_data.payload_length);
      memcpy(rx_item.topic, incoming_data.p_topic, incoming_data.topic_length);

      if (xQueueSend(mqtt_rx_queue, &rx_item, 0) == pdPASS) {
        LogInfo(
            ("Queued MQTT receive: Topic='%.*s', \r\nPayload='%.*s'", (int)rx_item.topic_length, rx_item.topic, (int)rx_item.payload_length, rx_item.payload));
      }
    }
  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_UART4_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  /* Step 1:Initialize the ESP32 Wi-Fi module */
  LogInfo(("Initializing Wi-Fi module..."));
  if (esp32_init() != ESP32_OK) {
    LogError(("Failed to initialize Wi-Fi module."));
    Error_Handler();
  }
  LogInfo(("Wi-Fi module initialized successfully.\n"));

  /* Step 2: Connect to Wi-Fi network*/
  LogInfo(("Joining Access Point: '%s' ...", WIFI_SSID));
  /* Keep attempting to connect to the specified Wi-Fi access point until
   * successful */
  while (esp32_join_ap((uint8_t*) WIFI_SSID, (uint8_t*) WIFI_PASSWORD)
      != ESP32_OK) {
    LogInfo(("Retrying to join Access Point: %s", WIFI_SSID));
  }
  LogInfo(("Successfully joined Access Point: %s", WIFI_SSID));

  /* Step 3: Configure SNTP for time synchronization */
  /* Configure Simple Network Time Protocol (SNTP) with a specified server */
  if (esp32_config_sntp(UTC_OFFSET) != ESP32_OK) {
    LogError(("Failed to configure SNTP."));
    Error_Handler();
  }
  LogInfo(("SNTP configured !"));

  /* Retrieve the current time from SNTP */
  sntp_time_t sntp_time;
  if (esp32_get_sntp_time(&sntp_time) != ESP32_OK) {
    LogError(("Failed to retrieve current time from SNTP."));
    Error_Handler();
  }
  LogInfo(
      ("SNTP time retrieved: %s, %02d %s %04d %02d:%02d:%02d", sntp_time.day, sntp_time.date, sntp_time.month, sntp_time.year, sntp_time.hour, sntp_time.min, sntp_time.sec));

  /*  Step 4: Configure the MQTT client and establish the connection */
  LogInfo(("Connecting to MQTT broker....."));
  if (mqtt_connect(CLIENT_ID, MQTT_BROKER, MQTT_PORT) != MQTT_SUCCESS) {
    LogError(("MQTT connection failed."));
    Error_Handler();
  }
  LogInfo(("Successfully connected to MQTT broker"));

  /* Create queues */
  mqtt_tx_queue = xQueueCreate(5, sizeof(mqtt_queue_item_t));
  mqtt_rx_queue = xQueueCreate(5, sizeof(mqtt_queue_item_t));
  if (mqtt_tx_queue == NULL || mqtt_rx_queue == NULL) {
    LogError(("Queue creation failed."));
    Error_Handler();
  } else {
    LogInfo(("MQTT TX and RX queues created successfully."));
  }

  //create 3 freertos tasks

  BaseType_t status;

  status = xTaskCreate(ota_task, "OTA start", 128, NULL, 2, NULL);
  configASSERT(status == pdPASS);

  status = xTaskCreate(mqtt_receive_task, "Receive sensor data", 2048,
  NULL, 2, NULL);
  configASSERT(status == pdPASS);

  status = xTaskCreate(at_cmd_handle_task, "MQTT send and receive", 2048, NULL,
      1, NULL);
  configASSERT(status == pdPASS);

  vTaskStartScheduler();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, PURPLE_LED_Pin|EMERALD_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PURPLE_LED_Pin EMERALD_LED_Pin */
  GPIO_InitStruct.Pin = PURPLE_LED_Pin|EMERALD_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
