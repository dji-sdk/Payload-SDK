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

/*! @brief CameraModule focus mode. If the physical AF switch on the camera is
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

/*! @brief CameraModule shutter mode.
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
    /*! The ISO value is set to 200. */
    DJI_CAMERA_MANAGER_ISO_200 = 0x04,
    /*! The ISO value is set to 400.*/
    DJI_CAMERA_MANAGER_ISO_400 = 0x05,
    /*! The ISO value is set to 800.*/
    DJI_CAMERA_MANAGER_ISO_800 = 0x06,
    /*! The ISO value is set to 1600.*/
    DJI_CAMERA_MANAGER_ISO_1600 = 0x07,
    /*! The ISO value is set to 3200.*/
    DJI_CAMERA_MANAGER_ISO_3200 = 0x08,
    /*! The ISO value is set to 6400.*/
    DJI_CAMERA_MANAGER_ISO_6400 = 0x09,
    /*! The ISO value is set to 12800.*/
    DJI_CAMERA_MANAGER_ISO_12800 = 0x0A,
    /*! The ISO value is set to 25600.*/
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

typedef enum {
    DJI_CAMERA_MANAGER_SOURCE_DEFAULT_CAM = 0x0,
    DJI_CAMERA_MANAGER_SOURCE_WIDE_CAM = 0x1,
    DJI_CAMERA_MANAGER_SOURCE_ZOOM_CAM = 0x2,
    DJI_CAMERA_MANAGER_SOURCE_IR_CAM = 0x3,
    DJI_CAMERA_MANAGER_SOURCE_VISIBLE_CAM = 0x7,
} E_DjiCameraManagerStreamSource, E_DjiCameraManagerStreamStorage;

typedef enum {
    DJI_CAMERA_MANAGER_NIGHT_SCENE_MODE_DISABLE = 0,
    DJI_CAMERA_MANAGER_NIGHT_SCENE_MODE_ENABLE = 1,
    DJI_CAMERA_MANAGER_NIGHT_SCENE_MODE_AUTO = 2,
} E_DjiCameraManagerNightSceneMode;

typedef enum {
    DJI_CAMERA_MANAGER_CAPTURE_OR_RECORDING_CAPTURE = 0,
    DJI_CAMERA_MANAGER_CAPTURE_OR_RECORDING_RECORDING = 1,
} E_DjiCameraManagerCaptureOrRecording;

typedef enum {
    DJI_CAMERA_MANAGER_EXPAND_NAME_TYPE_FILE = 1,
    DJI_CAMERA_MANAGER_EXPAND_NAME_TYPE_DIR = 2,
} E_DjiCameraManagerExpandNameType;

typedef enum {
    DJI_CAMERA_MANAGER_PHOTO_RATIO_4X3 = 0,
    DJI_CAMERA_MANAGER_PHOTO_RATIO_16X9 = 1,
    DJI_CAMERA_MANAGER_PHOTO_RATIO_3X2 = 2,
    DJI_CAMERA_MANAGER_PHOTO_RATIO_1X1 = 3,
    DJI_CAMERA_MANAGER_PHOTO_RATIO_18X3 = 4,
    DJI_CAMERA_MANAGER_PHOTO_RATIO_5X4 = 5,
} E_DjiCameraManagerPhotoRatio;

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
    char fileName[DJI_FILE_NAME_SIZE_MAX];
    uint32_t fileSize;
    uint32_t fileIndex;
    T_DjiCameraManagerFileCreateTime createTime;
    E_DjiCameraMediaFileSubType type;
    T_DjiCameraManagerFileAttributeData attributeData;
} T_DjiCameraManagerSubFileListInfo;

typedef struct {
    char fileName[DJI_FILE_NAME_SIZE_MAX];
    uint32_t fileSize;
    uint32_t fileIndex;
    T_DjiCameraManagerFileCreateTime createTime;
    E_DjiCameraMediaFileType type;
    T_DjiCameraManagerFileAttributeData attributeData;
    uint8_t subFileListTotalNum;
    T_DjiCameraManagerSubFileListInfo* subFileListInfo;
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
    DJI_DOWNLOAD_FILE_EVENT_START_TRANSFER_END,
} E_DjiDownloadFileEvent;

typedef enum {
    DJI_CAMERA_MANAGER_VIDEO_RESOLUTION_640X480P = 0, // 640X480P
    DJI_CAMERA_MANAGER_VIDEO_RESOLUTION_1280X640P = 2, // 1280X640P
    DJI_CAMERA_MANAGER_VIDEO_RESOLUTION_1280X720P = 4, // 1280X720P
    DJI_CAMERA_MANAGER_VIDEO_RESOLUTION_1920X1080P = 10, // 1920X1080P
    DJI_CAMERA_MANAGER_VIDEO_RESOLUTION_3840X2160P = 16, // 3840X2160P
} E_DjiCameraManagerVideoResolution;

typedef enum {
    DJI_CAMERA_MANAGER_VIDEO_FRAME_RATE_15FPS = 0, // 14.985
    DJI_CAMERA_MANAGER_VIDEO_FRAME_RATE_25FPS = 2, // 25.000
    DJI_CAMERA_MANAGER_VIDEO_FRAME_RATE_30FPS = 3, // 29.970
    DJI_CAMERA_MANAGER_VIDEO_FRAME_RATE_60FPS = 6, // 59.940
} E_DjiCameraManagerVideoFrameRate;

typedef enum {
    DJI_CAMERA_MANAGER_PHOTO_STORAGE_FORMAT_RAW = 0,
    DJI_CAMERA_MANAGER_PHOTO_STORAGE_FORMAT_JPEG = 1,
    DJI_CAMERA_MANAGER_PHOTO_STORAGE_FORMAT_RAW_JPEG = 2,
    DJI_CAMERA_MANAGER_PHOTO_STORAGE_FORMAT_YUV = 3, // Save as YUV format image
    DJI_CAMERA_MANAGER_PHOTO_STORAGE_FORMAT_RJPEG = 7, // Radiometric JPEG
} E_DjiCameraManagerPhotoStorageFormat;

typedef enum {
    DJI_CAMERA_MANAGER_VIDEO_STORAGE_FORMAT_MOV = 0,
    DJI_CAMERA_MANAGER_VIDEO_STORAGE_FORMAT_MP4 = 1,
} E_DjiCameraManagerVideoStorageFormat;

typedef enum {
    DJI_CAMERA_MANAGER_METERING_MODE_CENTRAL = 0,
    DJI_CAMERA_MANAGER_METERING_MODE_AVERAGE = 1,
    DJI_CAMERA_MANAGER_METERING_MODE_SPOT = 2,
} E_DjiCameraManagerMeteringMode;

typedef enum {
    DJI_CAMERA_MANAGER_FFC_MODE_MANUAL = 0,
    DJI_CAMERA_MANAGER_FFC_MODE_AUTO = 1,
} E_DjiCameraManagerFfcMode;

typedef enum {
    DJI_CAMERA_MANAGER_IR_GAIN_MODE_AUTO = 0,
    DJI_CAMERA_MANAGER_IR_GAIN_MODE_LOW = 1,
    DJI_CAMERA_MANAGER_IR_GAIN_MODE_HIGH = 2,
} E_DjiCameraManagerIrGainMode;

typedef enum {
    /* Camera is not capturing photos*/
    DJI_CAMERA_MANAGER_CAPTURING_STATE_IDLE = 0,

    /* Camera is capturing a single photo */
    DJI_CAMERA_MANAGER_CAPTURING_STATE_SINGLE = 1,

    /* Camera is capturing multiple photos */
    DJI_CAMERA_MANAGER_CAPTURING_STATE_MULTI = 2,
} E_DjiCameraManagerCapturingState;

typedef enum {
    DJI_CAMERA_MANAGER_RECORDING_STATE_IDLE = 0,
    DJI_CAMERA_MANAGER_RECORDING_STATE_STARTING = 1,
    DJI_CAMERA_MANAGER_RECORDING_STATE_RECORDING = 2,
    DJI_CAMERA_MANAGER_RECORDING_STATE_STOPPING = 3,
} E_DjiCameraManagerRecordingState;

/*! @brief: when the remote control is in split-screen mode, the coordinate range of the x-axis is 0-0.5.
 */
typedef struct {
    dji_f32_t pointX;               /*! x-coordinate of point thermometry, range: 0-1 */
    dji_f32_t pointY;               /*! y-coordinate of point thermometry, range: 0-1 */
} T_DjiCameraManagerPointThermometryCoordinate;

typedef struct {
    dji_f32_t areaTempLtX;          /*! x-coordinate of the upper left corner of the area thermometry, range: 0-1 */
    dji_f32_t areaTempLtY;          /*! y-coordinate of the upper left corner of the area thermometry, range: 0-1 */
    dji_f32_t areaTempRbX;          /*! x-coordinate of the lower right corner of the area thermometry, range: 0-1 */
    dji_f32_t areaTempRbY;          /*! y-coordinate of the lower right corner of the area thermometry, range: 0-1 */
} T_DjiCameraManagerAreaThermometryCoordinate;

//result of point thermometry
typedef struct {
    dji_f32_t pointX;              /*! x-coordinate of point thermometry, range: 0-1 */
    dji_f32_t pointY;              /*! y-coordinate of point thermometry, range: 0-1 */
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
    uint8_t fileType;
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

typedef struct {
    uint32_t size;
    E_DjiCameraManagerStreamSource streamSource[4];
    E_DjiCameraManagerStreamStorage streamStorage[4];
} T_DjiCameraManagerStreamList;

typedef struct {
    E_DjiCameraManagerVideoResolution videoResolution;
    E_DjiCameraManagerVideoFrameRate videoFrameRate;
} T_DjiCameraManagerVideoFormat;

typedef struct {
    uint8_t size;
    union {
        E_DjiCameraManagerPhotoStorageFormat photoStorageFormat[16];
        E_DjiCameraManagerVideoStorageFormat videoStorageFormat[16];
        E_DjiCameraManagerPhotoRatio photoRatioFormat[16];
        E_DjiCameraManagerStreamSource streamSource[16];
        E_DjiCameraManagerStreamStorage streamStorage[16];
        E_DjiCameraManagerNightSceneMode nightSceneMode[16];
        E_DjiCameraManagerMeteringMode meteringMode[16];
    };
    uint32_t minValue;
    uint32_t maxValue;
} T_DjiCameraManagerRangeList;

typedef struct {
    double lowGainTempMin;
    double lowGainTempMax;
    double highGainTempMin;
    double highGainTempMax;
} T_DjiCameraManagerIrTempMeterRange;

typedef struct {
    uint32_t totalCapacity;     /* MByte */
    uint32_t remainCapacity;    /* MByte */
} T_DjiCameraManagerStorageInfo;

typedef struct {
    uint32_t flag; /* 0xFFFFFFFF */
    uint32_t seqNum;
    uint64_t timestamp;
    uint32_t dataByte; /* actual num of bytes used for points */
} __attribute__((packed)) T_DjiCameraManagerPointCloudHeader;

typedef struct {
    dji_f32_t x; /* the x-axis of NED coordinate system */
    dji_f32_t y; /* the y-axis of NED coordinate system */
    dji_f32_t z; /* the z-axis of NED coordinate system */
    uint8_t intensity;
    uint8_t r;
    uint8_t g;
    uint8_t b;
}__attribute__((packed)) T_DjiCameraManagerPointXYZRGBInfo;

typedef struct {
    T_DjiCameraManagerPointCloudHeader pointCloudHeader;
    uint32_t crc_header;
    uint32_t crc_rest;
    T_DjiCameraManagerPointXYZRGBInfo points[1];
}__attribute__((packed)) T_DjiCameraManagerColorPointCloud;

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
 * @param cameraType: refer to E_DjiCameraType.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetCameraType(E_DjiMountPosition position, E_DjiCameraType *cameraType);

/**
 * @brief Get camera firmware version of the selected camera mounted position.
 * @param position: camera mounted position
 * @param firmwareVersion: refer to T_DjiCameraManagerFirmwareVersion.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetFirmwareVersion(E_DjiMountPosition position,
                                                    T_DjiCameraManagerFirmwareVersion *firmwareVersion);

/**
 * @brief Get camera connection status.
 * @param position: camera mounted position
 * @param connectStatus: returned value of connection status
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetCameraConnectStatus(E_DjiMountPosition position,
                                                        bool *connectStatus);

/**
 * @brief Set camera working mode of the selected camera mounted position.
 * @note Set the camera's work mode to options such as taking pictures, recording video,
 * playback, or downloading. Please note that you cannot change the mode when a certain
 * task is executing. This action takes about 1-2 s.
 * @param position: camera mounted position
 * @param workMode: refer to E_DjiCameraManagerWorkMode.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetMode(E_DjiMountPosition position,
                                         E_DjiCameraManagerWorkMode workMode);

/**
 * @brief Get camera working mode of the selected camera mounted position.
 * @param position: camera mounted position
 * @param workMode: refer to E_DjiCameraManagerWorkMode.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetMode(E_DjiMountPosition position,
                                         E_DjiCameraManagerWorkMode *workMode);

/**
* @brief Set camera shoot mode of the selected camera mounted position.
* @param position: camera mounted position
* @param mode: refer to E_DjiCameraManagerShootPhotoMode.
* @return Execution result.
*/
T_DjiReturnCode DjiCameraManager_SetShootPhotoMode(E_DjiMountPosition position,
                                                   E_DjiCameraManagerShootPhotoMode mode);

/**
* @brief Get camera shoot mode of the selected camera mounted position.
* @param position: camera mounted position
* @param mode: refer to E_DjiCameraManagerShootPhotoMode.
* @return Execution result.
*/
T_DjiReturnCode DjiCameraManager_GetShootPhotoMode(E_DjiMountPosition position,
                                                   E_DjiCameraManagerShootPhotoMode *takePhotoMode);

/**
 * @brief Start shooting photo.
 * @note Camera must be in ShootPhoto mode. For thermal imaging cameras,
 * it is allowed to take a single photo while recording video. Check the SD
 * card's capacity before using this method to ensure there is enough space.
 * @param position: camera mounted position
 * @param mode: refer to E_DjiCameraManagerShootPhotoMode.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_StartShootPhoto(E_DjiMountPosition position,
                                                 E_DjiCameraManagerShootPhotoMode mode);

/**
 * @brief Stop shooting photo.
 * @note Camera must be in ShootPhoto mode and the shoot mode is either
 * Interval or Time-lapse. If set to single shot mode, the camera will
 * automatically stop after taking the photo.
 * @param position: camera mounted position
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_StopShootPhoto(E_DjiMountPosition position);

/**
 * @brief Get camera capturing state.
 * @note This API is not supported by L1/P1/M3D/M3TD models.
 * @param position: camera mounted position
 * @param capturingState: result of getting, see E_DjiCameraManagerCapturingState.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetCapturingState(E_DjiMountPosition position,
                                                   E_DjiCameraManagerCapturingState *capturingState);

/**
 * @brief Set the burst count for burst shooting mode.
 * @param position: camera mounted position
 * @param count: refer to E_DjiCameraBurstCount.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetPhotoBurstCount(E_DjiMountPosition position,
                                                    E_DjiCameraBurstCount count);

/**
 * @brief Set the parameters for INTERVAL shooting mode.
 * @note In this mode, the camera captures a photo, waits a specified interval
 * of time, then captures another photo, continuing until the set number of
 * photos is reached. Supported by thermal imaging cameras, too.
 * @param position: camera mounted position
 * @param intervalSetting: refer to T_DjiCameraPhotoTimeIntervalSettings.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetPhotoTimeIntervalSettings(E_DjiMountPosition position,
                                                              T_DjiCameraPhotoTimeIntervalSettings intervalSetting);

/**
 * @brief Get the parameters for INTERVAL shooting mode.
 * @param position: camera mounted position
 * @param intervalSetting: refer to T_DjiCameraPhotoTimeIntervalSettings.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetPhotoTimeIntervalSettings(E_DjiMountPosition position,
                                                              T_DjiCameraPhotoTimeIntervalSettings *intervalSetting);

/**
 * @brief Get the remaining time of interval shooting.
 * @note Not supported by L1/P1/M3D/M3TD models.
 * @param position: camera mounted position
 * @param remainTime: time in seconds.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetIntervalShootingRemainTime(E_DjiMountPosition position,
                                                               uint8_t *remainTime);

/**
 * @brief Set camera focus mode of the selected camera mounted position.
 * @note Set the lens focus mode. In auto focus mode, the target
 * point is the focal point. In manual focus mode, if focus assist is
 * enabled, it adjusts focus in the zoomed-out area.
 * @param position: camera mounted position
 * @param focusMode: refer to E_DjiCameraManagerFocusMode.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetFocusMode(E_DjiMountPosition position,
                                              E_DjiCameraManagerFocusMode focusMode);
/**
 * @brief Get camera focus mode of the selected camera mounted position.
 * @param position: camera mounted position
 * @param focusMode: refer to E_DjiCameraManagerFocusMode.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetFocusMode(E_DjiMountPosition position,
                                              E_DjiCameraManagerFocusMode *focusMode);

/**
 * @brief Set camera focus point of the selected camera mounted position.
 * @note  Sets the target point for focusing. In auto mode, this is the focal
 * point. In manual mode with focus assist enabled, it's the zoomed-out area.
 * @param position: camera mounted position
 * @param focusPosData: refer to T_DjiCameraManagerFocusPosData.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetFocusTarget(E_DjiMountPosition position,
                                                T_DjiCameraManagerFocusPosData focusPosData);

/**
 * @brief Get camera focus point of the selected camera mounted position.
 * @param position: camera mounted position
 * @param focusPosData: refer to T_DjiCameraManagerFocusPosData.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetFocusTarget(E_DjiMountPosition position,
                                                T_DjiCameraManagerFocusPosData *tapFocusPos);

/**
 * @brief Start camera optical zooming of the selected camera mounted position.
 * @note Changes the lens's focal length in the specified direction at a specified
 * speed. Zooming stops at the lens's max or min focal length or when
 * StopContinuousOpticalZoom is called.
 * @param position: camera mounted position
 * @param zoomDirection: optical zoom direction, refer to E_DjiCameraZoomDirection.
 * @param zoomSpeed: optical zoom direction, refer to E_DjiCameraZoomSpeed.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_StartContinuousOpticalZoom(E_DjiMountPosition position,
                                                            E_DjiCameraZoomDirection zoomDirection,
                                                            E_DjiCameraZoomSpeed zoomSpeed);

/**
 * @brief Stop the ongoing optical zoom operation of the selected camera mounted position.
 * @note Should be called to halt the focal length change initiated by
 * DjiCameraManager_StartContinuousOpticalZoom.
 * @param position: camera mounted position
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_StopContinuousOpticalZoom(E_DjiMountPosition position);

/**
 * @brief Set target zoom factor for optical zooming of the selected camera mounted position.
 * @note This interface sets the zoom to the specified target value.
 * @param position: camera mounted position
 * @param zoomDirection: optical zoom direction, refer to E_DjiCameraZoomDirection.
 * @param factor: target zoom factor.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetOpticalZoomParam(E_DjiMountPosition position,
                                                     E_DjiCameraZoomDirection zoomDirection,
                                                     dji_f32_t factor);

/**
 * @brief Get parameters for camera optical zooming of the selected camera mounted position.
 * @param position: camera mounted position
 * @param opticalZoomParam: refer to T_DjiCameraManagerOpticalZoomParam.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetOpticalZoomParam(E_DjiMountPosition position,
                                                     T_DjiCameraManagerOpticalZoomParam *opticalZoomParam);

/**
 * @brief  Set target zoom factor for infrared zooming of the selected camera mounted position.
 * @param position: camera mounted position
 * @param factor: target zoom factor.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetInfraredZoomParam(E_DjiMountPosition position,
                                                      dji_f32_t factor);

/**
 * @brief Enable/Disable the tap-zoom function for the selected camera mounted position.
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
 * @note The final zoom scale during a tap-zoom action will be:
 * Current Zoom Scale x Multiplier.
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
 * @note Only available when tap-zoom is enabled. Sets a new target,
 * reorienting the gimbal to locate the target on the screen center and
 * applying the tap-zoom multiplier.
 * @param position: camera mounted position
 * @param tapZoomPos: refer to T_DjiCameraManagerTapZoomPosData.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_TapZoomAtTarget(E_DjiMountPosition position,
                                                 T_DjiCameraManagerTapZoomPosData tapZoomPos);

/**
 * @brief Get camera focus ring value range.
 * @param position: camera mounted position
 * @param rangeList: returned value of range.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetFocusRingRange(E_DjiMountPosition position,
                                                   T_DjiCameraManagerRangeList *rangeList);

/**
 * @brief Set camera focus ring value.
 * @param position: camera mounted position
 * @param value: focus ring value.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetFocusRingValue(E_DjiMountPosition position,
                                                   uint16_t value);

/**
 * @brief Get camera focus ring value.
 * @param position: camera mounted position
 * @param value: focus ring value to be returned.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetFocusRingValue(E_DjiMountPosition position,
                                                   uint16_t *value);

/**
 * @brief Set camera's exposure mode of the selected camera mounted position.
 * @note Different exposure modes define whether settings like aperture, shutter
 * speed, and ISO are set automatically or manually. Exposure compensation is
 * adjustable in all modes except the manual mode.
 * @param position: camera mounted position
 * @param mode: refer to E_DjiCameraManagerExposureMode.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetExposureMode(E_DjiMountPosition position,
                                                 E_DjiCameraManagerExposureMode mode);

/**
 * @brief Get camera's exposure mode of the selected camera mounted position.
 * @note Different exposure modes define whether settings like aperture, shutter
 * speed, and ISO are set automatically or manually. Exposure compensation is
 * adjustable in all modes except the manual mode.
 * @param position: camera mounted position
 * @param mode: refer to E_DjiCameraManagerExposureMode.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetExposureMode(E_DjiMountPosition position,
                                                 E_DjiCameraManagerExposureMode *mode);

/**
 * @brief Set camera's iso value of the selected camera mounted position.
 * @note  ISO value can only be set when the camera exposure mode is in
 * manual mode.
 * @param position: camera mounted position
 * @param iso: refer to E_DjiCameraManagerISO.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetISO(E_DjiMountPosition position,
                                        E_DjiCameraManagerISO iso);

/**
 * @brief Get camera's iso value of the selected camera mounted position.
 * @param position: camera mounted position
 * @param iso: refer to E_DjiCameraManagerISO.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetISO(E_DjiMountPosition position,
                                        E_DjiCameraManagerISO *iso);

/**
 * @brief Set camera's aperture size value of the selected camera mounted position.
 * @note The exposure mode must be on DJI_CAMERA_MANAGER_EXPOSURE_MODE_EXPOSURE_MANUAL or
 * DJI_CAMERA_MANAGER_EXPOSURE_MODE_APERTURE_PRIORITY.
 * @param position: camera mounted position
 * @param aperture: refer to E_DjiCameraManagerAperture.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetAperture(E_DjiMountPosition position,
                                             E_DjiCameraManagerAperture aperture);

/**
 * @brief Get camera's aperture size value of the selected camera mounted position.
 * @param position: camera mounted position
 * @param aperture: refer to E_DjiCameraManagerAperture.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetAperture(E_DjiMountPosition position,
                                             E_DjiCameraManagerAperture *aperture);

/**
 * @brief Set camera's shutter speed value of the selected camera mounted position.
 * @note Set the camera shutter speed. Ensure the shutter speed is not set
 * slower than the video frame rate when the camera's mode is RECORD_VIDEO.
 * For example, if the video frame rate is 30fps, the shutterSpeed must be <=
 * 1/30. Precondition: The shutter speed can be set only when the camera
 * exposure mode is DJI_CAMERA_MANAGER_EXPOSURE_MODE_EXPOSURE_MANUAL mode or
 * DJI_CAMERA_MANAGER_EXPOSURE_MODE_SHUTTER_PRIORITY
 * @param position: camera mounted position
 * @param shutterSpeed: refer to E_DjiCameraManagerShutterSpeed.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetShutterSpeed(E_DjiMountPosition position,
                                                 E_DjiCameraManagerShutterSpeed shutterSpeed);

/**
 * @brief Get camera's shutter value of the selected camera mounted position.
 * @param position: camera mounted position
 * @param shutterSpeed: refer to E_DjiCameraManagerShutterSpeed.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetShutterSpeed(E_DjiMountPosition position,
                                                 E_DjiCameraManagerShutterSpeed *shutterSpeed);

/**
 * @brief Set camera's EV value of the selected camera mounted position.
 * @note This function is available in program, shutter, or aperture
 * exposure modes. Enums are DJI_CAMERA_MANAGER_EXPOSURE_MODE_EXPOSURE_MANUAL,
 * DJI_CAMERA_MANAGER_EXPOSURE_MODE_SHUTTER_PRIORITY, and
 * DJI_CAMERA_MANAGER_EXPOSURE_APERTURE_PRIORITY
 * @param position: camera mounted position
 * @param ev: refer to E_DjiCameraManagerExposureCompensation.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetExposureCompensation(E_DjiMountPosition position,
                                                         E_DjiCameraManagerExposureCompensation ev);

/**
 * @brief Get camera's EV value of the selected camera mounted position.
 * @param position: camera mounted position
 * @param ev: refer to E_DjiCameraManagerExposureCompensation.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetExposureCompensation(E_DjiMountPosition position,
                                                         E_DjiCameraManagerExposureCompensation *ev);

/**
 * @brief Set AE lock mode.
 * @param position: camera mounted position
 * @param enable: true to enable, false to disable AE lock.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetAELockEnabled(E_DjiMountPosition position,
                                                  bool enable);

/**
 * @brief Get AE lock mode.
 * @note This API is not supported by L1/P1/M3D/M3TD models.
 * @param position: camera mounted position
 * @param enable: result of AE lock mode.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetAELockEnabled(E_DjiMountPosition position,
                                                  bool *enable);

/**
 * @brief Reset camera settings.
 * @param position: camera mounted position
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_ResetCameraSettings(E_DjiMountPosition position);

/**
 * @brief Start to take video of the selected camera mounted position.
 * @note Camera must be in RECORD_VIDEO mode. For thermal imaging camera,
 * user can take a single photo when recording video.
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
 * @brief Get camera recording state.
 * @param position: camera mounted position
 * @param recordingState: result of getting, see E_DjiCameraManagerRecordingState.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetRecordingState(E_DjiMountPosition position,
                                                   E_DjiCameraManagerRecordingState *recordingState);

/**
 * @brief Get camera recording time.
 * @note This API is not supported by L1/P1/M3D/M3TD models.
 * @param position: camera mounted position
 * @param recordingTime: result of getting, unit is seconds.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetRecordingTime(E_DjiMountPosition position,
                                                   uint16_t *recordingTime);
/**
 * @brief Get camera stream source range.
 * @param position: camera mounted position
 * @param rangeList: pointer to the result.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetStreamSourceRange(E_DjiMountPosition position,
                                                      T_DjiCameraManagerRangeList *rangeList);

/**
 * @brief Choose camera stream source.
 * @param position: camera mounted position
 * @param streamSource: stream source to be chose.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetStreamSource(E_DjiMountPosition position,
                                                 E_DjiCameraManagerStreamSource streamSource);

/**
 * @brief Get photo storage format range.
 * @param position: camera mounted position
 * @param rangeList: range list returned value
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetPhotoStorageFormatRange(E_DjiMountPosition position,
                                                     T_DjiCameraManagerRangeList *rangeList);

/**
 * @brief Set photo storage format.
 * @param position: camera mounted position
 * @param format: storage format.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetPhotoFormat(E_DjiMountPosition position,
                                                E_DjiCameraManagerPhotoStorageFormat format);

/**
 * @brief Get photo storage format.
 * @param position: camera mounted position
 * @param format: returned value of storage format.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetPhotoFormat(E_DjiMountPosition position,
                                                E_DjiCameraManagerPhotoStorageFormat *format);

/**
 * @brief Get video storage format range.
 * @param position: camera mounted position
 * @param rangeList: range list returned value
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetVideoFormatRange(E_DjiMountPosition position,
                                                     T_DjiCameraManagerRangeList *rangeList);

/**
 * @brief Set video storage format.
 * @param position: camera mounted position
 * @param format: storage format.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetVideoStorageFormat(E_DjiMountPosition position,
                                                E_DjiCameraManagerVideoStorageFormat format);

/**
 * @brief Get video storage format.
 * @param position: camera mounted position
 * @param format: returned value of storage format.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetVideoFormat(E_DjiMountPosition position,
                                                E_DjiCameraManagerVideoStorageFormat *format);

/**
 * @brief Get photo ratio range
 * @param position: camera mounted position
 * @param rangeList: range list returned value
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetPhotoRatioRange(E_DjiMountPosition position,
                                                     T_DjiCameraManagerRangeList *rangeList);

/**
 * @brief Set camera photo ratio
 * @param position: camera mounted position
 * @param photoRatio: ratio to be set
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetPhotoRatio(E_DjiMountPosition position,
                                               E_DjiCameraManagerPhotoRatio photoRatio);

/**
 * @brief Get camera photo ratio
 * @param position: camera mounted position
 * @param photoRatio: returned value of photo ratio
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetPhotoRatio(E_DjiMountPosition position,
                                               E_DjiCameraManagerPhotoRatio *photoRatio);

/**
 * @brief Get camera video resolution and frame rate
 * @param position: camera mounted position
 * @param photoRatio: returned value of video resolution and frame rate
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetVideoResolutionFrameRate(E_DjiMountPosition position,
                                                             T_DjiCameraManagerVideoFormat *videoParam);

/**
 * @brief Get night scene mode range.
 * @param position: camera mounted position.
 * @param tempRange: returned valued of night scene mode range.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetNightSceneModeRange(E_DjiMountPosition position,
                                                        T_DjiCameraManagerRangeList *rangeList);

/**
 * @brief Set night scene mode.
 * @note Make sure that stream source is zoom or wide camera.
 * @param position: camera mounted position
 * @param nightSceneMode: night scene mode.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetNightSceneMode(E_DjiMountPosition position,
                                                  E_DjiCameraManagerNightSceneMode nightSceneMode);

/**
 * @brief Get night scene mode.
 * @param position: camera mounted position
 * @param nightSceneMode: pointer to night scene mode.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetNightSceneMode(E_DjiMountPosition position,
                                                  E_DjiCameraManagerNightSceneMode *nightSceneMode);

/**
 * @brief Get range of stream source(s) can be stored when capturing or recording.
 * @param position: camera mounted position.
 * @param rangeList: returned value of range, in member streamStorage.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetStreamStorageRange(E_DjiMountPosition position,
                                                      T_DjiCameraManagerRangeList *rangeList);

/**
 * @brief Select capture or recording stream(s) to store.
 * @note Precondition: set camera's work corresponding to streamType
 * @param position: camera mounted position.
 * @param streamType: capture mode or recording mode.
 * @param streamStorageList: Pointer to the struct that contains stream list.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetCaptureRecordingStreams(E_DjiMountPosition position,
                                                            E_DjiCameraManagerCaptureOrRecording streamType,
                                                            T_DjiCameraManagerStreamList *streamStorageList);

/**
 * @brief Get the stream(s) of capture or recording mode to be stored.
 * @param position: camera mounted position.
 * @param streamType: capture mode or recording mode.
 * @param streamSourceList: the real return value.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetCaptureRecordingStreams(E_DjiMountPosition position,
                                                            E_DjiCameraManagerCaptureOrRecording streamType,
                                                            T_DjiCameraManagerStreamList *streamStorageList);

/**
 * @brief Turn on/off synchronized split screen zoom function.
 * @param position: camera mounted position.
 * @param enable: set true to turn on, false to turn off.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetSynchronizedSplitScreenZoomEnabled(E_DjiMountPosition position, bool enable);


/**
 * @brief Set suffix name of directory or file.
 * @note This setting applies only once for file names.
 * @param position: camera mounted position.
 * @param nameType: see E_DjiCameraManagerExpandNameType, select to set name of directory or file.
 * @param nameSize: Length of the name string, between 1 and 239 characters.
 * @param nameStr: Content of custom suffix name.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetCustomExpandName(E_DjiMountPosition position,
                                                     E_DjiCameraManagerExpandNameType nameType,
                                                     const uint8_t *nameStr,
                                                     uint32_t nameSize);

/**
 * @brief Get the custom suffix of the most recent directory or file name.
 * @param position: camera mounted position
 * @param nameType: to choose directory or file to get custom name
 * @param nameStr: name string buffer
 * @param nameSize: On input, indicates the maximum size of nameStr; on output,
 * the actual size of the name string.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetCustomExpandName(E_DjiMountPosition position,
                                                     E_DjiCameraManagerExpandNameType nameType,
                                                     uint8_t *nameStr,
                                                     uint32_t *nameSize);


/**
 * @brief Downloads a list of media files from the selected camera.
 * @note This synchronous interface may lead to higher CPU usage.
 * Times out after 3 seconds if the download fails.
 * @param position: the mount position of the camera
 * @param fileList: the pointer to the downloaded camera file list
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_DownloadFileList(E_DjiMountPosition position, T_DjiCameraManagerFileList *fileList);

/**
 * @brief Download selected camera media file list by slices.
 * @note This synchronous interface may lead to higher CPU usage.
 * Times out after 3 seconds if the download fails.
 * @param position: the mount position of the camera
 * @param sliceConfig: the slices config for downloading file list
 * @param fileList: the pointer to the downloaded camera file list
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_DownloadFileListBySlices(E_DjiMountPosition position,
                                                          T_DjiCameraManagerSliceConfig sliceConfig,
                                                          T_DjiCameraManagerFileList *fileList);
/**
 * @brief Registers a callback for downloading file data.
 * @param position: the mount position of the camera
 * @param callback: the download file data callback
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_RegDownloadFileDataCallback(E_DjiMountPosition position,
                                                             DjiCameraManagerDownloadFileDataCallback callback);

/**
 * @brief Downloads a media file specified by its index.
 * @note Supports downloading one file at a time. Wait for a download to finish before starting another.
 * This synchronous interface may lead to higher CPU usage.
 * Times out after 3 seconds if the download fails.
 * @param position: the mount position of the camera
 * @param fileIndex: the index of the camera media file
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_DownloadFileByIndex(E_DjiMountPosition position, uint32_t fileIndex);

/**
 * @brief Download selected camera media file by file index and file type.
 * @note This API only supports L2 camera.
 * Supports downloading one file at a time. Wait for a download to finish before starting another.
 * This synchronous interface may lead to higher CPU usage.
 * Times out after 3 seconds if the download fails.
 * @param position: the mount position of the camera
 * @param fileIndex: the index of the camera media file
 * @param fileType: the sub type of the camera media file
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_DownloadSubFileByIndexAndSubType(E_DjiMountPosition position, uint32_t index, E_DjiCameraMediaFileSubType fileType);
/**
 * @brief Obtains the rights to download media files before downloading media files
 * @param position: the mount position of the camera
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_ObtainDownloaderRights(E_DjiMountPosition position);

/**
 * @brief Releases the rights to download media files after downloading media files
 * @note Failure to release downloader rights may restrict access to the camera album
 * by the pilot app.
 * @param position: the mount position of the camera
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_ReleaseDownloaderRights(E_DjiMountPosition position);

/**
 * @brief Format SD card.
 * @param position: the mount position of the camera
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_FormatStorage(E_DjiMountPosition position);

/**
 * @brief Get storage info of SD card.
 * @note This API doesn't support L1/P1/M3D/M3TD models.
 * @param position: the mount position of the camera
 * @param storageInfo: Result of SD Card storage information
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetStorageInfo(E_DjiMountPosition position,
                                                T_DjiCameraManagerStorageInfo *storageInfo);

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

/**
 * @brief Set FFC mode.
 * @param position: camera mounted position.
 * @param ffcMode: mode to be set.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetFfcMode(E_DjiMountPosition position, E_DjiCameraManagerFfcMode ffcMode);

/**
 * @brief Trigger FFC one time.
 * @param position: camera mounted position.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_TriggerFfc(E_DjiMountPosition position);

/**
 * @brief Set infrared camera gain mode.
 * @param position: camera mounted position.
 * @param gainMode: gain mode to set.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetInfraredCameraGainMode(E_DjiMountPosition position,
                                               E_DjiCameraManagerIrGainMode gainMode);

/**
 * @brief Get temperature range of infrared camera.
 * @param position: camera mounted position.
 * @param tempRange: returned valued of temperature range.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetInfraredCameraGainModeTemperatureRange(E_DjiMountPosition position,
                                                               T_DjiCameraManagerIrTempMeterRange *tempRange);

/**
 * @brief Get metergin mode range of infrared camera.
 * @param position: camera mounted position.
 * @param tempRange: returned value of metering range.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetMeteringModeRange(E_DjiMountPosition position,
                                                      T_DjiCameraManagerRangeList *rangeList);

/**
 * @brief Set camera metering mode.
 * @param position: camera mounted position
 * @param meteringMode: metering mode
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetMeteringMode(E_DjiMountPosition position,
                                                 E_DjiCameraManagerMeteringMode meteringMode);

/**
 * @brief Get camera metering mode.
 * @param position: camera mounted position
 * @param meteringMode: pointer to returned value of metering mode
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetMeteringMode(E_DjiMountPosition position,
                                                 E_DjiCameraManagerMeteringMode *meteringMode);

/**
 * @brief Get range of metering point.
 * @param position: camera mounted position
 * @param hrzNum: returned value, horizontal range.
 * @param vtcNum: returned value, vertical range.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetMeteringPointRegionRange(E_DjiMountPosition position,
                                                             uint8_t *hrzNum, uint8_t *vtcNum);
/**
 * @brief Set metering point.
 * @param position: camera mounted position
 * @param x: Horizontal coordinate value, should be no greater than hrzNum - 1.
 * @param y: Horizontal coordinate value, should be no greater than vtcNum - 1.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetMeteringPoint(E_DjiMountPosition position,
                                                  uint8_t x, uint8_t y);

/**
 * @brief Set metering point normalize.
 * @param position: camera mounted position
 * @param x: Normalized horizontal coordinate, value ranges in 0 ~ 1.
 * @param y: Normalized Vertical coordinate, value ranges in 0 ~ 1.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_SetMeteringPointNormalized(E_DjiMountPosition position,
                                                           dji_f32_t x, dji_f32_t y);

/**
 * @brief Get camera metering mode.
 * @param position: camera mounted position
 * @param x: returned valued, current metering point in horizontal coordinate.
 * @param y: returned valued, current metering point in vertical coordinate.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetMeteringPoint(E_DjiMountPosition position,
                                                  uint8_t *x, uint8_t *y);


/**
 * @brief Get camera metering mode.
 * @param position: camera mounted position
 * @param x: a pointer to a float (dji_f32_t) that will receive the normalized x-coordinate of the
 * metering point.
 * @param y: a pointer to a float (dji_f32_t) that will receive the normalized y-coordinate of the
 * metering point.
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_GetMeteringPointNormalized(E_DjiMountPosition position,
                                                           dji_f32_t *x, dji_f32_t *y);

/**
 * @brief Start to record point cloud of the selected camera mounted position.
 * @param position: camera mounted position
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_StartRecordPointCloud(E_DjiMountPosition position);

/**
 * @brief Stop to record point cloud of the selected camera mounted position.
 * @note Precondition: The camera is recording currently.
 * @param position: camera mounted position
 * @return Execution result.
 */
T_DjiReturnCode DjiCameraManager_StopRecordPointCloud(E_DjiMountPosition position);

#ifdef __cplusplus
}
#endif

#endif // DJI_CAMERA_MANAGER_H
/************************ (C) COPYRIGHT DJI Innovations *******END OF FILE******/
