/*
 * flash.h
 *
 *  Created on: Feb 18, 2025
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */

#ifndef INC_OTA_FLASH_H_
#define INC_OTA_FLASH_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* Enum for flash operation status */
typedef enum
{
    FLASH_OK = 0,
    FLASH_ERROR
} flash_status_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define USER_FLASH_SIZE   (USER_FLASH_END_ADDRESS - USER_FLASH_FIRST_PAGE_ADDRESS)

/* Exported functions ------------------------------------------------------- */
flash_status_t flash_write(__IO uint32_t* Address, uint32_t* Data, uint16_t DataLength);
flash_status_t flash_erase(uint32_t StartSector);

#endif /* INC_OTA_FLASH_H_ */
