/**
 ********************************************************************
 * @file    dji_camera_manager.h
 * @brief   This is the header file for "dji_camera_manager.c", defining the structure and
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
#ifndef DJI_CAMERA_MANAGER_H
#define DJI_CAMERA_MANAGER_H

/* Includes ------------------------------------------------------------------*/
#include "dji_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/*! @brief CameraModule work modes.
 */
typedef enum {
    /*!
     - Capture mode. In this mode, the user can capture pictures.
     */
    DJI_CAMERA_MANAGER_WORK_MODE_SHOOT_PHOTO = 0,
    /*!
     - Record mode. In this mode, the user can record videos.
     */
    DJI_CAMERA_MANAGER_WORK_MODE_RECORD_VIDEO = 1,
    /*!
     - Playback mode. In this mode, the user can preview photos and videos, and
     can delete files. It is supported by Phantom 3 Professional camera, X3, X5
     and X5R cameras on aircraft and Phantom 4 camera. Playback mode is not
     supported by Z30, X5S, X4S, Phantom 4 Pro, Mavic Pro, Phantom 3 Standard,
     Phantom 3 Advanced, Phantom 3 4K and Osmo series.
     */
    DJI_CAMERA_MANAGER_WORK_MODE_PLAYBACK = 2,
    /*!
     - In this mode, the user can download media to the Mobile Device. Not
     supported by X5 camera nor X5R camera while mounted on aircraft.
     */
    DJI_CAMERA_MANAGER_WORK_MODE_MEDIA_DOWNLOAD = 3,
    /*!
     - In this mode, live stream resolution and frame rate will be 1080i50 (PAL)
     or 720p60 (NTSC). In this mode videos can be recorded. Still photos can
     also be taken only when video is recording. The only way to exit broadcast
     mode is to change modes to RECORD_VIDEO. Only supported by Inspire 2.
     */
    DJI_CAMERA_MANAGER_WORK_MODE_BROADCAST = 4,
    /*!
     * The camera work mode is unknown.
     */
    DJI_CAMERA_MANAGER_WORK_MODE_WORK_MODE_UNKNOWN = 0xFF,
} E_DjiCameraManagerWorkMode;

/*! @brief The ShootPhoto mode itself can have several modes. The default
 * value is SINGLE.
 */
typedef enum {
    /*!
     - Sets the camera to take a single photo.
     */
    DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_SINGLE = 0x01,
    /*!
     - Sets the camera to take an HDR photo. X5 camera, X5R camera, XT camera,
     Z30 camera, Phantom 4 Pro camera, X4S camera and X5S camera do not support
     HDR mode.
     */
    DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_HDR = 0x02,
    /*!
     - Set the camera to take multiple photos at once. XT camera does not
     support Burst mode.
     */
    DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_BURST = 0x04,
    /*!
     - Automatic Exposure Bracketing (AEB) capture. In this mode you can quickly
     take multiple shots (the default is 3) at different exposures without
     having to manually change any settings between frames. XT camera and Z30
     camera does not support AEB mode.
     */
    DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_AEB = 0x05,
    /*!
     - Sets the camera to take a picture (or multiple pictures) continuously at
     a set time interval. The minimum interval for JPEG format of any quality is
     2s. For all cameras except X4S, X5S and Phantom 4 Pro camera: The minimum
     interval for RAW or RAW+JPEG format is 10s. For the X4S, X5S and Phantom 4
     Pro cameras the minimum interval for RAW or RAW+JPEG dformat is 5s.
     */
    DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_INTERVAL = 0x06,
    /*!
     - Sets the camera to take a burst of RAW photos. Use getRAWPhotoBurstCount
     to check how many photos have been shot. Only supported by X5S.
     */
    DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_RAW_BURST = 0x09,
    /*!
     - 	Sets the camera to take an regional photos. It is supported by H20/H20T.
     */
    DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_REGIONAL_SR = 0x16,
    /*!
     - The shoot photo mode is unknown.
     */
    DJI_CAMERA_MANAGER_SHOOT_PHOTO_MODE_UNKNOWN = 0xFF,
} E_DjiCameraManagerShootPhotoMode;

/*! @brief the photo action of INTERVAL shooting photo mode
 */
typedef enum {
    DJI_CAMERA_MANAGER_EXPOSURE_MODE_PROGRAM_AUTO = 1,       /*!< Program mode */
    DJI_CAMERA_MANAGER_EXPOSURE_MODE_SHUTTER_PRIORITY = 2,   /*!< Shutter priority mode */
    DJI_CAMERA_MANAGER_EXPOSURE_MODE_APERTURE_PRIORITY = 3,  /*!< Aperture priority mode */
    DJI_CAMERA_MANAGER_EXPOSURE_MODE_EXPOSURE_MANUAL = 4,    /*!< Manual mode */
    DJI_CAMERA_MANAGER_EXPOSURE_MODE_EXPOSURE_UNKNOWN = 0xFF /*!< The camera exposure mode is unknown. */
} E_DjiCameraManagerExposureMode;

/*! @brief the photo action of INTERVAL shooting photo mode
 */
typedef enum {
    /*! The number of pictures to continuously take at one time in AEB mode is 3
     */
    DJI_CAMERA_MANAGER_PHOTO_AEB_COUNT_3 = 3,
    /*! The number of pictures to continuously take at one time in AEB mode is 5
     */
    DJI_CAMERA_MANAGER_PHOTO_AEB_COUNT_5 = 5,
    /*! The number of pictures to continuously take at one time in AEB mode is 7
     */
    DJI_CAMERA_MANAGER_PHOTO_AEB_COUNT_7 = 7,
    /*! The number of pictures to continuously take at one time in AEB mode is
     * unknown.
     */
    DJI_CAMERA_MANAGER_PHOTO_AEB_COUNT_KNOWN = 0xFF,
} E_DjiCameraManagerPhotoAEBCount;

/*! @breif CameraModule focus mode. If the physical AF switch on the camera is
 * set to auto.
 */
typedef enum {
    /*!
     - The camera's focus mode is set to manual. In this mode, user sets the
     focus ring value to adjust the focal distance.
     */
    DJI_CAMERA_MANAGER_FOCUS_MODE_MANUAL = 0,
    /*!
     - The camera's focus mode is set to auto. For the Z30 camera, the focus is
     calculated completely automatically. For all other cameras, a focus target
     can be set by the user, which is used to calculate focus automatically.
     */
    DJI_CAMERA_MANAGER_FOCUS_MODE_AUTO = 1,
    /*!
     - The camera's focus mode is set to Continuous AF. It is only supported by
     Mavic Pro with firmware version V01.03.0000 or above, X4S camera, Mavic 2
     Zoom camera and Mavic 2 Pro camera.
     */
    DJI_CAMERA_MANAGER_FOCUS_MODE_AFC = 2,
    /*!
     - The camera's focus mode is unknown.
     */
    DJI_CAMERA_MANAGER_FOCUS_MODE_UNKNOWN = 0xFF,
} E_DjiCameraManagerFocusMode;

/*! @breif CameraModule shutter mode.
 */
typedef enum {
    /*! The shutter mode of camera is automatical */
    DJI_CAMERA_MANAGER_SHUTTER_AUTO_MODE = 0x00,
    /*! The shutter mode of camera is manual, the shutter speed setting is
       valid. */
    DJI_CAMERA_MANAGER_SHUTTER_MANUAL_MODE = 0x01,
} E_DjiCameraManagerShutterMode;

/*! @brief CameraModule shutter speed values.
 */
typedef enum {
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_8000 = 0,     /*!< 1/8000 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_6400 = 1,     /*!< 1/6400 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_6000 = 2,     /*!< 1/6000 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_5000 = 3,     /*!< 1/5000 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_4000 = 4,     /*!< 1/4000 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_3200 = 5,     /*!< 1/3200 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_3000 = 6,     /*!< 1/3000 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_2500 = 7,     /*!< 1/2500 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_2000 = 8,     /*!< 1/2000 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_1600 = 9,     /*!< 1/1600 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_1500 = 10,    /*!< 1/1500 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_1250 = 11,    /*!< 1/1250 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_1000 = 12,    /*!< 1/1000 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_800 = 13,     /*!< 1/800 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_725 = 14,     /*!< 1/725 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_640 = 15,     /*!< 1/640 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_500 = 16,     /*!< 1/500 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_400 = 17,     /*!< 1/400 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_350 = 18,     /*!< 1/350 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_320 = 19,     /*!< 1/320 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_250 = 20,     /*!< 1/250 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_240 = 21,     /*!< 1/240 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_200 = 22,     /*!< 1/200 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_180 = 23,     /*!< 1/180 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_160 = 24,     /*!< 1/160 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_125 = 25,     /*!< 1/125 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_120 = 26,     /*!< 1/120 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_100 = 27,     /*!< 1/100 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_90 = 28,      /*!< 1/90 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_80 = 29,      /*!< 1/80 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_60 = 30,      /*!< 1/60 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_50 = 31,      /*!< 1/50 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_40 = 32,      /*!< 1/40 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_30 = 33,      /*!< 1/30 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_25 = 34,      /*!< 1/25 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_20 = 35,      /*!< 1/20 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_15 = 36,      /*!< 1/15 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_12DOT5 = 37,  /*!< 1/12.5 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_10 = 38,      /*!< 1/10 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_8 = 39,       /*!< 1/8 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_6DOT25 = 40,  /*!< 1/6.25 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_5 = 41,       /*!< 1/5 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_4 = 42,       /*!< 1/4 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_3 = 43,       /*!< 1/3 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_2DOT5 = 44,   /*!< 1/2.5 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_2 = 45,       /*!< 1/2 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_1DOT67 = 46,  /*!< 1/1.67 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1_1DOT25 = 47,  /*!< 1/1.25 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1 = 48,         /*!< 1.0 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1DOT3 = 49,     /*!< 1.3 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_1DOT6 = 50,     /*!< 1.6 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_2 = 51,         /*!< 2.0 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_2DOT5 = 52,     /*!< 2.5 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_3 = 53,         /*!< 3.0 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_3DOT2 = 54,     /*!< 3.2 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_4 = 55,         /*!< 4.0 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_5 = 56,         /*!< 5.0 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_6 = 57,         /*!< 6.0 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_7 = 58,         /*!< 7.0 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_8 = 59,         /*!< 8.0 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_9 = 60,         /*!< 9.0 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_10 = 61,        /*!< 10.0 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_13 = 62,        /*!< 13.0 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_15 = 63,        /*!< 15.0 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_20 = 64,        /*!< 20.0 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_25 = 65,        /*!< 25.0 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_30 = 66,        /*!< 30.0 s */
    DJI_CAMERA_MANAGER_SHUTTER_SPEED_UNKNOWN = 0xFF, /*!< Unknown */
} E_DjiCameraManagerShutterSpeed;

/*! @brief CameraModule ISO values.
 */
typedef enum {
    /*! The ISO value is automatically set. This cannot be used for all cameras
       when in Manual mode. */
    DJI_CAMERA_MANAGER_ISO_AUTO = 0x00,
    /*!  The ISO value is set to 100. */
    DJI_CAMERA_MANAGER_ISO_100 = 0x03,
    /*! The ISO value is set to 100. */
    DJI_CAMERA_MANAGER_ISO_200 = 0x04,
    /*! The ISO value is set to 100.*/
    DJI_CAMERA_MANAGER_ISO_400 = 0x05,
    /*! The ISO value is set to 100.*/
    DJI_CAMERA_MANAGER_ISO_800 = 0x06,
    /*! The ISO value is set to 100.*/
    DJI_CAMERA_MANAGER_ISO_1600 = 0x07,
    /*! The ISO value is set to 100.*/
    DJI_CAMERA_MANAGER_ISO_3200 = 0x08,
    /*! The ISO value is set to 100.*/
    DJI_CAMERA_MANAGER_ISO_6400 = 0x09,
    /*! The ISO value is set to 100.*/
    DJI_CAMERA_MANAGER_ISO_12800 = 0x0A,
    /*! The ISO value is set to 100.*/
    DJI_CAMERA_MANAGER_ISO_25600 = 0x0B,
    /*! ISO value is fixed by the camera firmware. When the camera color is set
     to D_LOG, camera will fix the ISO to a specific value in order to optimize
     the performance.
     */
    DJI_CAMERA_MANAGER_ISO_FIXED = 0xFF,
} E_DjiCameraManagerISO;

/*! @brief CameraModule exposure compensation.
 */
typedef enum {
    /*! The camera's exposure compensation is -5.0ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_5_0 = 1,
    /*! The camera's exposure compensation is -4.7ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_4_7 = 2,
    /*! The camera's exposure compensation is -4.3ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_4_3 = 3,
    /*! The camera's exposure compensation is -4.0ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_4_0 = 4,
    /*! The camera's exposure compensation is -3.7ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_3_7 = 5,
    /*! The camera's exposure compensation is -3.3ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_3_3 = 6,
    /*! The camera's exposure compensation is -3.0ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_3_0 = 7,
    /*! The camera's exposure compensation is -2.7ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_2_7 = 8,
    /*! The camera's exposure compensation is -2.3ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_2_3 = 9,
    /*! The camera's exposure compensation is -2.0ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_2_0 = 10,
    /*! The camera's exposure compensation is -1.7ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_1_7 = 11,
    /*! The camera's exposure compensation is -1.3ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_1_3 = 12,
    /*! The camera's exposure compensation is -1.0ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_1_0 = 13,
    /*! The camera's exposure compensation is -0.7ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_0_7 = 14,
    /*! The camera's exposure compensation is -0.3ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_0_3 = 15,
    /*! The camera's exposure compensation is 0.0ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_N_0_0 = 16,
    /*! The camera's exposure compensation is +0.3ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_0_3 = 17,
    /*! The camera's exposure compensation is +0.7ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_0_7 = 18,
    /*! The camera's exposure compensation is +1.0ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_1_0 = 19,
    /*! The camera's exposure compensation is +1.3ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_1_3 = 20,
    /*! The camera's exposure compensation is +1.7ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_1_7 = 21,
    /*! The camera's exposure compensation is +2.0ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_2_0 = 22,
    /*! The camera's exposure compensation is +2.3ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_2_3 = 23,
    /*! The camera's exposure compensation is +2.7ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_2_7 = 24,
    /*! The camera's exposure compensation is +3.0ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_3_0 = 25,
    /*! The camera's exposure compensation is +3.3ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_3_3 = 26,
    /*! The camera's exposure compensation is +3.7ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_3_7 = 27,
    /*! The camera's exposure compensation is +4.0ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_4_0 = 28,
    /*! The camera's exposure compensation is +4.3ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_4_3 = 29,
    /*! The camera's exposure compensation is +4.7ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_4_7 = 30,
    /*! The camera's exposure compensation is +5.0ev.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_P_5_0 = 31,
    /*! The camera's exposure compensation is fixed by the camera.*/
    DJI_CAMERA_MANAGER_EXPOSURE_COMPENSATION_FIXED = 0xFF,
} E_DjiCameraManagerExposureCompensation;

/*! @brief CameraModule aperture values.
 *  @note X5, X5R, Z30, Phantom 4 Pro camera, X4S and X5S support this
 * setting.
 */
typedef enum {
    /*! 	The Aperture value is f/1.6. It is only supported by Z30
       camera.*/
    DJI_CAMERA_MANAGER_APERTURE_F_1_DOT_6 = 160,
    /*! The Aperture value is f/1.7.*/
    DJI_CAMERA_MANAGER_APERTURE_F_1_DOT_7 = 170,
    /*! The Aperture value is f/1.8.*/
    DJI_CAMERA_MANAGER_APERTURE_F_1_DOT_8 = 180,
    /*! The Aperture value is f/2.*/
    DJI_CAMERA_MANAGER_APERTURE_F_2 = 200,
    /*! The Aperture value is f/2.2.*/
    DJI_CAMERA_MANAGER_APERTURE_F_2_DOT_2 = 220,
    /*! The Aperture value is f/2.4. It is only supported by Z30 camera.*/
    DJI_CAMERA_MANAGER_APERTURE_F_2_DOT_4 = 240,
    /*! The Aperture value is f/2.5.*/
    DJI_CAMERA_MANAGER_APERTURE_F_2_DOT_5 = 250,
    /*! The Aperture value is f/2.6.*/
    DJI_CAMERA_MANAGER_APERTURE_F_2_DOT_6 = 260,
    /*! The Aperture value is f/2.8.*/
    DJI_CAMERA_MANAGER_APERTURE_F_2_DOT_8 = 280,
    /*! The Aperture value is f/3.2.*/
    DJI_CAMERA_MANAGER_APERTURE_F_3_DOT_2 = 320,
    /*! The Aperture value is f/3.4.*/
    DJI_CAMERA_MANAGER_APERTURE_F_3_DOT_4 = 340,
    /*! The Aperture value is f/3.5.*/
    DJI_CAMERA_MANAGER_APERTURE_F_3_DOT_5 = 350,
    /*! The Aperture value is f/4.*/
    DJI_CAMERA_MANAGER_APERTURE_F_4 = 400,
    /*! The Aperture value is f/4.5.*/
    DJI_CAMERA_MANAGER_APERTURE_F_4_DOT_5 = 450,
    /*! The Aperture value is f/4.8.*/
    DJI_CAMERA_MANAGER_APERTURE_F_4_DOT_8 = 480,
    /*! The Aperture value is f/5.*/
    DJI_CAMERA_MANAGER_APERTURE_F_5 = 500,
    /*! The Aperture value is f/5.6.*/
    DJI_CAMERA_MANAGER_APERTURE_F_5_DOT_6 = 560,
    /*! The Aperture value is f/6.3.*/
    DJI_CAMERA_MANAGER_APERTURE_F_6_DOT_3 = 630,
    /*! The Aperture value is f/6.8.*/
    DJI_CAMERA_MANAGER_APERTURE_F_6_DOT_8 = 680,
    /*! The Aperture value is f/7.1.*/
    DJI_CAMERA_MANAGER_APERTURE_F_7_DOT_1 = 710,
    /*! The Aperture value is f/8.*/
    DJI_CAMERA_MANAGER_APERTURE_F_8 = 800,
    /*! The Aperture value is f/9.*/
    DJI_CAMERA_MANAGER_APERTURE_F_9 = 900,
    /*! The Aperture value is f/9.6.*/
    DJI_CAMERA_MANAGER_APERTURE_F_9_DOT_6 = 960,
    /*! The Aperture value is f/10.*/
    DJI_CAMERA_MANAGER_APERTURE_F_10 = 1000,
    /*! The Aperture value is f/11.*/
    DJI_CAMERA_MANAGER_APERTURE_F_11 = 1100,
    /*! The Aperture value is f/13.*/
    DJI_CAMERA_MANAGER_APERTURE_F_13 = 1300,
    /*! The Aperture value is f/14.*/
    DJI_CAMERA_MANAGER_APERTURE_F_14 = 1400,
    /*! The Aperture value is f/16.*/
    DJI_CAMERA_MANAGER_APERTURE_F_16 = 1600,
    /*! The Aperture value is f/18.*/
    DJI_CAMERA_MANAGER_APERTURE_F_18 = 1800,
    /*! The Aperture value is f/19.*/
    DJI_CAMERA_MANAGER_APERTURE_F_19 = 1900,
    /*! The Aperture value is f/20.*/
    DJI_CAMERA_MANAGER_APERTURE_F_20 = 2000,
    /*! The Aperture value is f/22.*/
    DJI_CAMERA_MANAGER_APERTURE_F_22 = 2200,
    /*! The Aperture value is Unknown. */
    DJI_CAMERA_MANAGER_APERTURE_F_UNKNOWN = 0xFFFF,
} E_DjiCameraManagerAperture;

typedef enum {
    DJI_CAMERA_MANAGER_RECORDING_CONTROL_STOP = 0,
    DJI_CAMERA_MANAGER_RECORDING_CONTROL_BEGIN = 1,
    DJI_CAMERA_MANAGER_RECORDING_CONTROL_PAUSE = 2,
    DJI_CAMERA_MANAGER_RECORDING_CONTROL_RESUME = 3,
} E_DjiCameraManagerRecordingControl;

typedef enum {
    DJI_CAMERA_MANAGER_FILE_LIST_COUNT_60_PER_SLICE = 60,
    DJI_CAMERA_MANAGER_FILE_LIST_COUNT_120_PER_SLICE = 120,
    DJI_CAMERA_MANAGER_FILE_LIST_COUNT_ALL_PER_SLICE = 0xFFFF,
} E_DjiCameraManagerFileListCountPerSlice;

typedef struct {
    uint8_t firmware_version[4];
} T_DjiCameraManagerFirmwareVersion;

/*! @brief Tap zoom target point data struct, used by user.
 */
typedef T_DjiCameraPointInScreen T_DjiCameraManagerTapZoomPosData;

/*! @brief Tap focus target point data struct, used by user.
 */
typedef T_DjiCameraPointInScreen T_DjiCameraManagerFocusPosData;

typedef struct {
    dji_f32_t currentOpticalZoomFactor;
    dji_f32_t maxOpticalZoomFactor;
} T_DjiCameraManagerOpticalZoomParam;

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} T_DjiCameraManagerFileCreateTime;

typedef struct {
    union {
        struct {
            uint32_t attributePhotoReserved: 22;
            uint32_t attributePhotoRatio: 8;
            uint32_t attributePhotoRotation: 2;
            uint8_t reserved[12];
        } photoAttribute;
        struct {
            uint32_t attributeVideoDuration: 16;
            uint32_t attributeVideoFramerate: 6;
            uint32_t attributeVideoRotation: 2;
            uint32_t attributeVideoResolution: 8;
            uint8_t reserved[12];
        } videoAttribute;
    };
} T_DjiCameraManagerFileAttributeData;

typedef struct {
    char fileName[256];
    uint32_t fileSize;
    uint32_t fileIndex;
    T_DjiCameraManagerFileCreateTime createTime;
    E_DjiCameraMediaFileType type;
    T_DjiCameraManagerFileAttributeData attributeData;
} T_DjiCameraManagerFileListInfo;

typedef struct {
    uint16_t totalCount;
    T_DjiCameraManagerFileListInfo *fileListInfo;
} T_DjiCameraManagerFileList;

typedef struct {
    uint16_t sliceStartIndex;
    E_DjiCameraManagerFileListCountPerSlice countPerSlice;
} T_DjiCameraManagerSliceConfig;

typedef enum {
    DJI_DOWNLOAD_FILE_EVENT_START,
    DJI_DOWNLOAD_FILE_EVENT_TRANSFER,
    DJI_DOWNLOAD_FILE_EVENT_END,
} E_DjiDownloadFileEvent;

/*!< Attention: when the remote control is in split-screen mode, the coordinate range of the x-axis is 0 ~ 0.5
* */
typedef struct {
    dji_f32_t pointX;               /*! x-coordinate of point thermometry, range: 0 ~ 1 */
    dji_f32_t pointY;               /*! y-coordinate of point thermometry, range: 0 ~ 1 */
} T_DjiCameraManagerPointThermometryCoordinate;

typedef struct {
    dji_f32_t areaTempLtX;          /*! x-coordinate of the upper left corner of the area thermometry, range: 0 ~ 1 */
    dji_f32_t areaTempLtY;          /*! y-coordinate of the upper left corner of the area thermometry, range: 0 ~ 1 */
    dji_f32_t areaTempRbX;          /*! x-coordinate of the lower right corner of the area thermometry, range: 0 ~ 1 */
    dji_f32_t areaTempRbY;          /*! y-coordinate of the lower right corner of the area thermometry, range: 0 ~ 1 */
} T_DjiCameraManagerAreaThermometryCoordinate;

//result of point thermometry
typedef struct {
    dji_f32_t pointX;              /*! x-coordinate of point thermometry, range: 0 ~ 1 */
    dji_f32_t pointY;              /*! y-coordinate of point thermometry, range: 0 ~ 1 */
    dji_f32_t pointTemperature;    /*! The temperature of the current point */
} T_DjiCameraManagerPointThermometryData;

//result of area thermometry
typedef struct {
    dji_f32_t areaTempLtX;           /*! x_coordinate of the upper left corner of the current thermometry area */
    dji_f32_t areaTempLtY;           /*! y_coordinate of the upper left corner of the current thermometry area */
    dji_f32_t areaTempRbX;           /*! x_coordinate of the lower right corner of the current thermometry area */
    dji_f32_t areaTempRbY;           /*! y_coordinate of the lower right corner of the current thermometry area */
    dji_f32_t areaAveTemp;           /*! The average temperature of the current thermometry area */
    dji_f32_t areaMinTemp;           /*! The minimum temperature of the current thermometry area */
    dji_f32_t areaMaxTemp;           /*! The maximum temperature of the current thermometry area */
    dji_f32_t areaMinTempPointX;     /*! x_coordinate of the minimum temperature in the thermometry area */
    dji_f32_t areaMinTempPointY;     /*! y_coordinate of the minimum temperature in the thermometry area */
    dji_f32_t areaMaxTempPointX;     /*! x_coordinate of the maximum temperature in the thermometry area */
    dji_f32_t areaMaxTempPointY;     /*! y_coordinate of the maximum temperature in the thermometry area */
} T_DjiCameraManagerAreaThermometryData;

typedef struct {
    E_DjiDownloadFileEvent downloadFileEvent;
    uint32_t fileIndex;
    uint32_t fileSize;
    dji_f32_t progressInPercent;
} T_DjiDownloadFilePacketInfo;

typedef struct {
    dji_f64_t longitude; /*! Range: [-180,180] */
    dji_f64_t latitude; /*! Range: [-90,90] */
    int32_t altitude; /*! Unit: 0.1m */
    int32_t distance; /*! Unit: 0.1m */
    int16_t screenX; /*! Unit: 0.1% */
    int16_t screenY; /*! Unit: 0.1% */
    bool enable_lidar;
    uint8_t exception;
} T_DjiCameraManagerLaserRangingInfo;

typedef T_DjiReturnCode (*DjiCameraManagerDownloadFileDataCallback)(T_DjiDownloadFilePacketInfo packetInfo,
                                                                    const uint8_t *data,
                                                                    uint16_t dataLen);

/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialise camera manager module, and user should call this function
 * before using camera manager features.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_Init(void);

/**
 * @brief Deinitialise camera manager module.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_DeInit(void);

/**
 * @brief Get camera type of the selected camera mounted position.
 * @param position: camera mounted position
 * @param cameraType: see references of E_DjiCameraType.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetCameraType(E_DjiMountPosition position, E_DjiCameraType *cameraType);

/**
 * @brief Get camera firmware version of the selected camera mounted position.
 * @param position: camera mounted position
 * @param firmwareVersion: see references of T_DjiCameraManagerFirmwareVersion.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetFirmwareVersion(E_DjiMountPosition position,
                                                    T_DjiCameraManagerFirmwareVersion *firmwareVersion);

/**
 * @brief Set camera working mode of the selected camera mounted position.
 * @note Set the camera's work mode to taking pictures, video, playback or
 * download and so on. Please note that you cannot change the mode when a certain task
 * is executing.This action will cost about 1~2s.
 * @param position: camera mounted position
 * @param workMode: see reference of E_DjiCameraManagerWorkMode.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetMode(E_DjiMountPosition position,
                                         E_DjiCameraManagerWorkMode workMode);

/**
 * @brief Get camera working mode of the selected camera mounted position.
 * @param position: camera mounted position
 * @param workMode: see reference of E_DjiCameraManagerWorkMode.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetMode(E_DjiMountPosition position,
                                         E_DjiCameraManagerWorkMode *workMode);

/**
* @brief Set camera shoot mode of the selected camera mounted position.
* @param position: camera mounted position
* @param mode: see reference of E_DjiCameraManagerShootPhotoMode.
* @return Execution result.
*/
T_DjiReturnCode DjiCameraManager_SetShootPhotoMode(E_DjiMountPosition position,
                                                   E_DjiCameraManagerShootPhotoMode mode);

/**
* @brief Get camera shoot mode of the selected camera mounted position.
* @param position: camera mounted position
* @param mode: see reference of E_DjiCameraManagerShootPhotoMode.
* @return Execution result.
*/
T_DjiReturnCode DjiCameraManager_GetShootPhotoMode(E_DjiMountPosition position,
                                                   E_DjiCameraManagerShootPhotoMode *takePhotoMode);

/**
 * @brief Start to shoot photo.
 * @note Camera must be in ShootPhoto mode. For thermal imaging camera,
 * Single photo can be taken while recording video. The SD card state should
 * be checked before this method is used to ensure sufficient space exists.
 * @param position: camera mounted position
 * @param mode: see reference of E_DjiCameraManagerShootPhotoMode.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_StartShootPhoto(E_DjiMountPosition position,
                                                 E_DjiCameraManagerShootPhotoMode mode);

/**
 * @brief Stop to shoot photo when you are in taking photo.
 * @note StartShootPhoto has been invoked and the shoot mode is either
 * Interval or Time-lapse. If the shoot mode is set to single, the camera
 * will automatically stop taking the photo once the individual photo is
 * taken.
 * @param position: camera mounted position
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_StopShootPhoto(E_DjiMountPosition position);

/**
 * @brief Set the burst count in the burst take-photo mode.
 * @param position: camera mounted position
 * @param count: see reference of E_DjiCameraBurstCount.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetPhotoBurstCount(E_DjiMountPosition position,
                                                    E_DjiCameraBurstCount count);

/**
 * @brief Get the burst count in the burst take-photo mode.
 * @param position: camera mounted position
 * @param count: see reference of E_DjiCameraBurstCount.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetPhotoBurstCount(E_DjiMountPosition position,
                                                    E_DjiCameraBurstCount *count);

/**
 * @brief Set the burst count in the AEB(Automatic Exposure Bracketing) take-photo mode.
 * @param position: camera mounted position
 * @param count: see reference of E_DjiCameraManagerPhotoAEBCount.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetPhotoAEBCount(E_DjiMountPosition position,
                                                  E_DjiCameraManagerPhotoAEBCount count);

/**
 * @brief Get the burst count in the AEB(Automatic Exposure Bracketing) take-photo mode.
 * @param position: camera mounted position
 * @param count: see reference of E_DjiCameraManagerPhotoAEBCount.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetPhotoAEBCount(E_DjiMountPosition position,
                                                  E_DjiCameraManagerPhotoAEBCount *count);

/**
 * @brief Set the parameters in the INTERVAL take-photo mode.
 * @note When in this shoot-photo mode, The camera will capture a photo, wait
 * a specified interval of time, take another photo, and continue in this
 * manner until it has taken the required number of photos. Also supported by
 * thermal imaging camera.
 * @param position: camera mounted position
 * @param intervalSetting: see reference of T_DjiCameraPhotoTimeIntervalSettings.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetPhotoTimeIntervalSettings(E_DjiMountPosition position,
                                                              T_DjiCameraPhotoTimeIntervalSettings intervalSetting);

/**
 * @brief Get the parameters in the INTERVAL take-photo mode.
 * @param position: camera mounted position
 * @param intervalSetting: see reference of T_DjiCameraPhotoTimeIntervalSettings.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetPhotoTimeIntervalSettings(E_DjiMountPosition position,
                                                              T_DjiCameraPhotoTimeIntervalSettings *intervalSetting);

/**
 * @brief Set camera focus mode of the selected camera mounted position.
 * @note Set the lens focus mode. When the focus mode is auto, the target
 * point is the focal point. When the focus mode is manual, the target point
 * is the zoom out area if the focus assistant is enabled for the manual
 * mode.
 * @param position: camera mounted position
 * @param focusMode: see reference of E_DjiCameraManagerFocusMode.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetFocusMode(E_DjiMountPosition position,
                                              E_DjiCameraManagerFocusMode focusMode);
/**
 * @brief Get camera focus mode of the selected camera mounted position.
 * @param position: camera mounted position
 * @param focusMode: see reference of E_DjiCameraManagerFocusMode.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetFocusMode(E_DjiMountPosition position,
                                              E_DjiCameraManagerFocusMode *focusMode);

/**
 * @brief Set amera focus point of the selected camera mounted position.
 * @note  Sets the lens focus target point. When the focus mode is auto, the
 * target point is the focal point. When the focus mode is manual, the target
 * point is the zoom out area if the focus assistant is enabled for the manual
 * mode.
 * @param position: camera mounted position
 * @param focusPosData: see reference of T_DjiCameraManagerFocusPosData.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetFocusTarget(E_DjiMountPosition position,
                                                T_DjiCameraManagerFocusPosData focusPosData);

/**
 * @brief Get amera focus point of the selected camera mounted position.
 * @param position: camera mounted position
 * @param focusPosData: see reference of T_DjiCameraManagerFocusPosData.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetFocusTarget(E_DjiMountPosition position,
                                                T_DjiCameraManagerFocusPosData *tapFocusPos);

/**
 * @brief Start camera optical zooming of the selected camera mounted position.
 * @note Start changing the focal length of the lens in specified direction
 * with specified speed. Focal length change (zooming) will halt when maximum
 * or minimum focal lengths are reached, or DjiCameraManager_StopContinuousOpticalZoom*
 * is called.
 * @param position: camera mounted position
 * @param zoomDirection: optical zoom direction, see reference of E_DjiCameraZoomDirection.
 * @param zoomSpeed: optical zoom direction, see reference of E_DjiCameraZoomSpeed.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_StartContinuousOpticalZoom(E_DjiMountPosition position,
                                                            E_DjiCameraZoomDirection zoomDirection,
                                                            E_DjiCameraZoomSpeed zoomSpeed);

/**
 * @brief Stop camera optical zooming of the selected camera mounted position.
 * @note Called to stop focal length changing, when it currently is from
 * calling DjiCameraManager_StartContinuousOpticalZoom*.
 * @param position: camera mounted position
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_StopContinuousOpticalZoom(E_DjiMountPosition position);

/**
 * @brief Set parameters for camera optical zooming of the selected camera mounted position.
 * @note In this interface, the zoom will set the zoom factor as the your
 * target value.
 * @param position: camera mounted position
 * @param zoomDirection: optical zoom direction, see reference of E_DjiCameraZoomDirection.
 * @param factor: target zoom factor.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetOpticalZoomParam(E_DjiMountPosition position,
                                                     E_DjiCameraZoomDirection zoomDirection,
                                                     dji_f32_t factor);

/**
 * @brief Get parameters for camera optical zooming of the selected camera mounted position.
 * @param position: camera mounted position
 * @param opticalZoomParam: see reference of T_DjiCameraManagerOpticalZoomParam.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetOpticalZoomParam(E_DjiMountPosition position,
                                                     T_DjiCameraManagerOpticalZoomParam *opticalZoomParam);

/**
 * @brief Set parameters for camera infrared zooming of the selected camera mounted position.
 * @param position: camera mounted position
 * @param factor: target zoom factor.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetInfraredZoomParam(E_DjiMountPosition position,
                                                      dji_f32_t factor);

/**
 * @brief Stop camera optical zooming of the selected camera mounted position.
 * @note Called to stop focal length changing, when it currently is from
 * calling DjiCameraManager_StartContinuousOpticalZoom*.
 * @param position: camera mounted position
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_StopContinuousOpticalZoom(E_DjiMountPosition position);

/**
 * @brief Enable/Disable camera's tap-zoom function of the selected camera mounted position.
 * @note TapZoomAtTarget can only be called when tap-zoom is enabled.
 * @param position: camera mounted position
 * @param param: enable/disable
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetTapZoomEnabled(E_DjiMountPosition position,
                                                   bool param);

/**
 * @brief Get status of camera's tap-zoom function of the selected camera mounted position.
 * @param position: camera mounted position
 * @param param: enable/disable
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetTapZoomEnabled(E_DjiMountPosition position, bool *param);

/**
 * @brief Set camera's tap-zoom multiplier of the selected camera mounted position.
 * @note Tap-zoom uses a multiplier to change the zoom scale when called. The
 * inal zoom scale for a TapZoom will be: Current Zoom Scale x Multiplier.
 * @param position: camera mounted position
 * @param tapZoomMultiplier: The multiplier range is [1,5]. A multiplier of 1 will not change the zoom.
 * hen the multiplier is 1, the zoom scale will not change during TapZoom.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetTapZoomMultiplier(E_DjiMountPosition position, uint8_t tapZoomMultiplier);

/**
 * @brief Get camera's tap-zoom multiplier of the selected camera mounted position.
 * @param position: camera mounted position
 * @param tapZoomMultiplier: The multiplier range is [1,5]. A multiplier of 1 will not change the zoom.
 * When the multiplier is 1, the zoom scale will not change during TapZoom.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetTapZoomMultiplier(E_DjiMountPosition position, uint8_t *tapZoomMultiplier);

/**
 * @brief Set camera's tap-zoom point of the selected camera mounted position.
 * @note Tap-zoom at the target. It can be called only when TapZoom is
 * enabled. When a new target is set, the gimbal will rotate and locate the
 * target in the center of the screen. At the same time, the camera will zoom
 * by multiplying the TapZoom multiplier
 * @param position: camera mounted position
 * @param tapZoomPos: see reference of T_DjiCameraManagerTapZoomPosData.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_TapZoomAtTarget(E_DjiMountPosition position,
                                                 T_DjiCameraManagerTapZoomPosData tapZoomPos);

/**
 * @brief Set camera's exposure mode of the selected camera mounted position.
 * @note  The different exposure modes define whether aperture, shutter speed,
 * ISO can be set automatically or manually. Exposure compensation can be
 * changed in all modes except manual mode where it is not settable.
 * @param position: camera mounted position
 * @param mode: see reference of E_DjiCameraManagerExposureMode.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetExposureMode(E_DjiMountPosition position,
                                                 E_DjiCameraManagerExposureMode mode);

/**
 * @brief Get camera's exposure mode of the selected camera mounted position.
 * @note  The different exposure modes define whether aperture, shutter speed,
 * ISO can be set automatically or manually. Exposure compensation can be
 * changed in all modes except manual mode where it is not settable.
 * @param position: camera mounted position
 * @param mode: see reference of E_DjiCameraManagerExposureMode.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetExposureMode(E_DjiMountPosition position,
                                                 E_DjiCameraManagerExposureMode *mode);

/**
 * @brief Set camera's iso value of the selected camera mounted position.
 * @note  ISO value can only be set when the camera exposure mode is in
 * manual mode.
 * @param position: camera mounted position
 * @param iso: see reference of E_DjiCameraManagerISO.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetISO(E_DjiMountPosition position,
                                        E_DjiCameraManagerISO iso);

/**
 * @brief Get camera's iso value of the selected camera mounted position.
 * @param position: camera mounted position
 * @param iso: see reference of E_DjiCameraManagerISO.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetISO(E_DjiMountPosition position,
                                        E_DjiCameraManagerISO *iso);

/**
 * @brief Set camera's aperture size value of the selected camera mounted position.
 * @note The exposure mode must be in DJI_CAMERA_MANAGER_EXPOSURE_MODE_EXPOSURE_MANUAL or
 * DJI_CAMERA_MANAGER_EXPOSURE_MODE_APERTURE_PRIORITY.
 * @param position: camera mounted position
 * @param aperture: see reference of E_DjiCameraManagerAperture.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetAperture(E_DjiMountPosition position,
                                             E_DjiCameraManagerAperture aperture);

/**
 * @brief Get camera's aperture size value of the selected camera mounted position.
 * @param position: camera mounted position
 * @param aperture: see reference of E_DjiCameraManagerAperture.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetAperture(E_DjiMountPosition position,
                                             E_DjiCameraManagerAperture *aperture);

/**
 * @brief Set camera's shutter value of the selected camera mounted position.
 * @note Set the camera shutter speed. The shutter speed should not be set
 * slower than the video frame rate when the camera's mode is RECORD_VIDEO.
 * For example, if the video frame rate is 30fps, the shutterSpeed must be <=
 * 1/30. Precondition: The shutter speed can be set only when the camera
 * exposure mode is DJI_CAMERA_MANAGER_EXPOSURE_MODE_EXPOSURE_MANUAL mode or
 * DJI_CAMERA_MANAGER_EXPOSURE_MODE_SHUTTER_PRIORITY
 * @param position: camera mounted position
 * @param shutterSpeed: see reference of E_DjiCameraManagerShutterSpeed.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetShutterSpeed(E_DjiMountPosition position,
                                                 E_DjiCameraManagerShutterSpeed shutterSpeed);

/**
 * @brief Get camera's shutter value of the selected camera mounted position.
 * @param position: camera mounted position
 * @param shutterSpeed: see reference of E_DjiCameraManagerShutterSpeed.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetShutterSpeed(E_DjiMountPosition position,
                                                 E_DjiCameraManagerShutterSpeed *shutterSpeed);

/**
 * @brief Set camera's EV value of the selected camera mounted position.
 * @note Set the camera's exposure compensation. In order to use this
 * function, set the camera exposure mode to shutter, program or aperture.
 * exposure mode is DJI_CAMERA_MANAGER_EXPOSURE_MODE_EXPOSURE_MANUAL mode or
 * DJI_CAMERA_MANAGER_EXPOSURE_MODE_SHUTTER_PRIORITY or
 * DJI_CAMERA_MANAGER_EXPOSURE_APERTURE_PRIORITY
 * @param position: camera mounted position
 * @param ev: see reference of E_DjiCameraManagerExposureCompensation.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetExposureCompensation(E_DjiMountPosition position,
                                                         E_DjiCameraManagerExposureCompensation ev);

/**
 * @brief Get camera's EV value of the selected camera mounted position.
 * @param position: camera mounted position
 * @param ev: see reference of E_DjiCameraManagerExposureCompensation.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetExposureCompensation(E_DjiMountPosition position,
                                                         E_DjiCameraManagerExposureCompensation *ev);

/**
 * @brief Start to take video of the selected camera mounted position.
 * @note Camera must be in RECORD_VIDEO mode. For thermal imaging camera,
 * user can take Single photo when recording video.
 * @param position: camera mounted position
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_StartRecordVideo(E_DjiMountPosition position);

/**
 * @brief Stop to take video of the selected camera mounted position.
 * @note Precondition: The camera is recording currently.
 * @param position: camera mounted position
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_StopRecordVideo(E_DjiMountPosition position);

/**
 * @brief Download selected camera media file list.
 * @note The interface is a synchronous interface, which occupies more CPU resources when using it.
 * If the download file fails, the timeout time is 3S.
 * @param position: the mount position of the camera
 * @param fileList: the pointer to the downloaded camera file list
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_DownloadFileList(E_DjiMountPosition position, T_DjiCameraManagerFileList *fileList);

/**
 * @brief Download selected camera media file list by slices.
 * @note The interface is a synchronous interface, which occupies more CPU resources when using it.
 * If the download file fails, the timeout time is 3S.
 * @param position: the mount position of the camera
 * @param sliceConfig: the slices config for downloading file list
 * @param fileList: the pointer to the downloaded camera file list
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_DownloadFileListBySlices(E_DjiMountPosition position,
                                                          T_DjiCameraManagerSliceConfig sliceConfig,
                                                          T_DjiCameraManagerFileList *fileList);
/**
 * @brief Regsiter selected camera download file data callback,
 * @param position: the mount position of the camera
 * @param callback: the download file data callback
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_RegDownloadFileDataCallback(E_DjiMountPosition position,
                                                             DjiCameraManagerDownloadFileDataCallback callback);

/**
 * @brief Download selected camera media file by file index.
 * @note Only support download one file at the same time, the new file download need wait for the previous file
 * download finished.The interface is a synchronous interface, which occupies more CPU resources when using it.
 * If the download file fails, the timeout time is 3S.
 * @param position: the mount position of the camera
 * @param fileIndex: the index of the camera media file
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_DownloadFileByIndex(E_DjiMountPosition position, uint32_t fileIndex);

/**
 * @brief Delete selected camera media file by file index.
 * @param position: the mount position of the camera
 * @param fileIndex: the index of the camera media file
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_DeleteFileByIndex(E_DjiMountPosition position, uint32_t fileIndex);

/**
 * @brief Get the camera laser ranging info of the selected camera mounted position.
 * @note Maximum data update frequency: 5Hz.
 * @param position: the mount position of the camera
 * @param laserRangingInfo: the pointer to the camera laser ranging info
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetLaserRangingInfo(E_DjiMountPosition position,
                                                     T_DjiCameraManagerLaserRangingInfo *laserRangingInfo);

/**
 * @brief Set point thermometry coordinates of the selected camera mounted position.
 * @param position: camera mounted position
 * @param pointCoordinate: point thermometry coordinates
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetPointThermometryCoordinate(E_DjiMountPosition position,
                                                               T_DjiCameraManagerPointThermometryCoordinate pointCoordinate);

/**
 * @brief Get point thermometry result.
 * @note Before get point thermometry data from camera, DjiCameraManager_SetPointThermometryCoordinate()
 * function has to be called.
 * @param position: camera mounted position
 * @param pointThermometryData: point thermometry result
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetPointThermometryData(E_DjiMountPosition position,
                                                         T_DjiCameraManagerPointThermometryData *pointThermometryData);

/**
 * @brief Set area thermometry coordinates of the selected camera mounted position.
 * @param position: camera mounted position
 * @param areaCoordinate: area thermometry coordinates
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetAreaThermometryCoordinate(E_DjiMountPosition position,
                                                              T_DjiCameraManagerAreaThermometryCoordinate areaCoordinate);

/**
 * @brief Get area thermometry result.
 * @note Before get area thermometry data from camera, DjiCameraManager_SetAreaThermometryCoordinate()
 * function has to be called.
 * @param position: camera mounted position
 * @param areaThermometryData: area thermometry result
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetAreaThermometryData(E_DjiMountPosition position,
                                                        T_DjiCameraManagerAreaThermometryData *areaThermometryData);

#ifdef __cplusplus
}
#endif

#endif // DJI_CAMERA_MANAGER_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
