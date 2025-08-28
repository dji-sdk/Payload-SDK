/**
 ********************************************************************
 * @file    dji_sdk_config.h
 * @brief   This is the header file for "dji_config.c", defining the structure and
 * (exported) function prototypes.
 *
 * @copyright (c) 2021 DJI. All rights reserved.
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
#ifndef DJI_SDK_CONFIG_H
#define DJI_SDK_CONFIG_H

/* Includes ------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
#define CONFIG_MODULE_SAMPLE_POWER_MANAGEMENT_ON

#define CONFIG_MODULE_SAMPLE_WIDGET_ON

#define CONFIG_MODULE_SAMPLE_WIDGET_SPEAKER_ON

#define CONFIG_MODULE_SAMPLE_DATA_TRANSMISSION_ON

#define CONFIG_MODULE_SAMPLE_FC_SUBSCRIPTION_ON

#define CONFIG_MODULE_SAMPLE_GIMBAL_EMU_ON

#define CONFIG_MODULE_SAMPLE_CAMERA_ON

#define CONFIG_MODULE_SAMPLE_XPORT_ON

#define CONFIG_MODULE_SAMPLE_UPGRADE_ON

#define CONFIG_MODULE_SAMPLE_HMS_CUSTOMIZATION_ON

/*!< Attention: Please uncomment it in gps environment.
* */
// #define CONFIG_MODULE_SAMPLE_TIME_SYNC_ON

// #define CONFIG_MODULE_SAMPLE_POSITIONING_ON

#define CONFIG_MODULE_SAMPLE_TETHERED_BATTERY_ON

#define USER_FIRMWARE_MAJOR_VERSION         (1)
#define USER_FIRMWARE_MINOR_VERSION         (0)
#define USER_FIRMWARE_MODIFY_VERSION        (0)
#define USER_FIRMWARE_DEBUG_VERSION         (99)

/* Exported types ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif // DJI_SDK_CONFIG_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
