/**
 ********************************************************************
 * @file    button.h
 * @version V0.0.0
 * @date    2019/01/01
 * @brief   This is the header file for "button.c", defining the structure and
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
#ifndef BUTTON_H
#define BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
typedef enum
{
    BUTTON_MODE_GPIO = 0,
}ButtonMode_TypeDef;

typedef enum
{
    BUTTON_KEY1 = 0,
    BUTTON_NUM = 1,
}Button_TypeDef;

/* Exported functions --------------------------------------------------------*/
void Button_Init(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);
uint32_t Button_GetState(Button_TypeDef Button);

#ifdef __cplusplus
}
#endif

#endif // BUTTON_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
