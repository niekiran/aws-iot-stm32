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

#include "mqtt_helper.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "sh1106.h"
#include "payment_app.h"
#include "oled_util.h"
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

SPI_HandleTypeDef hspi1;

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
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int _write(int file, char *ptr, int len) {
  HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);
  return len;
}



// this task receives unsolicited mqtt messages from the aws server
void mqtt_receive_task(void *parameters) {

  mqtt_queue_item_t item = { 0 };

  while(1) {

    BaseType_t rc = xQueueReceive(mqtt_rx_queue, &item, portMAX_DELAY);

    configASSERT(rc == pdPASS);

    // Check if both topic and payload contain valid data
    if (item.operation == MQTT_OPERATION_RECEIVE &&
        item.topic_length   > 0 &&
        item.payload_length > 0) {

      // Log the received message
      LogInfo(("\r\nReceived message:"));
      LogInfo(("Topic: %.*s", (int)item.topic_length, item.topic));
      LogInfo(("Message: %.*s", (int)item.payload_length, item.payload));

      handle_payment_info(item.topic, item.topic_length, item.payload, item.payload_length);

      vTaskDelay(pdMS_TO_TICKS(5000));

      display_qr();
    }
  }
}


void at_cmd_handle_task(void *parameters) {

  (void)parameters;

  mqtt_queue_item_t item, new_message = { 0 };
  mqtt_receive_t mqtt_data = { 0 };
  mqtt_status_t status = MQTT_ERROR;

  static char topic_buffer[MAX_MQTT_TOPIC_SIZE];
  static char payload_buffer[MAX_MQTT_PAYLOAD_SIZE];


  mqtt_data.p_payload = payload_buffer;
  mqtt_data.p_topic = topic_buffer;
  mqtt_data.topic_length = sizeof(topic_buffer);
  mqtt_data.payload_length = sizeof(payload_buffer);

  while(1) {

    //1. De-queue MQTT TX queue
    if (xQueueReceive(mqtt_tx_queue, &item, 0) == pdPASS) {
      if (item.operation == MQTT_OPERATION_PUBLISH) {

        //2. Send MQTT publish AT command using mqtt_publish() (check mqtt_helper.c)
        status = mqtt_publish(item.topic, item.topic_length,
                              item.payload, item.payload_length);

        if (status == MQTT_SUCCESS) {
          LogInfo(("MQTT Publish successful: Topic='%.*s', Payload='%.*s'",
                   (int)item.topic_length, item.topic,
                   (int)item.payload_length, (char *)item.payload));
        } else  {
          LogError(("MQTT Publish failed: Topic='%.*s'",
                    (int)item.topic_length, item.topic));
        }

      } else if (item.operation == MQTT_OPERATION_SUBSCRIBE) {

        //2. Send MQTT subscribe AT command using mqtt_subscribe() (check mqtt_helper.c)
        status = mqtt_subscribe(item.topic, item.topic_length);
        if (status == MQTT_SUCCESS) {
            LogInfo(("MQTT Subscribe successful: Topic='%.*s'",
                     (int)item.topic_length, item.topic));
        } else {
            LogError(("MQTT Subscribe failed: Topic='%.*s'",
                      (int)item.topic_length, item.topic));
        }
      }
    }


    //3. Check for incoming MQTT data (check esp32_recv_mqtt_data() in esp32_at.c)
    esp32_status_t rx_status = esp32_recv_mqtt_data(&mqtt_data);

    if (rx_status != ESP32_ERROR && mqtt_data.payload_length > 0) {
      new_message.operation = MQTT_OPERATION_RECEIVE;
      new_message.payload_length = mqtt_data.payload_length;
      new_message.topic_length = mqtt_data.topic_length;
      memcpy(new_message.payload, mqtt_data.p_payload, mqtt_data.payload_length);
      memcpy(new_message.topic, mqtt_data.p_topic, mqtt_data.topic_length);

      if (xQueueSend(mqtt_rx_queue, &new_message, 0) == pdPASS) {
          LogInfo(("Queued MQTT receive: Topic='%.*s'",
                   (int)new_message.topic_length, new_message.topic));
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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  LogInfo(("Initializing OLED display..."));
   if (sh1106_init(&hspi1) != SH1106_OK) {
     LogError(("Failed to initialize OLED display."));
     Error_Handler();
   }
   LogInfo(("OLED display initialized successfully.\n"));

   display_qr();

  /* Step 1:Initialize the ESP32 Wi-Fi module */
  LogInfo(("Initializing Wi-Fi module..."));
  oled_show_status("Wi-Fi Init...");

  if (esp32_init() != ESP32_OK) {
    LogError(("Failed to initialize Wi-Fi module."));
    oled_show_status("Wi-Fi Init Failed!");

    Error_Handler();
  }
  LogInfo(("Wi-Fi module initialized successfully.\n"));
  oled_show_status("Wi-Fi Ready");

#ifndef USE_OTA
  /* Step 2: Connect to Wi-Fi network*/
  LogInfo(("Joining Access Point: '%s' ...", WIFI_SSID));
  /* Keep attempting to connect to the specified Wi-Fi access point until
   * successful */
  while (esp32_join_ap((uint8_t *)WIFI_SSID, (uint8_t *)WIFI_PASSWORD) !=
         ESP32_OK) {
    LogInfo(("Retrying to join Access Point: %s", WIFI_SSID));
    oled_show_status("Wi-Fi Retry...");

  }
  LogInfo(("Successfully joined Access Point: %s", WIFI_SSID));
  oled_show_message("Wi-Fi Connected", WIFI_SSID);

  /* Step 3: Configure SNTP for time synchronization */
  /* Configure Simple Network Time Protocol (SNTP) with a specified server */
  LogInfo(("Configuring SNTP..."));
  oled_show_status("Config SNTP...");
  if (esp32_config_sntp(UTC_OFFSET) != ESP32_OK) {
    LogError(("Failed to configure SNTP."));
    oled_show_status("SNTP Failed!");
    Error_Handler();
  }
  LogInfo(("SNTP configured !"));
  oled_show_status("SNTP Configured");

  /* Retrieve the current time from SNTP */
  sntp_time_t sntp_time;
  oled_show_status("Syncing Time...");
  if (esp32_get_sntp_time(&sntp_time) != ESP32_OK) {
    LogError(("Failed to retrieve current time from SNTP."));
    oled_show_status("Time Sync Failed!");
    Error_Handler();
  }
  LogInfo(("SNTP time retrieved: %s, %02d %s %04d %02d:%02d:%02d",
           sntp_time.day, sntp_time.date, sntp_time.month, sntp_time.year,
           sntp_time.hour, sntp_time.min, sntp_time.sec));
  oled_show_status("Time Synced");


  /*  Step 4: Configure the MQTT client and establish the connection */
  LogInfo(("Connecting to MQTT broker at %s:%d...", MQTT_BROKER, MQTT_PORT));
  oled_show_status("MQTT Connecting...");
  if (mqtt_connect(CLIENT_ID, MQTT_BROKER, MQTT_PORT) != MQTT_SUCCESS) {
    LogError(("MQTT connection failed."));
    oled_show_status("MQTT Connect Fail");

    Error_Handler();
  }
  LogInfo(("Successfully connected to MQTT broker: %s", MQTT_BROKER));
  oled_show_status("MQTT Connected");

#else
  LogInfo(("Already connected to MQTT broker %s", MQTT_BROKER));
#endif

  LogInfo(("Subscribing to topic: %s", RAZORPAY_TOPIC));
  oled_show_message("Subscribing...", RAZORPAY_TOPIC);
  if (mqtt_subscribe(RAZORPAY_TOPIC, strlen(RAZORPAY_TOPIC)) != MQTT_SUCCESS) {
    LogError(("Subscription to topic '%s' failed.", RAZORPAY_TOPIC));
    oled_show_status("Sub Failed!");
    Error_Handler();
  }
  LogInfo(("Successfully Subscribed to topic: %s", RAZORPAY_TOPIC));
  oled_show_status("Subscribed!");

  /* Create queues */
  mqtt_rx_queue = xQueueCreate(5, sizeof(mqtt_queue_item_t));
  mqtt_tx_queue = xQueueCreate(5, sizeof(mqtt_queue_item_t));
  if (mqtt_rx_queue == NULL || mqtt_tx_queue == NULL) {
    LogError(("Queue creation failed."));
    Error_Handler();
  } else {
    LogInfo(("MQTT TX and RX queues created successfully."));
  }

  display_qr();

  //create 3 freertos tasks

  BaseType_t status;

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
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  HAL_GPIO_WritePin(GPIOA, OLED_RST_Pin|OLED_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);

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

  /*Configure GPIO pins : OLED_RST_Pin OLED_DC_Pin */
  GPIO_InitStruct.Pin = OLED_RST_Pin|OLED_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : OLED_CS_Pin */
  GPIO_InitStruct.Pin = OLED_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OLED_CS_GPIO_Port, &GPIO_InitStruct);

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
