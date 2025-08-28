/**
 ********************************************************************
 *
 * @copyright (c) 2023 DJI. All rights reserved.
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

/* Includes ------------------------------------------------------------------*/
#include <iostream>
#include <dji_logger.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "test_liveview_entry.hpp"
#include "dji_liveview_object_detection.hpp"
#include "dji_payload_camera.h"
#include "dji_high_speed_data_channel.h"
#include "dji_aircraft_info.h"
#include <string>
#include <vector>
#include "dji_typedef.h"
#include <chrono>
#include <thread>
#include <fstream>
#include <ctime>
#include <sstream>
#include "dji_open_ar.h"
#include <queue>

#ifdef OPEN_CV_INSTALLED
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/core.hpp>
#include "image_processor_yolovfastest.hpp"
#endif

/* Private constants ---------------------------------------------------------*/
#define YOLO_LABLES_NUM       76
#define INVALID_CLASS_NUM     4

static const char* s_classLables[] = {
    "person",        "bicycle",       "car",           "motorbike",
    "aeroplane",     "bus",           "train",         "truck",
    "boat",          "traffic light", "fire hydrant",  "stop sign",
    "parking meter", "bench",         "bird",          "cat",
    "dog",           "horse",         "sheep",         "cow",
    "elephant",      "bear",          "zebra",         "giraffe",
    "backpack",      "umbrella",      "handbag",       "tie",
    "suitcase",      "frisbee",       "skis",          "snowboard",
    "sports ball",   "kite",          "baseball bat",  "baseball glove",
    "skateboard",    "surfboard",     "tennis racket", "bottle",
    "wine glass",    "cup",           "fork",          "knife",
    "spoon",         "bowl",          "banana",        "apple",
    "sandwich",      "orange",        "broccoli",      "carrot",
    "hot dog",       "pizza",         "donut",         "cake",
    "chair",         "sofa",          "pottedplant",   "bed",
    "diningtable",   "toilet",        "tvmonitor",     "laptop",
    "mouse",         "remote",        "keyboard",      "cell phone",
    "microwave",     "oven",          "toaster",       "sink",
    "refrigerator",  "book",          "clock",         "vase",
    "scissors",      "teddy bear",    "hair drier",    "toothbrush",
};

static const char* s_invalidLables[] = {
    "XXX", "WW", "YYYYYYYYYYY", "ZZZZZZZZ"
};

static std::ofstream outFileH264;
static std::ofstream outFileYUV;
static std::string getCurrentTimestamp();
static T_DjiAircraftInfoBaseInfo aircraftInfoBaseInfo;
static void outH264Tofile(const uint8_t *buf, int32_t len);
static void outYUVTofile(const uint8_t *buf, int32_t len);
static void DjiLiveview_RcvImageCallback(E_DjiLiveViewCameraPosition position, const uint8_t *buf, uint32_t len ,T_DjiLiveviewImageInfo imageInfo);
static void DjiLiveview_EncoderUseCallback(const uint8_t *buf, uint32_t len);

#ifdef OPEN_CV_INSTALLED
static ImageProcessorYolovFastest processor("YOLOvFastest");
static std::queue<cv::Mat> s_imageQueue;
static std::queue<T_DjiLiveViewStandardMetaData *> s_metaQueue;
static void *DjiLiveview_ObjectDetectionThread(void *arg);
T_DjiTaskHandle s_procThreadHandle;
T_DjiMutexHandle s_metaQueueMutexHandle;
T_DjiMutexHandle s_imageQueueMutexHandle;
#endif

void DjiUser_InitOpenAr(T_DjiOpenArPoint* point)
{
    point->uuid = 1;
    point->style_id = 10;
    point->resource_id = 0;
    point->is_always_in_edge = 1;

    point->coordinate = {113.939467, 22.526366, 1.0};

    point->text_attr.is_show = 1;
    memcpy(point->text_attr.text, "测试文本", sizeof("测试文本"));

    point->icon_attr = {0, 1, 33445566};
    point->touch_attr = {1, {0.0, 0.0, 1.0}};
}

void ArRrefleshAll()
{
    USER_LOG_INFO("do ar reflesh all");
    uint8_t buf[1024];
    memset(buf, 0, sizeof(buf));

    T_DjiOpenArCircle* circle = (T_DjiOpenArCircle*)buf;
    circle->ids = {50, 10};
    circle->center = {113.939467, 22.526366, 11.};
    circle->radius = 2;
    circle->normal_vector = {0, 0, 3};
    circle->face = {0, 1, 1, 1};
    circle->stroke = {1, 1, 1, 1, 1, 1};
    DjiLiveview_ArSetCircle(circle);
}

void DjiUser_RunOpenArSample()
{
    DjiLiveview_ArRegRefleshAllCallback(ArRrefleshAll);
    bool exit = false;
    while (!exit)
    {
        std::cout
            << "\n"
            << "| Available commands:                                                                              \n"
            << "| [0] Set Point - draw tow points\n"
            << "| [1] Update point - append tow points\n"
            << "| [2] Delete point - delete tow point\n"
            << "| [3] Clear point- clear all points\n"
            << "| [4] Set line - draw a line with five points.\n"
            << "| [5] Update line - add a line with tow points.\n"
            << "| [6] Delete line - delete the line drawn in the [5] step.\n"
            << "| [7] Clear line - clear lines\n"
            << "| [8] Set polygon - draw a cuboid\n"
            << "| [9] Update polygon - draw a triangular prism\n"
            << "| [a] Delete polygon - delete the triangular prism\n"
            << "| [b] Clear polygon - clear polygon\n"
            << "| [c] Set circle - draw a circle\n"
            << "| [d] Update circle - add a circle\n"
            << "| [e] Delete circle - delete a circle\n"
            << "| [f] Clear circle - clear circle\n"
            << "| [q] Exit\n"
            << std::endl;

        char inputChar;
        std::cin >> inputChar;
        uint8_t buf[1024];
        memset(buf, 0, sizeof(buf));

        T_DjiOpenArPoint point;
        DjiUser_InitOpenAr(&point);

        T_DjiOpenArPointArray* point_array = (T_DjiOpenArPointArray*)buf;
        T_DjiOpenArLine* line = (T_DjiOpenArLine*)buf;
        T_DjiOpenArPolygon* polygon = (T_DjiOpenArPolygon*)buf;
        T_DjiOpenArCircle* circle = (T_DjiOpenArCircle*)buf;
        T_DjiOpenArPivotAxis* pivot = (T_DjiOpenArPivotAxis*)buf;

        T_DjiOpenArDeletePointEntry* delete_point_entry = (T_DjiOpenArDeletePointEntry*)buf;
        T_DjiOpenArDeleteLineEntry* delete_line_entry = (T_DjiOpenArDeleteLineEntry*)buf;
        T_DjiOpenArDeletePolygonEntry* delete_polygon_entry = (T_DjiOpenArDeletePolygonEntry*)buf;
        T_DjiOpenArDeleteCircleEntry* delete_circle_entry = (T_DjiOpenArDeleteCircleEntry*)buf;
        T_DjiOpenArDeletePovixAxisEntry* delete_povix_entry = (T_DjiOpenArDeletePovixAxisEntry*)buf;

        switch(inputChar) {
         case '0':
             point_array->len = 2;

             point.uuid = 1;
             memcpy(point.text_attr.text, "测试文本", sizeof("测试文本"));
             memcpy(&point_array->points[0], &point, sizeof(point));

             point.uuid = 2;
             point.coordinate.altitude = 2.0f;
             memcpy(point.text_attr.text, "test text", sizeof("test text"));
             memcpy(&point_array->points[1], &point, sizeof(point));

             DjiLiveview_ArSetPoint(point_array);
             break;
         case '1':
             point_array->len = 2;
             point.uuid = 3;
             point.coordinate.altitude = 3.0f;
             memcpy(point.text_attr.text, "point update", sizeof("point update"));
             memcpy(&point_array->points[0], &point, sizeof(point));

             point.uuid = 4;
             point.coordinate.altitude = 4.0f;
             memcpy(&point_array->points[1], &point, sizeof(point));
             DjiLiveview_ArUpdatePoint(point_array);

             break;
         case '2':
             delete_point_entry->resource_id = 0;
             delete_point_entry->uuid_len = 2;
             delete_point_entry->uuid_array[0] = 1;
             delete_point_entry->uuid_array[1] = 2;
             DjiLiveview_ArDeletePoint(delete_point_entry);
             break;

         case '3':
             DjiLiveview_ArClearPoint(0);
             break;

         case '4':
             line->ids.uuid = 10;
             line->ids.style_id = 10;
             line->point_array.len = 5;

             point.uuid = 11;
             point.coordinate.altitude = 4.0f;
             memcpy(&line->point_array.points[0], &point, sizeof(point));

             point.uuid = 12;
             point.coordinate.altitude = 5.0f;
             memcpy(&line->point_array.points[1], &point, sizeof(point));

             point.uuid = 13;
             point.coordinate.altitude = 6.0f;
             memcpy(&line->point_array.points[2], &point, sizeof(point));

             point.uuid = 14;
             point.coordinate.altitude = 7.0f;
             memcpy(&line->point_array.points[3], &point, sizeof(point));

             point.uuid = 15;
             point.coordinate.altitude = 8.0f;
             memcpy(&line->point_array.points[4], &point, sizeof(point));

             DjiLiveview_ArSetLine(line);
             break;

         case '5':
             line->ids.uuid = 11;
             line->ids.style_id = 10;
             line->point_array.len = 2;

             point.uuid = 16;
             point.coordinate.altitude = 9.0f;
             memcpy(&line->point_array.points[0], &point, sizeof(point));

             point.uuid = 17;
             point.coordinate.altitude = 10.0f;
             memcpy(&line->point_array.points[1], &point, sizeof(point));

             DjiLiveview_ArUpdateLine(line);
             break;

         case '6':
             delete_line_entry[0].uuid = 10;
             DjiLiveview_ArDeleteLine(delete_line_entry, 1);
             break;

         case '7':
             DjiLiveview_ArClearLine();
             break;

         case '8':
             polygon->ids = {40, 11};
             polygon->face = {0, 1, 1, 1};
             polygon->stroke = {1, 1, 1, 1, 1, 1};
             polygon->normal_vector = {0, 0, 6};
             polygon->point_array.len = 4;
             point.uuid = 41;
             point.coordinate.altitude = 1;
             point.coordinate.longitude = 113.939438538;
             point.coordinate.latitude  = 22.5263937487;
             memcpy(&polygon->point_array.points[0], &point, sizeof(point));

             point.uuid = 42;
             point.coordinate.longitude = 113.939496338;
             point.coordinate.latitude = 22.5263937487;
             memcpy(&polygon->point_array.points[1], &point, sizeof(point));

             point.uuid = 43;
             point.coordinate.longitude = 113.939496338;
             point.coordinate.latitude  = 22.5263397487;
             memcpy(&polygon->point_array.points[2], &point, sizeof(point));

             point.uuid = 44;
             point.coordinate.longitude = 113.939438538;
             point.coordinate.latitude = 22.5263397487;
             memcpy(&polygon->point_array.points[3], &point, sizeof(point));

             DjiLiveview_ArSetPolygon(polygon);
             break;

         case '9':
             polygon->ids = {45, 11};
             polygon->face = {0, 1, 1, 1};
             polygon->stroke = {1, 1, 1, 1, 1, 1};
             polygon->normal_vector = {0, 0, 4};
             polygon->point_array.len = 3;
             point.uuid = 46;
             point.coordinate.altitude = 12;
             point.coordinate.longitude = 113.939438538;
             point.coordinate.latitude  = 22.5263937487;
             memcpy(&polygon->point_array.points[0], &point, sizeof(point));

             point.uuid = 47;
             point.coordinate.longitude = 113.939496338;
             point.coordinate.latitude = 22.5263937487;
             memcpy(&polygon->point_array.points[1], &point, sizeof(point));

             point.uuid = 48;
             point.coordinate.longitude = 113.939496338;
             point.coordinate.latitude  = 22.5263397487;
             memcpy(&polygon->point_array.points[2], &point, sizeof(point));
             DjiLiveview_ArUpdatePolygon(polygon);

             break;
         case 'a':
             delete_polygon_entry->uuid = 40;
             DjiLiveview_ArDeletePolygon(delete_polygon_entry, 1);
             break;

         case 'b':
             DjiLiveview_ArClearPolygon();
             break;

         case 'c':
             circle->ids = {50, 10};
             circle->center = {113.939467, 22.526366, 11.};
             circle->radius = 3;
             circle->normal_vector = {0, 0, 3};
             circle->face = {0, 1, 1, 1};
             circle->stroke = {1, 1, 1, 1, 1, 1};
             DjiLiveview_ArSetCircle(circle);
             break;

         case 'd':
             circle->ids = {51, 11};
             circle->center = {113.939467, 22.526366, 8.};
             circle->radius = 4;
             circle->normal_vector = {0, 0, 5};
             circle->face = {1, 1, 1, 1};
             circle->stroke = {1, 1, 1, 1, 1, 1};
             DjiLiveview_ArUpdateCircle(circle);
             break;

         case 'e':
             delete_circle_entry->uuid = 50;
             DjiLiveview_ArDeleteCircle(delete_circle_entry, 1);
             break;

         case 'f':
             DjiLiveview_ArClearCircle();
             break;
         case 'q':
             exit = true;
             break;
         default:
             break;
        }
    }
}

void DjiUser_RunCameraAiDetectionSample()
{
    int pos = 1;
    int mediaSource = 0; //support 0(app liveview)/1(1080p)/7(4k) for H30 camera
    char isQuit;
    E_DjiLiveViewCameraPosition CameraPostion;
    E_DjiLiveViewCameraSource MediaResource;

    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    USER_LOG_INFO("Input cammera sourece(1:1080p, 3:M4 serials 4K, 7:H30 serials 4K): ");
    std::cin >> mediaSource;
    if (pos < 1 || pos > 3 || mediaSource > 7)
    {
        USER_LOG_ERROR("invalid param");
        return;
    }

    std::string timestamp = getCurrentTimestamp();

    // avoid miss dir error
    mkdir ("data", 0755);
    std::string h264FileName = "data/output_" + timestamp + ".h264";
    outFileH264.open(h264FileName, std::ios::out | std::ios::binary | std::ios::app);
    if (!outFileH264) {
        std::cerr << "cant open " << h264FileName << std::endl;
    }

#ifdef OPEN_CV_INSTALLED
    osalHandler->MutexCreate(&s_metaQueueMutexHandle);
    osalHandler->MutexCreate(&s_imageQueueMutexHandle);
    osalHandler->TaskCreate("objectDetectionTask",DjiLiveview_ObjectDetectionThread,1024*1024,NULL, &s_procThreadHandle);
    if (processor.Init() != 0) {
        std::cerr << "Failed to initialize the processor." << std::endl;
        return ;
    }
#endif

    CameraPostion = static_cast<E_DjiLiveViewCameraPosition>(pos);
    MediaResource = static_cast<E_DjiLiveViewCameraSource>(mediaSource);
    T_DjiReturnCode returnCode;
    const T_DjiDataChannelBandwidthProportionOfHighspeedChannel bandwidthProportionOfHighspeedChannel =
        {10, 60, 30};

    returnCode = DjiHighSpeedDataChannel_SetBandwidthProportion(bandwidthProportionOfHighspeedChannel);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Liveview init failed, HighSpeed channel init error: 0x%08llX", returnCode);
        return;
    }
    USER_LOG_INFO("step 1: init liveview");

    returnCode = DjiLiveview_Init();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Liveview init faild, ret: 0x%08llX", returnCode);
        goto init_failed;
    }

    returnCode = DjiAircraftInfo_GetBaseInfo(&aircraftInfoBaseInfo);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("get aircraft base info error");
        return;
    }

#ifdef OPEN_CV_INSTALLED
    returnCode = DjiLiveview_RegUserAiTargetLableList(YOLO_LABLES_NUM, s_classLables);
#else
    returnCode = DjiLiveview_RegUserAiTargetLableList(INVALID_CLASS_NUM, s_invalidLables);
#endif

    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR("Reg ai target lable faild, ret: 0x%08llX", returnCode);
        goto init_failed;
    }
    USER_LOG_INFO("step 2: reg encoder callback");

    returnCode = DjiLiveview_RegEncoderCallback(DjiLiveview_EncoderUseCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR( "reg Yuv Encoder callback faild, ret: 0x%08llX", returnCode);
        goto reg_encoder_callback_failed;
    }
    USER_LOG_INFO("step 3:start yuv stream");

    returnCode = DjiLiveview_StartImageStream(CameraPostion, MediaResource,
                                              PIXFMT_RGB_PACKED ,DjiLiveview_RcvImageCallback);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR( "start to subscribe YUV stream failed, ret: 0x%08llX", returnCode);
        goto start_yuv_stream_failed;
    }
    USER_LOG_INFO("codec sample start ...");

    while (true) {
        std::cin >> isQuit;
        if (isQuit == 'q' || isQuit == 'Q') {
            break;
        }
    }
    USER_LOG_INFO("codec sample end");

DjiLiveview_UnregUserAiTargetLableList();
start_yuv_stream_failed:
    returnCode = DjiLiveview_StopImageStream(CameraPostion, MediaResource);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR( "stop subscrib YUV stream failed, ret: 0x%08llX", returnCode);
    }

reg_encoder_callback_failed:
    returnCode = DjiLiveview_UnregEncoderCallback();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR( "unreg encoder callback failed, ret: 0x%08llX", returnCode);
    }

init_failed:
    returnCode = DjiLiveview_Deinit();
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
    {
        USER_LOG_ERROR( "deinit liveview failed, ret: 0x%08llX", returnCode);
    }
    outFileH264.close();
    outFileYUV.close();

#ifdef OPEN_CV_INSTALLED
    osalHandler->TaskDestroy(s_procThreadHandle);
    osalHandler->MutexDestroy(s_imageQueueMutexHandle);
    osalHandler->MutexDestroy(s_metaQueueMutexHandle);

    while(!s_imageQueue.empty()){
        s_imageQueue.pop();
    }
    while(!s_metaQueue.empty()) {
        s_metaQueue.pop();
    }

#endif

}

static std::string getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm* now_tm = std::localtime(&now);

    std::ostringstream oss;
    oss << (now_tm->tm_year + 1900)
        << (now_tm->tm_mon + 1)
        << now_tm->tm_mday
        << now_tm->tm_hour
        << now_tm->tm_min;

    return oss.str();
}
static void outH264Tofile(const uint8_t *buf, int32_t len) {
    if (!outFileH264) {
        USER_LOG_ERROR( "output.h264 is not open");
        return;
    }
    outFileH264.write(reinterpret_cast<const char *>(buf), len);
}

static void outYUVTofile(const uint8_t *buf, int32_t len) {
    if (!outFileYUV) {
        USER_LOG_ERROR( "outyuv.h264 is not open");
        return;
    }
    outFileYUV.write(reinterpret_cast<const char *>(buf), len);
}

static void DjiLiveview_RcvImageCallback(E_DjiLiveViewCameraPosition position, const uint8_t *buf, uint32_t len, T_DjiLiveviewImageInfo imageInfo)
{
    T_DjiReturnCode DjiStat;
    uint32_t OutPutLen;
    std::vector<T_DjiLiveViewBoundingBox> bounding_boxes;
    USER_LOG_INFO("catch image frame data, image type = %d  height = %d, width = %d, frameId = %d, bufferLen= %d",
                  imageInfo.pixFmt ,imageInfo.height, imageInfo.width, imageInfo.frameId, len);
    T_DjiLiveViewStandardMetaData * metaData = nullptr;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

#ifdef OPEN_CV_INSTALLED
    cv::Mat rgb_image( imageInfo.height, imageInfo.width, CV_8UC3, const_cast<uint8_t*>(buf));
    cv::Mat rgb_image_copy = rgb_image.clone();

    osalHandler->MutexLock(s_imageQueueMutexHandle);
    while (s_imageQueue.size() > 30) {
        USER_LOG_WARN("The image queue is full. Drop this strike.");
        s_imageQueue.pop();
    }
    s_imageQueue.push(rgb_image_copy);
    osalHandler->MutexUnlock(s_imageQueueMutexHandle);

    osalHandler->MutexLock(s_metaQueueMutexHandle);
    if (!s_metaQueue.empty()) {
        metaData = s_metaQueue.front();
        s_metaQueue.pop();
    }
    osalHandler->MutexUnlock(s_metaQueueMutexHandle);

    DjiLiveview_EncodeAFrameToH264(buf, len, imageInfo, metaData);
    if(metaData != nullptr) free(metaData);

#else

    size_t size = sizeof(T_DjiLiveViewStandardMetaData) + 3 * sizeof(T_DjiLiveViewBoundingBox);
    metaData = (T_DjiLiveViewStandardMetaData *)malloc(size);
    if (!metaData) {
        fprintf(stderr, "Failed to allocate memory\n");
        return ;
    }

    // 初始化 boxCount
    metaData->boxCount = 4;

    // 初始化每个 T_DjiLiveViewBoundingBox
    for (int i = 0; i < 4; i++) {
        metaData->boxData[i].id = i;
        metaData->boxData[i].type = i;
        metaData->boxData[i].state = 1;
        metaData->boxData[i].box.cx =  (i+1)*1000;
        metaData->boxData[i].box.cy =  (i+1)*1000;
        metaData->boxData[i].box.w = 1000;
        metaData->boxData[i].box.h = 1000;
        metaData->boxData[i].box.distance = 0;
    }

    // 打印结果

    DjiLiveview_SendAiMetaToPilot(metaData);

    DjiLiveview_EncodeAFrameToH264(buf, len,imageInfo, metaData);
#endif
}

static void DjiLiveview_EncoderUseCallback(const uint8_t *buf, uint32_t len)
{
    T_DjiReturnCode returnCode;
    outH264Tofile(buf, len);
    if (aircraftInfoBaseInfo.aircraftSeries != DJI_AIRCRAFT_SERIES_M4D)
    {
        returnCode = DjiPayloadCamera_SendVideoStream(buf, len);
        if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS)
        {
            USER_LOG_ERROR("failed to send video to pilot, ret: 0x%08llX", returnCode);
        }
    }
}

static void* DjiLiveview_ObjectDetectionThread(void *arg) {
    T_DjiReturnCode DjiStat;
    T_DjiOsalHandler *osalHandler = DjiPlatform_GetOsalHandler();

    while(1) {
        #ifdef OPEN_CV_INSTALLED
        osalHandler->MutexLock(s_imageQueueMutexHandle);
        if (s_imageQueue.empty()) {
            osalHandler->MutexUnlock(s_imageQueueMutexHandle);
            continue;
        }
        cv::Mat rgb_image = s_imageQueue.front();
        s_imageQueue.pop();
        osalHandler->MutexUnlock(s_imageQueueMutexHandle);
        cv::Mat bgr_image;
        cv::cvtColor(rgb_image, bgr_image, cv::COLOR_RGB2BGR);

        std::shared_ptr<cv::Mat> image_ptr = std::make_shared<cv::Mat>(bgr_image);


        std::vector<T_DjiLiveViewBoundingBox> bounding_boxes;
        processor.Process(image_ptr, bounding_boxes);

        T_DjiLiveViewStandardMetaData *metaData = (T_DjiLiveViewStandardMetaData *)malloc(
            sizeof(T_DjiLiveViewStandardMetaData) + bounding_boxes.size() * sizeof(T_DjiLiveViewBoundingBox));
        metaData->boxCount = bounding_boxes.size();
        for (int i = 0; i < bounding_boxes.size(); i++) {
            metaData->boxData[i] = bounding_boxes[i];
        }

        DjiLiveview_SendAiMetaToPilot(metaData);

        osalHandler->MutexLock(s_metaQueueMutexHandle);
        s_metaQueue.push(metaData);
        osalHandler->MutexUnlock(s_metaQueueMutexHandle);

        #else
            break;
        #endif
    }
    return NULL;
}
