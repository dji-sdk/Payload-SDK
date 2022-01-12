/**
 ********************************************************************
 * @file    psdk_template.h
 * @version V0.0.0
 * @date    2017/8/10
 * @brief   This file to indicate PSDK coding style and comment style.
 * @attention Code file rules:
 * rule: file encoding use UTF8;
 * rule: max line length 120 characters;
 * rule: line separator \r\n;
 * rule: use clion auto code format tool.
 *
 * If not want to format some code, use @formatter:off & @formatter:on to disable code format, for example:
 * //@formatter:off
 * extern int g_testCodeFormatOFF;
 * //@formatter:on
 *
 * @copyright (c) 2017-2018 DJI. All rights reserved.
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
#ifndef PSDK_TEMPLATE_H
#define PSDK_TEMPLATE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
#define PSDK_TEMPLATE_MAX_BUFFER_SIZE      256

/**
 * @defgroup psdk_some_flag
 * @{
 */
#define PSDK_TEMPLATE_SUCCESS               0
#define PSDK_TEMPLATE_FAILURE               -1

#define PSDK_SOME_FLAG_AAA                   1    /*!< Define line comment. */
#define PSDK_SOME_FLAG_BBB                   2    /*!< Define line comment,
                                                          if comment is too long, split line. */
#define PSDK_SOME_FLAG_CCC                   3

/* Exported macros -----------------------------------------------------------*/

#define IS_PSDK_SOME_FLAG(FLAG)   (((FLAG) == PSDK_SOME_FLAG_AAA) || \
                                     ((FLAG) == PSDK_SOME_FLAG_BBB) || \
                                     ((FLAG) == PSDK_SOME_FLAG_CCC))

/**
 * @}
 */

#define PSDK_TEMPLATE_MIN(a, b) (((a)<(b))?(a):(b))

/* Exported types ------------------------------------------------------------*/

/**
 * @brief Enum brief comment.
 */
typedef enum {
    TEMPLATE_COLOR_RED = 0,
    TEMPLATE_COLOR_GREED = 1,
    TEMPLATE_COLOR_BLUE = 2,
} E_TemplateColor;

/**
  * @brief Struct brief comment.
  */
typedef struct {
    uint8_t structMember1;        /*!< structure member comment. */
    uint8_t longStructMember2;    /*!< structure member comment. */

    uint8_t someFlag;             /*!< if this value use marco define ,must use defgroup to indicate, and comment here
                                        use ref. for example:
                                        someFlag of PSDK template. This parameter can be a value of @ref psdk_some_flag */
} T_StructDefine;

typedef void (*PsdkTemplateFunc)(void);

typedef int Inttype;

/* Use pack(1) to transfer structure. */
#pragma pack(1)

typedef struct {
    uint8_t structMember1;
    uint32_t structMember2;
} T_StructProtocolCommunication;

#pragma pack()

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void PsdkTemplate_Init(void);
int PsdkTemplate_Send(const uint8_t *buffer, uint16_t len);

/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif //PSDK_TEMPLATE_H

/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
