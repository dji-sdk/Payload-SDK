/**
 ********************************************************************
 * @file    test_upgrade_common.h
 * @version V2.0.0
 * @date    3/12/20
 * @brief   This is the header file for "test_upgrade_linux_common.c", defining the structure and
 * (exported) function prototypes.
 *
 * @copyright (c) 2018-2019 DJI. All rights reserved.
 *
 * All information contained herein is, and remains, the property of DJI.
 * The intellectual and technical concepts contained herein are proprietary
 * to DJI and may be covered by U.S. and foreign patents, patents in process,
 * and protected by trade secret or copyright law.  Dissemination of this
 * information, including but not limited to data and other proprietary
 * material(s) incorporated within the information, in any form, is strictly
 * prohibited without the express written consent of DJI.
 *
 * If you receive this source code without DJI’s authorization, you may not
 * further disseminate the information, and you must immediately remove the
 * source code and notify DJI of its removal. DJI reserves the right to pursue
 * legal actions against you for any loss(es) or damage(s) caused by your
 * failure to do so.
 *
 *********************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TEST_UPGRADE_COMMON_H
#define TEST_UPGRADE_COMMON_H

/* Includes ------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
// firmware version
#define PSDK_TEST_FIRMWARE_VERSION_MAJOR        2
#define PSDK_TEST_FIRMWARE_VERSION_MINOR        1
#define PSDK_TEST_FIRMWARE_VERSION_MODIFY       0
#define PSDK_TEST_FIRMWARE_VERSION_DEBUG        3

// define if only emulate upgrade, no system reboot
#define PSDK_TEST_UPGRADE_NO_REBOOT_EMU         0

#if PSDK_ARCH_SYS_LINUX

// define if replace program before reboot
#define PSDK_TEST_REPLACE_PROGRAM_BEFORE_REBOOT 1
// define if use ftp transfer, only support linux
#define PSDK_TEST_UPGRADE_USE_FTP_TRANSFER      1

#else

// for mcu, program is on flash , need replace program in loader, not before reboot
#define PSDK_TEST_REPLACE_PROGRAM_BEFORE_REBOOT 0
// define if use ftp transfer
#define PSDK_TEST_UPGRADE_USE_FTP_TRANSFER      0

#endif

/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif // TEST_UPGRADE_LINUX_COMMON_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
