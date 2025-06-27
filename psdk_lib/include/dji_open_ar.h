/**
 ********************************************************************
 * @file    dji_open_ar.h
 * @brief   This is the header file for "dji_open_ar.c", defining the structure and
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
#ifndef DJI_OPEN_AR_H
#define DJI_OPEN_AR_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct
{
    float longitude;
    float latitude;
    float altitude;
} T_DjiOpenArCoordinate;

/** 
 * @brief Text information definition
 */
typedef struct
{
	uint8_t is_show:1; /*!< Whether to display, 0: not display, 1: display */
	uint8_t text[30]; /*!< Text information, with '\0' as the terminator */
} T_DjiOpenArTextAttribute;

typedef struct
{
	uint8_t selected:1; /*!< Whether selected, 0: not selected, 1: selected */
	uint8_t visible :1; /*!< Whether to display, 0: not display, 1: display */
	uint32_t color; /*!< Contains alpha channel */
} T_DjiOpenArIconAttribute;

/** 
* @brief  Click action for a point
*/
typedef struct
{
	uint8_t action;	/*!<  0: click; 1: drag along the line; 2: drag from a distance */
	T_DjiOpenArCoordinate normal_vector; /*!< Dragging direction: normal vector */
} T_DjiOpenArTouchAttribute;

/**
 * @brief Represents the unique identifier for AR elements
 */
typedef struct
{
    uint32_t uuid; /*!< uuid used for updating and deleting */
    uint32_t style_id; /*!< used to index resources config, corresponding resource is configured in widget.json */
} T_DjiOpenArIds;

/**
 * @brief AR face drawing attributes
 */
typedef struct
{
	uint8_t dist_alpha_enable :1; /*!< Enable distance-based alpha blending (1) or not (0) */
	uint8_t face_bottom_enable:1; /*!< Enable drawing bottom face (1) or not (0) */
	uint8_t face_side_enable  :1; /*!< Enable drawing side face (1) or not (0) */
	uint8_t face_top_enable   :1; /*!< Enable drawing top face (1) or not (0) */
} T_DjiOpenArPolygonFaceAttribute;

/**
 * @brief AR border drawing attributes
 */
typedef struct
{
	uint8_t top_show                :1;
	uint8_t top_dist_alpha_enable   :1;
	uint8_t side_show               :1;
	uint8_t side_dist_alpha_enable  :1;
	uint8_t bottom_show             :1;
	uint8_t bottom_dist_alpha_enable:1;
} T_DjiOpenArPolygonStrokeAttribute;

/**
 * @brief  AR pint drawing attibutes
 */
typedef struct
{
	uint32_t uuid; /*!< uuid for point , used to updating and delting */
	uint32_t style_id; /*!< dentifier for the style associated with the point, define with widget.json */
	uint32_t resource_id; /*!< Identifier for the resource associated with the point */
	uint8_t is_always_in_edge; /*!< Whether the point is always on the edge */
	T_DjiOpenArCoordinate coordinate; /*!< Physical position in the world coordinate system */
	T_DjiOpenArTextAttribute text_attr; /*!< Attributes related to the text associated with the point */
	T_DjiOpenArIconAttribute icon_attr; /*!< Attributes related to the icon associated with the point */
	T_DjiOpenArTouchAttribute touch_attr; /*!< Attributes related to touch interactions with the point */
} T_DjiOpenArPoint;

/**
 * @brief Array of points
 */
typedef struct
{
	uint32_t len; /*!< Length of the array (number of points) */
	T_DjiOpenArPoint points[0]; /*!< 0 length implies dynamic allocation */
} T_DjiOpenArPointArray;

/**
 * @biref AR Drawing Line
 */
typedef struct
{
    T_DjiOpenArIds ids; /*!< uuids for the AR line */
	T_DjiOpenArPointArray point_array; /*!< Array of points defining the line */
} T_DjiOpenArLine;

/**
 * @brief AR Drawing Polygon
 */
typedef struct
{
    T_DjiOpenArIds ids; /*!< Unique identifiers for the AR polygon */
	T_DjiOpenArCoordinate normal_vector; /*!< Normal vector of the polygon surface */
	T_DjiOpenArPolygonFaceAttribute face; /*!< Face attributes of the polygon */
	T_DjiOpenArPolygonStrokeAttribute stroke; /*!< Stroke attributes of the polygon */
	T_DjiOpenArPointArray point_array; /*!< Array of points defining the polygon */
} T_DjiOpenArPolygon;

/**
 * @brief AR Drawing Circle
 */
typedef struct
{
	T_DjiOpenArIds ids; /*!< Unique identifiers for the AR circle */
	float radius; /*!< Radius of the circle */
	T_DjiOpenArCoordinate center; /*!< Center coordinates of the circle */
	T_DjiOpenArCoordinate normal_vector; /*!< Normal vector of the circle's surface */
	T_DjiOpenArPolygonFaceAttribute face; /*!< Face attributes of the circle */
	T_DjiOpenArPolygonStrokeAttribute stroke; /*!< Stroke attributes of the circle */
} T_DjiOpenArCircle;

/**
 * @brief AR Drawing 3D Cone Pivot Axis
 */
typedef struct
{
    T_DjiOpenArIds ids; /*!< Unique identifiers for the 3D cone */
	uint8_t show_enable; /*!< Flag indicating if the cone should be displayed (1) or not (0) */
    T_DjiOpenArCoordinate pivot; /*!< Center point of the cone */
	T_DjiOpenArCoordinate normal_vector; /*!< Normal vector for the cone's surface */
} T_DjiOpenArPivotAxis;

/**
 * @brief eletion Entry for Specified Point, Face, Line, etc.
 */
typedef struct
{
	uint32_t resource_id; /*!< Resource ID for the element to be deleted */
    uint32_t uuid_len; /*!< Length of the UUID array */
    uint32_t uuid_array[0]; /*!< Array of UUIDs to identify the elements to be deleted */
} T_DjiOpenArDeletePointEntry;

typedef struct
{
    uint32_t uuid;
} T_DjiOpenArDeleteCommonEntry;

/**
 * @brief Typedef for deletion of line entries
 */
typedef T_DjiOpenArDeleteCommonEntry T_DjiOpenArDeleteLineEntry;

/**
 * @brief Typedef for deletion of polygon entries
 */
typedef T_DjiOpenArDeleteCommonEntry T_DjiOpenArDeletePolygonEntry;

/**
 * @brief Typedef for deletion of circle entries
 */
typedef T_DjiOpenArDeleteCommonEntry T_DjiOpenArDeleteCircleEntry;

/**
 * @brief Typedef for deletion of pivot axis entries
 */
typedef T_DjiOpenArDeleteCommonEntry T_DjiOpenArDeletePovixAxisEntry;

/**
 * @brief Set the ar point.
 * @param metaData: the information for Ar point.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArSetPoint(const T_DjiOpenArPointArray* entry);

/**
 * @brief Update the ar point.
 * @param metaData: the information for Ar point.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArUpdatePoint(const T_DjiOpenArPointArray* entry);

/**
 * @brief Delete the ar point
 * @param metaData: the information for Ar point.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArDeletePoint(const T_DjiOpenArDeletePointEntry* entry);

/**
 * @brief Clear the ar point
 * @param metaData: the resource id for Ar point.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArClearPoint(uint32_t resource_id);

/**
 * @brief Set the ar line
 * @param metaData: the ar line information for setting.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArSetLine(const T_DjiOpenArLine* entry);

/**
 * @brief Update the ar line
 * @param metaData: The ar line information
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArUpdateLine(const T_DjiOpenArLine* entry);

/**
 * @brief Delete the ar line
 * @param metaData: The ar line entry for delete
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArDeleteLine(const T_DjiOpenArDeleteLineEntry* entry, uint32_t entry_len);

/**
 * @brief Clear the ar line.
 * @param metaData: The ar line information for clear.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArClearLine();

/**
 * @brief Set the ar polygon.
 * @param metaData: The ar polygon information for setting.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArSetPolygon(const T_DjiOpenArPolygon* entry);

/**
 * @brief Update the ar polygon.
 * @param metaData: The ar polygon information for update.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArUpdatePolygon(const T_DjiOpenArPolygon* entry);

/**
 * @brief Delete the ar polygon.
 * @param metaData: The polygon entry information for delete.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArDeletePolygon(const T_DjiOpenArDeletePolygonEntry* entry, uint32_t entry_len);

/**
 * @brief Clear the ar polygon.
 * @param metaData: none
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArClearPolygon();

/**
 * @brief Set the ar circle.
 * @param metaData: The ar circle information for setting.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArSetCircle(const T_DjiOpenArCircle* entry);

/**
 * @brief Update the ar circle.
 * @param metaData: The ar circle information for update.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArUpdateCircle(const T_DjiOpenArCircle* entry);

/**
 * @brief Delete the ar circle.
 * @param metaData: The ar circle information for delete.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArDeleteCircle(const T_DjiOpenArDeleteCircleEntry* entry, uint32_t entry_len);

/**
 * @brief Clear the ar circle.
 * @param metaData: The ar circle information for clear.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArClearCircle();

/**
 * @brief Set the pivot axis.
 * @param metaData: The pivot axis information for setting.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArSetPivotAxis(const T_DjiOpenArPivotAxis* entry);

/**
 * @brief Update the pivot axis.
 * @param metaData: The pivot axis information for update.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArUpdatePivotAxis(const T_DjiOpenArPivotAxis* entry);

/**
 * @brief Delete the pivot axis.
 * @param metaData: The pivot axis information for delete.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArDeletePivotAxis(const T_DjiOpenArDeletePovixAxisEntry* entry, uint32_t entry_len);

/**
 * @brief Clear the pivot axis.
 * @param metaData: none
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArClearPivotAxis();

/**
 * @brief Once entering this callback it is necessary to reflesh all AR drawings.
 * @param metaData: none
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
typedef void (*DjiLiveview_ArRefleshAllCallback)(void);

/**
 * @brief Register the ar refresh function.
 * @param metaData: The ar reflesh callback function.
 * @note This interface support on DJI manifold3.
 * @return Execution result.
 */
T_DjiReturnCode DjiLiveview_ArRegRefleshAllCallback(DjiLiveview_ArRefleshAllCallback callback);

#ifdef __cplusplus
}
#endif

#endif // DJI_OPEN_AR_H
