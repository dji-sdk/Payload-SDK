/**
  ******************************************************************************
  * @file    IAP/IAP_Main/Inc/flash_if.h
  * @author  MCD Application Team
  * @brief   This file provides all the headers of the flash_if functions.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_IF_H
#define __FLASH_IF_H

/* Includes ------------------------------------------------------------------*/
#include "dji_platform.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Base address of the Flash sectors */
/* Bank0 2MB*/
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbyte */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbyte */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbyte */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbyte */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbyte */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbyte */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbyte */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbyte */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbyte */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbyte */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbyte */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbyte */
#define ADDR_FLASH_SECTOR_12    ((uint32_t)0x08100000) /* Base @ of Sector 12, 128 Kbyte */
#define ADDR_FLASH_SECTOR_13    ((uint32_t)0x08120000) /* Base @ of Sector 13, 128 Kbyte */
#define ADDR_FLASH_SECTOR_14    ((uint32_t)0x08140000) /* Base @ of Sector 14, 128 Kbyte */
#define ADDR_FLASH_SECTOR_15    ((uint32_t)0x08160000) /* Base @ of Sector 15, 128 Kbyte */
#define ADDR_FLASH_SECTOR_16    ((uint32_t)0x08180000) /* Base @ of Sector 16, 128 Kbyte */
#define ADDR_FLASH_SECTOR_17    ((uint32_t)0x081A0000) /* Base @ of Sector 17, 128 Kbyte */
#define ADDR_FLASH_SECTOR_18    ((uint32_t)0x081C0000) /* Base @ of Sector 18, 128 Kbyte */
#define ADDR_FLASH_SECTOR_19    ((uint32_t)0x081E0000) /* Base @ of Sector 19, 128 Kbyte */

/* Bank1 2MB*/
#define ADDR_FLASH_SECTOR_20    ((uint32_t)0x08200000) /* Base @ of Sector 20, 16 Kbyte */
#define ADDR_FLASH_SECTOR_21    ((uint32_t)0x08204000) /* Base @ of Sector 21, 16 Kbyte */
#define ADDR_FLASH_SECTOR_22    ((uint32_t)0x08208000) /* Base @ of Sector 22, 16 Kbyte */
#define ADDR_FLASH_SECTOR_23    ((uint32_t)0x0820C000) /* Base @ of Sector 23, 16 Kbyte */
#define ADDR_FLASH_SECTOR_24    ((uint32_t)0x08218000) /* Base @ of Sector 24, 64 Kbyte */
#define ADDR_FLASH_SECTOR_25    ((uint32_t)0x08220000) /* Base @ of Sector 25, 128 Kbyte */
#define ADDR_FLASH_SECTOR_26    ((uint32_t)0x08240000) /* Base @ of Sector 26, 128 Kbyte */
#define ADDR_FLASH_SECTOR_27    ((uint32_t)0x08260000) /* Base @ of Sector 27, 128 Kbyte */
#define ADDR_FLASH_SECTOR_28    ((uint32_t)0x08280000) /* Base @ of Sector 28, 128 Kbyte */
#define ADDR_FLASH_SECTOR_29    ((uint32_t)0x082A0000) /* Base @ of Sector 29, 128 Kbyte */
#define ADDR_FLASH_SECTOR_30    ((uint32_t)0x082C0000) /* Base @ of Sector 30, 128 Kbyte */
#define ADDR_FLASH_SECTOR_31    ((uint32_t)0x082E0000) /* Base @ of Sector 31, 128 Kbyte */
#define ADDR_FLASH_SECTOR_32    ((uint32_t)0x08300000) /* Base @ of Sector 32, 128 Kbyte */
#define ADDR_FLASH_SECTOR_33    ((uint32_t)0x08320000) /* Base @ of Sector 33, 128 Kbyte */
#define ADDR_FLASH_SECTOR_34    ((uint32_t)0x08340000) /* Base @ of Sector 34, 128 Kbyte */
#define ADDR_FLASH_SECTOR_35    ((uint32_t)0x08360000) /* Base @ of Sector 35, 128 Kbyte */
#define ADDR_FLASH_SECTOR_36    ((uint32_t)0x08380000) /* Base @ of Sector 36, 128 Kbyte */
#define ADDR_FLASH_SECTOR_37    ((uint32_t)0x083A0000) /* Base @ of Sector 37, 128 Kbyte */
#define ADDR_FLASH_SECTOR_38    ((uint32_t)0x083C0000) /* Base @ of Sector 38, 128 Kbyte */
#define ADDR_FLASH_SECTOR_39    ((uint32_t)0x083E0000) /* Base @ of Sector 39, 128 Kbyte */

/* Bank1-Ex 3584KB*/
#define ADDR_FLASH_SECTOR_40    ((uint32_t)0x08400000) /* Base @ of Sector 40, 256 Kbyte */
#define ADDR_FLASH_SECTOR_41    ((uint32_t)0x08440000) /* Base @ of Sector 41, 256 Kbyte */
#define ADDR_FLASH_SECTOR_42    ((uint32_t)0x08480000) /* Base @ of Sector 42, 256 Kbyte */
#define ADDR_FLASH_SECTOR_43    ((uint32_t)0x084C0000) /* Base @ of Sector 43, 256 Kbyte */
#define ADDR_FLASH_SECTOR_44    ((uint32_t)0x08500000) /* Base @ of Sector 44, 256 Kbyte */
#define ADDR_FLASH_SECTOR_45    ((uint32_t)0x08540000) /* Base @ of Sector 45, 256 Kbyte */
#define ADDR_FLASH_SECTOR_46    ((uint32_t)0x08580000) /* Base @ of Sector 46, 256 Kbyte */
#define ADDR_FLASH_SECTOR_47    ((uint32_t)0x085C0000) /* Base @ of Sector 47, 256 Kbyte */
#define ADDR_FLASH_SECTOR_48    ((uint32_t)0x08600000) /* Base @ of Sector 48, 256 Kbyte */
#define ADDR_FLASH_SECTOR_49    ((uint32_t)0x08640000) /* Base @ of Sector 49, 256 Kbyte */
#define ADDR_FLASH_SECTOR_50    ((uint32_t)0x08680000) /* Base @ of Sector 50, 256 Kbyte */
#define ADDR_FLASH_SECTOR_51    ((uint32_t)0x086C0000) /* Base @ of Sector 51, 256 Kbyte */
#define ADDR_FLASH_SECTOR_52    ((uint32_t)0x08700000) /* Base @ of Sector 52, 256 Kbyte */
#define ADDR_FLASH_SECTOR_53    ((uint32_t)0x08740000) /* Base @ of Sector 53, 256 Kbyte */

/* Error code */
enum {
    FLASHIF_OK = 0,
    FLASHIF_ERASE_ERROR,
    FLASHIF_WRITINGCTRL_ERROR,
    FLASHIF_WRITING_ERROR
};

enum {
    FLASHIF_PROTECTION_NONE = 0,
    FLASHIF_PROTECTION_PCROPENABLED = 0x1,
    FLASHIF_PROTECTION_WRPENABLED = 0x2,
    FLASHIF_PROTECTION_RDPENABLED = 0x4,
};

#define FLASH_SECTOR_NUM                    54
#define FLASH_END_ADDRESS                   0x0877FFFF

/* Define the address from where user application will be loaded.
   Note: the 1st sector 0x08000000-0x08007FFF is reserved for the IAP code */
#define APPLICATION_ADDRESS                 ADDR_FLASH_SECTOR_5
#define APPLICATION_ADDRESS_END             (ADDR_FLASH_SECTOR_19 - 1)

/* Define the address from where user application will be stored in upgrade mode */
#define APPLICATION_STORE_ADDRESS           ADDR_FLASH_SECTOR_25
#define APPLICATION_STORE_ADDRESS_END       (ADDR_FLASH_SECTOR_39 - 1)

/* Define the user application size */
#define APPLICATION_FLASH_SIZE             (APPLICATION_STORE_ADDRESS_END - APPLICATION_STORE_ADDRESS + 1)

/* Define the address for param store */
#define APPLICATION_PARAM_STORE_ADDRESS     ADDR_FLASH_SECTOR_39
#define APPLICATION_PARAM_STORE_ADDRESS_END (ADDR_FLASH_SECTOR_40 - 1)

/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
uint32_t FLASH_If_Init(void);
uint32_t FLASH_If_Erase(uint32_t StartAddress, uint32_t endAddress);
uint32_t FLASH_If_Write(uint32_t FlashAddress, const uint8_t *Data, uint32_t DataLength);
uint16_t FLASH_If_GetWriteProtectionStatus(void);
int32_t FLASH_If_WriteProtectionConfig(bool enableWriteProtection);

#endif  /* __FLASH_IF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
