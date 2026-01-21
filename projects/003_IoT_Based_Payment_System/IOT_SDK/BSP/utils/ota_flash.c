/*
 * flash.c
 *
 *  Created on: Feb 18, 2025
 *      Author: Shreyas Acharya, BHARATI SOFTWARE
 */

/* Includes ------------------------------------------------------------------*/
#include "ota_flash.h"
#include "application_config.h"

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Erases all user flash area
  * @param  start_sector: start of user flash area
  * @retval FLASH_OK: User flash area successfully erased
  *         FLASH_ERROR: Error occurred
  */
flash_status_t flash_erase(uint32_t start_sector)
{
    flash_status_t status = FLASH_OK;
    uint32_t flash_address = start_sector;

    HAL_FLASH_Unlock();

    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
       be done by word */
    if (flash_address <= (uint32_t) USER_FLASH_LAST_SECTOR_ADDRESS)
    {
        FLASH_EraseInitTypeDef flash_erase_init;
        uint32_t sector_nb = 0;

        flash_erase_init.TypeErase = FLASH_TYPEERASE_SECTORS;
        flash_erase_init.Sector = FLASH_SECTOR_5;
        //flash_erase_init.NbSectors = FLASH_SECTOR_7 - FLASH_SECTOR_3 + 1;
        flash_erase_init.NbSectors = 1;
        flash_erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;

        if (HAL_FLASHEx_Erase(&flash_erase_init, &sector_nb) != HAL_OK)
        {
            status = FLASH_ERROR;
        }
    }
    else
    {
        status = FLASH_ERROR;
    }
    HAL_FLASH_Lock();

    return status;
}

/**
  * @brief  Writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  flash_address: start address for writing data buffer
  * @param  data: pointer to data buffer
  * @param  data_length: length of data buffer (unit is 32-bit word)
  * @retval FLASH_OK: Data successfully written to Flash memory
  *         FLASH_ERROR: Error occurred while writing data in Flash memory
  */
flash_status_t flash_write(__IO uint32_t* flash_address, uint32_t* data, uint16_t data_length)
{
    flash_status_t status = FLASH_OK;
    uint32_t i = 0;

    HAL_FLASH_Unlock();

    for (i = 0; (i < data_length) && (*flash_address <= (USER_FLASH_END_ADDRESS - 4)); i++)
    {
        /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
           be done by word */
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, *flash_address, *(uint32_t*)(data + i)) == HAL_OK)
        {
            /* Increment FLASH destination address */
            *flash_address += 4;
        }
        else
        {
            /* Error occurred while writing data in Flash memory */
            status = FLASH_ERROR;
            break;
        }
    }

    HAL_FLASH_Lock();
    return status;
}
