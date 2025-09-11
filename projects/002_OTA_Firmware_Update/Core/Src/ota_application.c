/*
  * Copyright Amazon.com, Inc. and its affiliates. All Rights Reserved.
  * SPDX-License-Identifier: MIT
  *
  * Licensed under the MIT License. See the LICENSE accompanying this file
  * for the specific language governing permissions and limitations under
  * the License.
  */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ota_application.h"
#include "custom_job_doc.h"
/* New for simplified demo */
#include "custom_job_parser.h"

#include "ota_flash.h"
#include "application_config.h"
#include "MQTTFileDownloader_config.h"
#include "mqtt_helper.h"
#include "MQTTFileDownloader.h"
#include "jobs.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "ota_job_processor.h"

#define MAX_THING_NAME_SIZE      128U
#define MAX_JOB_ID_LENGTH        64U
#define START_JOB_MSG_LENGTH     147U
#define UPDATE_JOB_MSG_LENGTH    48U

MqttFileDownloaderContext_t mqtt_file_downloader_context= { 0 };
static uint32_t num_of_blocks_remaining = 0;
static uint32_t current_block_offset = 0;
static uint8_t current_file_id = 0;
static uint32_t total_bytes_received = 0;
char global_job_id[ MAX_JOB_ID_LENGTH ] = { 0 };

static void handle_mqtt_streams_block_arrived( uint8_t * data,
                                          size_t data_length );
static void finish_download();
static bool job_metadata_handler_chain( char * topic,
                                    size_t topic_length );
static void request_data_block( void );
static bool job_handler_chain( char * message,
                            size_t message_length );

extern QueueHandle_t mqtt_tx_queue;

/*=====================================================================================================================*/
/**
 * @brief Starts the OTA process by publishing a
 *        StartNextPendingJobExecution request to AWS IoT Jobs.
 *
 * This function builds the "start-next" topic and message using the AWS IoT
 * Jobs library and publishes it to the MQTT TX queue. The cloud responds
 * with details of the next pending OTA job, if available.
 *
 * @return OTA_Status_t
 *         - OTA_SUCCESS on success
 *         - OTA_ERR_TOPIC_BUILD if topic generation fails
 *         - OTA_ERR_MSG_TRUNC if job request message overflow
 *         - OTA_ERR_QUEUE_FULL if publish enqueue fails
 */
OTA_Status_t  ota_start( void ) {

  char topic_buf[ TOPIC_BUFFER_SIZE + 1 ] = { 0 };
  char msg_buf  [ START_JOB_MSG_LENGTH   ] = { 0 };
  size_t topic_len, msg_len;
  mqtt_queue_item_t pub_item = { 0 };

  /*
   * Step 1:
   * AWS IoT Jobs library:
   * Creates the topic string for a StartNextPendingJobExecution request.
   * It used to check if any pending jobs are available.
   */
  if (Jobs_StartNext( topic_buf, sizeof(topic_buf),
                      CLIENT_ID, strlen(CLIENT_ID),
                      &topic_len) != JobsSuccess) {

     LogError(("Jobs_StartNext failed"));
     return OTA_ERR_TOPIC_BUILD;
  }

  /*
   * Step 2:
   * AWS IoT Jobs library:
   * Creates the message string for a StartNextPendingJobExecution request.
   * It will be sent on the topic created in the previous step.
   */
  msg_len = Jobs_StartNextMsg( OTA_CLIENT_TOKEN,
                               sizeof(OTA_CLIENT_TOKEN) - 1U,
                               msg_buf, sizeof(msg_buf));

  if (msg_len > sizeof(msg_buf)) {

    LogError(("StartNextMsg truncated"));
    return OTA_ERR_MSG_TRUNC;
  }

  // Prepare queue item
  pub_item.operation      = MQTT_OPERATION_PUBLISH;
  pub_item.topic_length   = topic_len;
  pub_item.payload_length = msg_len;
  memcpy(pub_item.topic, topic_buf, topic_len + 1U);
  memcpy(pub_item.payload, msg_buf, msg_len);

  // Send to TX queue
  if (xQueueSend(mqtt_tx_queue, &pub_item, 0) != pdPASS) {
    LogError(("PUBLISH enqueue failed"));
    return OTA_ERR_QUEUE_FULL;
  }

  return OTA_SUCCESS;
}



/**
 * @brief Handles incoming MQTT messages related to OTA jobs and data blocks.
 *
 * @param[in] topic          The topic string of the MQTT message.
 * @param[in] topic_length   Length of the topic string.
 * @param[in] message        Payload of the MQTT message.
 * @param[in] message_length Length of the message.
 *
 * @return true if the message was handled, false otherwise.
 */
/* Implemented for use by the MQTT library */
bool ota_handle_incoming_mqtt_message(  char * topic,
                                       size_t topic_length,
                                       char * message,
                                       size_t message_length ) {

  bool handled = false;
  int32_t file_id = 0;
  int32_t block_id = 0;
  int32_t block_size = 0;

  /* Check for Job Metadata */
  handled = job_metadata_handler_chain( topic, topic_length );

  if( !handled ) {

    /*
     * AWS IoT Jobs library:
     * Checks if a message comes from the start-next/accepted reserved topic.
     */
    handled = Jobs_IsStartNextAccepted( topic, topic_length, CLIENT_ID, strlen(CLIENT_ID) );

    if( handled ) {

      handled = job_handler_chain( ( char * ) message, message_length );
    } else {

      /*
       * MQTT streams Library:
       * Checks if the incoming message contains the requested data block. It is performed by
       * comparing the incoming MQTT message topic with MQTT streams topics.
       */
      handled = mqttDownloader_isDataBlockReceived( &mqtt_file_downloader_context,
                                                   topic,
                                                   topic_length );

      if( handled ) {

        uint8_t decoded_data[ mqttFileDownloader_CONFIG_BLOCK_SIZE ];
        size_t decoded_data_length = 0;

        /*
         * MQTT streams Library:
         * Extracting and decoding the received data block from the incoming MQTT message.
         */
        handled = mqttDownloader_processReceivedDataBlock( &mqtt_file_downloader_context,
                                                           message,
                                                           message_length,
                                                           &file_id,
                                                           &block_id,
                                                           &block_size,
                                                           decoded_data,
                                                           &decoded_data_length );

        handle_mqtt_streams_block_arrived( decoded_data, decoded_data_length );
      }
    }
  }

  if( !handled ) {
    LogError( ( "Unrecognized incoming MQTT message received on topic: "
                "%.*s\nMessage: %.*s\n",
                ( unsigned int ) topic_length,
                topic,
                ( unsigned int ) message_length,
                ( char * ) message ) );
  }

  return handled;
}

/**
 * @brief Processes OTA job metadata messages and clears global job ID
 *        if the job update status is accepted or rejected.
 *
 * @param[in] topic         The MQTT topic.
 * @param[in] topic_length  Length of topic string.
 *
 * @return true if the topic was handled, false otherwise.
 */
static bool job_metadata_handler_chain( char * topic,
                                    size_t topic_length ) {

  bool handled = false;

  if( global_job_id[ 0 ] != 0 ) {

    /* Check if the incoming message corresponds to an "update/accepted" topic for the current job. */
    handled = Jobs_IsJobUpdateStatus( topic,
                                      topic_length,
                                      ( const char * ) &global_job_id,
                                      strnlen( global_job_id,
                                              MAX_JOB_ID_LENGTH ),
                                      CLIENT_ID,
                                      strlen(CLIENT_ID),
                                      JobUpdateStatus_Accepted );

    if( handled ) {

      LogInfo( ( "Job was accepted! Clearing Job ID." ) );
      global_job_id[ 0 ] = 0;
    } else {

      /* Check if the incoming message corresponds to an "update/rejected" topic for the current job. */
      handled = Jobs_IsJobUpdateStatus( topic,
                                        topic_length,
                                        ( const char * ) &global_job_id,
                                        strnlen( global_job_id,
                                                 MAX_JOB_ID_LENGTH ),
                                                 CLIENT_ID,
                                                 strlen(CLIENT_ID),
                                        JobUpdateStatus_Rejected );
    }

    if( handled ) {

      LogInfo( ( "Job was rejected! Clearing Job ID." ) );
      global_job_id[ 0 ] = 0;
    }
  }

  return handled;
}

/**
 * @brief Processes job file details extracted from the job document.
 *
 * @param[in] params  Parsed job file parameters.
 */
/* Custom OTA library callback */
static void process_job_file( custom_job_doc_fields_t * params )
{
  num_of_blocks_remaining = params->file_size /
                            mqttFileDownloader_CONFIG_BLOCK_SIZE;
  num_of_blocks_remaining += ( params->file_size %
                               mqttFileDownloader_CONFIG_BLOCK_SIZE >
                               0 )
                               ? 1
                               : 0;

  current_file_id = params->file_id;
  current_block_offset = 0;
  total_bytes_received = 0;

  /*
   * MQTT streams Library:
   * Initializing the MQTT streams downloader. Passing the
   * parameters extracted from the AWS IoT OTA jobs document
   * using OTA jobs parser.
   */
  mqttDownloader_init( &mqtt_file_downloader_context,
                       params->image_ref,
                       params->image_ref_len,
                       CLIENT_ID,
                       strlen(CLIENT_ID),
                       DATA_TYPE_JSON );

  // Prepare queue item
  mqtt_queue_item_t queue_item = { 0 };
  queue_item.operation = MQTT_OPERATION_SUBSCRIBE;
  queue_item.topic_length = mqtt_file_downloader_context.topicStreamDataLength;
  memcpy(queue_item.topic, mqtt_file_downloader_context.topicStreamData,
         mqtt_file_downloader_context.topicStreamDataLength );

  // Send to TX queue
  if (xQueueSend(mqtt_tx_queue, &queue_item, portMAX_DELAY) != pdPASS) {
    LogError( ("Failed to queue MQTT subscribe: Topic='%s'", mqtt_file_downloader_context.topicStreamData) );
  }

  LogInfo( ( "Starting The Download." ) );


  flash_erase(USER_FLASH_SECOND_SECTOR_ADDRESS);

  /* Request the first block */
  request_data_block();
}

/**
 * @brief Handles parsing and processing of the OTA job document.
 *
 * @param[in] message        Raw job execution message.
 * @param[in] message_length Length of job message.
 *
 * @return true if all job files were processed successfully,
 *         false otherwise.
 */
static bool job_handler_chain( char * message,
                               size_t message_length ) {

  char * job_doc;
  size_t job_doc_length = 0U;
  char * jobId;
  size_t job_id_length = 0U;
  int8_t file_index = 0;

  /*
  * AWS IoT Jobs library:
  * Extracting the custom job document from the jobs message recevied from AWS IoT core.
  */
  job_doc_length = Jobs_GetJobDocument( message, message_length, ( const char ** ) &job_doc );

  /*
  * AWS IoT Jobs library:
  * Extracting the job ID from the jobs message recevied from AWS IoT core.
  */
  job_id_length = Jobs_GetJobId( message, message_length, ( const char ** ) &jobId );

  if( global_job_id[ 0 ] == 0 ) {

     strncpy( global_job_id, jobId, job_id_length );
  }

  if( ( job_doc_length != 0U ) && ( job_id_length != 0U ) ) {

    custom_job_doc_fields_t job_fields = { 0 };

      do {
        /*
         * See custom_job_parser.c for this function
         */
        file_index = custom_parser_parse_job_doc_file( job_doc,
                                                       job_doc_length,
                                                       &job_fields );

         if( file_index >= 0 ) {

           LogInfo( ( "Received Custom Job Document" ) );

           /* Process the job file here */
           process_job_file(&job_fields);
           LogInfo( ( "Successfully processed job document.\n" ) );
         }
     } while( file_index > 0 );

  } else {

    LogInfo( ( "No job available..." ) );
    LogInfo( ( "Running  current application" ) );
    run_application();
  }

  /* File index will be -1 if an error occured, and 0 if all files were */
  /* processed */
  return file_index == 0;
}

/**
 * @brief Sends a request to AWS IoT Core for the next OTA data block.
 */
static void request_data_block( void ) {

  char get_stream_request[ GET_STREAM_REQUEST_BUFFER_SIZE ];
  size_t get_stream_request_length = 0U;
  static mqtt_queue_item_t queue_item = { 0 };

  /*
   * MQTT streams Library:
   * Creating the Get data block request. MQTT streams library only
   * creates the get block request.
   */
  get_stream_request_length = mqttDownloader_createGetDataBlockRequest( mqtt_file_downloader_context.dataType,
                                                                        current_file_id,
                                                                        mqttFileDownloader_CONFIG_BLOCK_SIZE,
                                                                        current_block_offset,
                                                                        1, /* Only ever request a single block for this simple example */
                                                                        get_stream_request,
                                                                        GET_STREAM_REQUEST_BUFFER_SIZE );


  // Populate queue item
  queue_item.operation = MQTT_OPERATION_PUBLISH;
  queue_item.payload_length = get_stream_request_length;
  queue_item.topic_length = mqtt_file_downloader_context.topicGetStreamLength;
  memcpy( queue_item.payload, get_stream_request, get_stream_request_length );
  memcpy( queue_item.topic, mqtt_file_downloader_context.topicGetStream,
          mqtt_file_downloader_context.topicGetStreamLength );

  // Send to TX queue
  if (xQueueSend(mqtt_tx_queue, &queue_item, portMAX_DELAY) != pdPASS) {
     LogError( ("Failed to queue MQTT publish: Topic='%.*s', Payload='%.*s'",
               (int)queue_item.topic_length, queue_item.topic,
               (int)queue_item.payload_length, (char *)queue_item.payload) );
  }
}


/**
 * @brief Handles incoming OTA data block from MQTT Streams and writes it to flash.
 *
 * @param[in] data        Pointer to received data block.
 * @param[in] data_length Length of the data block in bytes.
 */
uint32_t flash_write_address = USER_FLASH_SECOND_SECTOR_ADDRESS;
/* Implemented for the MQTT Streams library */
static void handle_mqtt_streams_block_arrived( uint8_t * data,
                                               size_t data_length ) {

  uint32_t flash_length = data_length / 4;

  flash_status_t f_status = FLASH_OK;
  f_status = flash_write(&flash_write_address, (uint32_t*)data, flash_length);

  if (f_status != FLASH_OK) {
    LogError(("Flash write error"));
  }

  total_bytes_received += data_length;
  num_of_blocks_remaining--;

  LogInfo( ( "Downloaded block %lu of %lu.",
             current_block_offset,
             ( current_block_offset + num_of_blocks_remaining ) ) );

  if (num_of_blocks_remaining == 0) {
    finish_download();
  } else {
    current_block_offset++;
    request_data_block();
  }
}

/**
 * @brief Transfers control from the bootloader to the user application
 *        located at USER_FLASH_FIRST_SECTOR_ADDRESS.
 */
static void jump_to_application(void) {

  LogInfo( ("Gonna Jump to Application\r\n"));

  // Get the application reset handler (start of application)
  void (*app_reset_handler)(void) = (void*)(*((volatile uint32_t*) (USER_FLASH_FIRST_SECTOR_ADDRESS + 4U)));

  // Check if the reset handler is valid (not erased)
  if (app_reset_handler != (void*)0xFFFFFFFF) {

    /* Reset the Clock system */
    HAL_RCC_DeInit();
    HAL_DeInit();

    /* Set the MSP (Main Stack Pointer) to the application's stack pointer */
    __set_MSP(*(volatile uint32_t*) USER_FLASH_FIRST_SECTOR_ADDRESS);

    /* Set the vector table to the application address */
    SCB->VTOR = USER_FLASH_FIRST_SECTOR_ADDRESS;

    /* Reset the sysTick timer */
    SysTick->CTRL = 0;

    /* Switch to use the Main Stack Pointer (MSP) */
    __set_CONTROL(0);

    /* call application reset handler */
    app_reset_handler();
  } else {
    LogError( ("Application is not present or corrupted\r\n") );
  }
}

/**
 * @brief Runs the user application by jumping to its reset vector.
 *        Performs a system reset if the application fails to start.
 */
void run_application() {
  jump_to_application();
  //printf("Resetting system\r\n");
  //HAL_Delay(1000);
  HAL_NVIC_SystemReset();
}

/**
 * @brief Copies firmware from one flash sector to another.
 *
 * @param[in] src_address   Source flash address.
 * @param[in] dest_address  Destination flash address.
 * @param[in] length        Number of bytes to copy.
 *
 * @return true on success, false on failure.
 */
static bool copy_firmware_to_ota_application_sector1( uint32_t src_address,
                                            uint32_t dest_address,
                                            uint32_t length) {

  flash_status_t f_status = FLASH_OK;
  uint32_t *src = (uint32_t *)src_address;
  uint32_t write_address = dest_address;
  uint32_t words = length / 4;

  // Erase the destination sector
  f_status = flash_erase(dest_address);
  if (f_status != FLASH_OK) {

    LogError(("Failed to erase second flash sector"));
    return false;
  }

  // Copy data from source to destination
  f_status = flash_write(&write_address, src, words);
  if (f_status != FLASH_OK) {

     LogError(("Failed to write to second flash sector"));
     return false;
  }

  LogInfo(("Successfully copied firmware to second sector at 0x%08lX", dest_address));
  return true;
}

/**
 * @brief Completes the OTA download process, validates and copies firmware
 *        to application sector, updates job status, and runs the new application.
 */
static void finish_download() {

  /* TODO: Do something with the completed download */
  /* Start the bootloader */
  char topic_buffer[ TOPIC_BUFFER_SIZE + 1 ] = { 0 };
  size_t topic_buffer_length = 0U;
  char message_buffer[ UPDATE_JOB_MSG_LENGTH ] = { 0 };
  mqtt_queue_item_t queue_item = { 0 };


  if (copy_firmware_to_ota_application_sector1(USER_FLASH_SECOND_SECTOR_ADDRESS, USER_FLASH_FIRST_SECTOR_ADDRESS , total_bytes_received)){
    LogInfo(("Flash complete.... Jumping to User App"));

    /*
     * AWS IoT Jobs library:
     * Creating the MQTT topic to update the status of OTA job.
     */
    Jobs_Update( topic_buffer,
                 TOPIC_BUFFER_SIZE,
                 CLIENT_ID,
                 strlen(CLIENT_ID),
                 global_job_id,
                 strlen( global_job_id ),
                 &topic_buffer_length );

    /*
     * AWS IoT Jobs library:
     * Creating the message which contains the status of OTA job.
     * It will be published on the topic created in the previous step.
     */
    size_t message_buffer_length = Jobs_UpdateMsg( Succeeded,
                                                "2",
                                                1U,
                                                message_buffer,
                                                UPDATE_JOB_MSG_LENGTH );


    // Populate queue item
    queue_item.operation = MQTT_OPERATION_PUBLISH;
    queue_item.payload_length = message_buffer_length;
    queue_item.topic_length = topic_buffer_length;
    memcpy(queue_item.payload, message_buffer, message_buffer_length );
    memcpy(queue_item.topic, topic_buffer, topic_buffer_length );

    // Send to TX queue
//    if (xQueueSend(mqtt_tx_queue, &queue_item, portMAX_DELAY) != pdPASS) {
//      LogError(("Failed to queue MQTT publish: Topic='%s'", topic_buffer));
//    }

    vTaskDelay(pdMS_TO_TICKS(1000));

    LogInfo( ("\033[1;32mOTA Completed successfully!\033[0m\n" ) );

    run_application();


  } else {

    LogError(("Failed to copy firmware to second sector! Aborting OTA update."));

    /*
     * AWS IoT Jobs library:
     * Creating the MQTT topic to update the status of OTA job.
     */
    Jobs_Update(topic_buffer, TOPIC_BUFFER_SIZE, CLIENT_ID, strlen(CLIENT_ID),
                global_job_id, strlen(global_job_id), &topic_buffer_length);

    /*
     * AWS IoT Jobs library:
     * Creating the message which contains the status of OTA job.
     * It will be published on the topic created in the previous step.
     */
    size_t message_buffer_length = Jobs_UpdateMsg(Failed, "CRC Error", 8U,
                                                 message_buffer, UPDATE_JOB_MSG_LENGTH);
    mqtt_publish(topic_buffer, topic_buffer_length, (uint8_t *)message_buffer, message_buffer_length);

    // Populate queue item
    queue_item.operation = MQTT_OPERATION_PUBLISH;
    queue_item.payload_length = message_buffer_length;
    queue_item.topic_length = topic_buffer_length;
    memcpy(queue_item.payload, message_buffer, message_buffer_length );
    memcpy(queue_item.topic, topic_buffer, topic_buffer_length );

    // Send to TX queue
    if (xQueueSend(mqtt_tx_queue, &queue_item, portMAX_DELAY) == pdPASS) {
      LogInfo(("Queued MQTT publish: Topic='%s'", topic_buffer));
    }
  }
}
