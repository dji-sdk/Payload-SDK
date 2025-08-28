/**
  ******************************************************************************
  * @file    IAP/IAP_Main/Src/flash_if.c
  * @author  MCD Application Team
  * @brief   This file provides all the memory related operation functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  * @statement DJI has modified some symbols' name.
  *
  ******************************************************************************
  */

/** @addtogroup Gd32F4xx_IAP_Main
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "flash_if.h"
#include "gd32f5xx.h"
#include "gd32f5xx_fmc.h"

/* Private typedef -----------------------------------------------------------*/
#define ENABLE_BOR  0

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static int32_t Flash_If_GetSectorNum(uint32_t address);

/* Private functions ---------------------------------------------------------*/
static int32_t Flash_If_GetSectorNum(uint32_t address)
{
  int32_t sectorNum = 0;

  if (address >= ADDR_FLASH_SECTOR_0 && address < ADDR_FLASH_SECTOR_1) {
    sectorNum = 0;
  } else if (address >= ADDR_FLASH_SECTOR_1 && address < ADDR_FLASH_SECTOR_2) {
    sectorNum = 1;
  } else if (address >= ADDR_FLASH_SECTOR_2 && address < ADDR_FLASH_SECTOR_3) {
    sectorNum = 2;
  } else if (address >= ADDR_FLASH_SECTOR_3 && address < ADDR_FLASH_SECTOR_4) {
    sectorNum = 3;
  } else if (address >= ADDR_FLASH_SECTOR_4 && address < ADDR_FLASH_SECTOR_5) {
    sectorNum = 4;
  } else if (address >= ADDR_FLASH_SECTOR_5 && address < ADDR_FLASH_SECTOR_6) {
    sectorNum = 5;
  } else if (address >= ADDR_FLASH_SECTOR_6 && address < ADDR_FLASH_SECTOR_7) {
    sectorNum = 6;
  } else if (address >= ADDR_FLASH_SECTOR_7 && address < ADDR_FLASH_SECTOR_8) {
    sectorNum = 7;
  } else if (address >= ADDR_FLASH_SECTOR_8 && address < ADDR_FLASH_SECTOR_9) {
    sectorNum = 8;
  } else if (address >= ADDR_FLASH_SECTOR_9 && address < ADDR_FLASH_SECTOR_10) {
    sectorNum = 9;
  } else if (address >= ADDR_FLASH_SECTOR_10 && address < ADDR_FLASH_SECTOR_11) {
    sectorNum = 10;
  } else if (address >= ADDR_FLASH_SECTOR_11 && address < ADDR_FLASH_SECTOR_12) {
    sectorNum = 11;
  } else if (address >= ADDR_FLASH_SECTOR_12 && address < ADDR_FLASH_SECTOR_13) {
    sectorNum = 12;
  } else if (address >= ADDR_FLASH_SECTOR_13 && address < ADDR_FLASH_SECTOR_14) {
    sectorNum = 13;
  } else if (address >= ADDR_FLASH_SECTOR_14 && address < ADDR_FLASH_SECTOR_15) {
    sectorNum = 14;
  } else if (address >= ADDR_FLASH_SECTOR_15 && address < ADDR_FLASH_SECTOR_16) {
    sectorNum = 15;
  } else if (address >= ADDR_FLASH_SECTOR_16 && address < ADDR_FLASH_SECTOR_17) {
    sectorNum = 16;
  } else if (address >= ADDR_FLASH_SECTOR_17 && address < ADDR_FLASH_SECTOR_18) {
    sectorNum = 17;
  } else if (address >= ADDR_FLASH_SECTOR_18 && address < ADDR_FLASH_SECTOR_19) {
    sectorNum = 18;
  } else if (address >= ADDR_FLASH_SECTOR_19 && address < ADDR_FLASH_SECTOR_20) {
    sectorNum = 19;
  } else if (address >= ADDR_FLASH_SECTOR_20 && address < ADDR_FLASH_SECTOR_21) {
    sectorNum = 20;
  } else if (address >= ADDR_FLASH_SECTOR_21 && address < ADDR_FLASH_SECTOR_22) {
    sectorNum = 21;
  } else if (address >= ADDR_FLASH_SECTOR_22 && address < ADDR_FLASH_SECTOR_23) {
    sectorNum = 22;
  } else if (address >= ADDR_FLASH_SECTOR_23 && address < ADDR_FLASH_SECTOR_24) {
    sectorNum = 23;
  } else if (address >= ADDR_FLASH_SECTOR_24 && address < ADDR_FLASH_SECTOR_25) {
    sectorNum = 24;
  } else if (address >= ADDR_FLASH_SECTOR_25 && address < ADDR_FLASH_SECTOR_26) {
    sectorNum = 25;
  } else if (address >= ADDR_FLASH_SECTOR_26 && address < ADDR_FLASH_SECTOR_27) {
    sectorNum = 26;
  } else if (address >= ADDR_FLASH_SECTOR_27 && address < ADDR_FLASH_SECTOR_28) {
    sectorNum = 27;
  } else if (address >= ADDR_FLASH_SECTOR_28 && address < ADDR_FLASH_SECTOR_29) {
    sectorNum = 28;
  } else if (address >= ADDR_FLASH_SECTOR_29 && address < ADDR_FLASH_SECTOR_30) {
    sectorNum = 29;
  } else if (address >= ADDR_FLASH_SECTOR_30 && address < ADDR_FLASH_SECTOR_31) {
    sectorNum = 30;
  } else if (address >= ADDR_FLASH_SECTOR_31 && address < ADDR_FLASH_SECTOR_32) {
    sectorNum = 31;
  } else if (address >= ADDR_FLASH_SECTOR_32 && address < ADDR_FLASH_SECTOR_33) {
    sectorNum = 32;
  } else if (address >= ADDR_FLASH_SECTOR_33 && address < ADDR_FLASH_SECTOR_34) {
    sectorNum = 33;
  } else if (address >= ADDR_FLASH_SECTOR_34 && address < ADDR_FLASH_SECTOR_35) {
    sectorNum = 34;
  } else if (address >= ADDR_FLASH_SECTOR_35 && address < ADDR_FLASH_SECTOR_36) {
    sectorNum = 35;
  } else if (address >= ADDR_FLASH_SECTOR_36 && address < ADDR_FLASH_SECTOR_37) {
    sectorNum = 36;
  } else if (address >= ADDR_FLASH_SECTOR_37 && address < ADDR_FLASH_SECTOR_38) {
    sectorNum = 37;
  } else if (address >= ADDR_FLASH_SECTOR_38 && address < ADDR_FLASH_SECTOR_39) {
    sectorNum = 38;
  } else if (address >= ADDR_FLASH_SECTOR_39 && address < ADDR_FLASH_SECTOR_40) {
    sectorNum = 39;
  } else if (address >= ADDR_FLASH_SECTOR_40 && address < ADDR_FLASH_SECTOR_41) {
    sectorNum = 40;
  } else if (address >= ADDR_FLASH_SECTOR_41 && address < ADDR_FLASH_SECTOR_42) {
    sectorNum = 41;
  } else if (address >= ADDR_FLASH_SECTOR_42 && address < ADDR_FLASH_SECTOR_43) {
    sectorNum = 42;
  } else if (address >= ADDR_FLASH_SECTOR_43 && address < ADDR_FLASH_SECTOR_44) {
    sectorNum = 43;
  } else if (address >= ADDR_FLASH_SECTOR_44 && address < ADDR_FLASH_SECTOR_45) {
    sectorNum = 44;
  } else if (address >= ADDR_FLASH_SECTOR_45 && address < ADDR_FLASH_SECTOR_46) {
    sectorNum = 45;
  } else if (address >= ADDR_FLASH_SECTOR_46 && address < ADDR_FLASH_SECTOR_47) {
    sectorNum = 46;
  } else if (address >= ADDR_FLASH_SECTOR_47 && address < ADDR_FLASH_SECTOR_48) {
    sectorNum = 47;
  } else if (address >= ADDR_FLASH_SECTOR_48 && address < ADDR_FLASH_SECTOR_49) {
    sectorNum = 48;
  } else if (address >= ADDR_FLASH_SECTOR_49 && address < ADDR_FLASH_SECTOR_50) {
    sectorNum = 49;
  } else if (address >= ADDR_FLASH_SECTOR_50 && address < ADDR_FLASH_SECTOR_51) {
    sectorNum = 50;
  } else if (address >= ADDR_FLASH_SECTOR_51 && address < ADDR_FLASH_SECTOR_52) {
    sectorNum = 51;
  } else if (address >= ADDR_FLASH_SECTOR_52 && address < ADDR_FLASH_SECTOR_53) {
    sectorNum = 52;
  } else if (address >= ADDR_FLASH_SECTOR_53 && address <= FLASH_END_ADDRESS) {
    sectorNum = 53;
  } else {
    printf("Not found sector by address: 0x%08X\r\n", address);
    sectorNum = -1;
  }

  return sectorNum;
}

uint32_t FLASH_If_Init(void)
{
    int32_t result = 0;

#if ENABLE_BOR
    fmc_unlock();
    ob_unlock();

    ob_user_bor_threshold(OB_BOR_TH_VALUE3);

    borValue = ob_user_bor_threshold_get();
    if (borValue != OB_BOR_TH_VALUE3) {
      return -1;
    }

    ob_start();
    ob_lock();
    fmc_lock();
#endif

    return result;
}

/**
  * @brief  This function does an erase of all user flash area
  * @param  StartSector: start of user flash area
  * @retval 0: user flash area successfully erased
  *         1: error occurred
  */
uint32_t FLASH_If_Erase(uint32_t startAddress, uint32_t endAddress)
{
  uint8_t isNeedErase[FLASH_SECTOR_NUM] = {0};
  int32_t startSectorNum = 0;
  int32_t endSectorNum = 0;
  fmc_state_enum fmc_state;
  int32_t i = 0;

  if (startAddress < ADDR_FLASH_SECTOR_0 || startAddress > FLASH_END_ADDRESS) {
    return -1;
  }

  if (endAddress < ADDR_FLASH_SECTOR_0 || endAddress > FLASH_END_ADDRESS) {
    return -1;
  }

  if (endAddress <= startAddress) {
    return -1;
  }

  startSectorNum = Flash_If_GetSectorNum(startAddress);
  endSectorNum = Flash_If_GetSectorNum(endAddress);

  if (startSectorNum < 0 || endSectorNum < 0) {
      return -1;
  }

  for (i = startSectorNum; i <= endSectorNum; i++) {
      isNeedErase[i] = 1;
      printf("isNeedErase %d\r\n", i);
  }

  /* unlock the flash program erase controller */
  fmc_unlock();

  /* clear pending flags */
  fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR | FMC_FLAG_PGAERR);

  /* wait the erase operation complete*/
  for (i = 0; i < sizeof(isNeedErase); i++) {
      if (isNeedErase[i] == 1) {
          if (i <= 31) {
              fmc_state = fmc_sector_erase(CTL_SN(i));
              if (fmc_state != FMC_READY) {
                printf("Erase sector %d failed. fmc_state %d\r\n", i, fmc_state);
                goto out;
              }
          } else {
              fmc_state = fmc_sector_erase((SN_5 | CTL_SN(i - 32)));
              if (fmc_state != FMC_READY) {
                printf("Erase sector %d failed. fmc_state %d\r\n", i, fmc_state);
                goto out;
              }
          }
      }
  }

  /* lock the flash program erase controller */
  fmc_lock();

  return 0;

out:
  fmc_lock();
  return -1;
}

/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  FlashAddress: start address for writing data buffer
  * @param  Data: pointer on data buffer
  * @param  DataLength: length of data buffer
  * @retval 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  */
uint32_t FLASH_If_Write(uint32_t FlashAddress, const uint8_t *Data, uint32_t DataLength)
{
    uint32_t ret = 0;
    uint32_t i = 0;
    uint32_t writeAddress = 0;
    uint32_t dataLenDoubleWord = 0;

    writeAddress = FlashAddress;

    /* unlock the flash program erase controller */
    fmc_unlock();
    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR | FMC_FLAG_PGAERR);

    dataLenDoubleWord = DataLength / 8;
    for(i = 0; (i < dataLenDoubleWord) && (writeAddress <= (FLASH_END_ADDRESS - 8)); i++) {
        if(FMC_READY == fmc_doubleword_program(writeAddress, (uint32_t *)(Data + i * 8))) {
            writeAddress += 8;
        } else {
            ret = FLASHIF_WRITING_ERROR;
            goto out;
        }
    }

    for (i = 0; i < DataLength % 8; i++) {
        if(FMC_READY == fmc_byte_program(writeAddress, *(uint8_t *) (Data + writeAddress - FlashAddress))) {
            writeAddress += 1;
        } else {
            ret = FLASHIF_WRITING_ERROR;
            goto out;
        }
    }

    /* lock the flash program erase controller */
    fmc_lock();

    return FLASHIF_OK;

out:
    fmc_lock();
    return ret;
}

/**
  * @brief  Returns the write protection status of user flash area.
  * @param  None
  * @retval 0: No write protected sectors inside the user flash area
  *         1: Some sectors inside the user flash area are write protected
  */
uint16_t FLASH_If_GetWriteProtectionStatus(void)
{
    uint32_t bank0State = 0x0;
    uint32_t bank1State = 0x0;

    /* unlock the flash program erase controller */
    fmc_unlock();

    bank0State = ob_write_protection0_get();
    bank1State = ob_write_protection1_get();

    /* lock the flash program erase controller */
    fmc_lock();

    if (bank0State == 0xFFFFF || bank1State == 0xFFFFF) {
        return FLASHIF_PROTECTION_NONE;
    } else {
        return FLASHIF_PROTECTION_WRPENABLED;
    }
}

/**
  * @brief  Configure the write protection status of user flash area.
  * @param  modifier DISABLE or ENABLE the protection
  * @retval int32_t HAL_OK if change is applied.
  */
int32_t FLASH_If_WriteProtectionConfig(bool enableWriteProtection)
{
    int32_t result;

    fmc_unlock();
    ob_unlock();

    if (enableWriteProtection == true) {
        result = ob_write_protection_enable(OB_WP_ALL);
    } else {
        result = ob_write_protection_disable(OB_WP_ALL);
    }
    ob_start();
    ob_lock();
    fmc_lock();

    return result;
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
