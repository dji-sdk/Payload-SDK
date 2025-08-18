/**
 ********************************************************************
 * @file    test_liveview_entry.cpp
 * @brief
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

/* Includes ------------------------------------------------------------------*/
#include <iostream>
#include <dji_logger.h>
#include "test_liveview_entry.hpp"
#include "test_liveview.hpp"

#ifdef OPEN_CV_INSTALLED

#include "opencv2/opencv.hpp"
#include "opencv2/dnn.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "../../../sample_c/module_sample/utils/util_misc.h"

using namespace cv;
#endif
using namespace std;

/* Private constants ---------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private values -------------------------------------------------------------*/
const char *classNames[] = {"background", "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck",
                            "boat", "traffic light",
                            "fire hydrant", "background", "stop sign", "parking meter", "bench", "bird", "cat", "dog",
                            "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "background", "backpack",
                            "umbrella", "background", "background", "handbag", "tie", "suitcase", "frisbee", "skis",
                            "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard",
                            "surfboard", "tennis racket",
                            "bottle", "background", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana",
                            "apple", "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut",
                            "cake", "chair", "couch", "potted plant", "bed", "background", "dining table", "background",
                            "background", "toilet", "background", "tv", "laptop", "mouse", "remote", "keyboard",
                            "cell phone", "microwave", "oven", "toaster", "sink", "refrigerator", "background", "book",
                            "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush"};

const size_t inWidth = 320;
const size_t inHeight = 300;
const float WHRatio = inWidth / (float) inHeight;
static int32_t s_demoIndex = -1;
char curFileDirPath[DJI_FILE_PATH_SIZE_MAX];
char tempFileDirPath[DJI_FILE_PATH_SIZE_MAX];
char prototxtFileDirPath[DJI_FILE_PATH_SIZE_MAX];
char weightsFileDirPath[DJI_FILE_PATH_SIZE_MAX];

/* Private functions declaration ---------------------------------------------*/
static void DjiUser_ShowRgbImageCallback(CameraRGBImage img, void *userData);
static T_DjiReturnCode DjiUser_GetCurrentFileDirPath(const char *filePath, uint32_t pathBufferSize, char *dirPath);

/* Exported functions definition ---------------------------------------------*/
void DjiUser_RunCameraStreamViewSample()
{
    char cameraIndexChar = 0;
    char demoIndexChar = 0;
    char isQuit = 0;
    CameraRGBImage camImg;
    char fpvName[] = "FPV_CAM";
    char mainName[] = "MAIN_CAM";
    char viceName[] = "VICE_CAM";
    char topName[] = "TOP_CAM";
    T_DjiReturnCode returnCode;

    LiveviewSample *liveviewSample;
    try {
        liveviewSample = new LiveviewSample();
    } catch (...) {
        return;
    }

    returnCode = DjiUser_GetCurrentFileDirPath(__FILE__, DJI_FILE_PATH_SIZE_MAX, curFileDirPath);
    if (returnCode != DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS) {
        USER_LOG_ERROR("Get file current path error, stat = 0x%08llX", returnCode);
    }

    cout << "Please choose the stream demo you want to run\n\n"
         << "--> [0] Normal RGB image display\n"
         << "--> [1] Binary image display\n"
         << "--> [2] Faces detection demo\n"
         << "--> [3] Tensorflow Object detection demo\n"
         << endl;
    cin >> demoIndexChar;

    switch (demoIndexChar) {
        case '0':
            s_demoIndex = 0;
            break;
        case '1':
            s_demoIndex = 1;
            break;
        case '2':
            s_demoIndex = 2;
            break;
        case '3':
            s_demoIndex = 3;
            break;
        default:
            cout << "No demo selected";
            delete liveviewSample;
            return;
    }

    cout << "Please enter the type of camera stream you want to view\n\n"
         << "--> [0] Fpv Camera\n"
         << "--> [1] Main Camera\n"
         << "--> [2] Vice Camera\n"
         << "--> [3] Top Camera\n"
         << endl;
    cin >> cameraIndexChar;

    switch (cameraIndexChar) {
        case '0':
            liveviewSample->StartFpvCameraStream(&DjiUser_ShowRgbImageCallback, &fpvName);
            break;
        case '1':
            liveviewSample->StartMainCameraStream(&DjiUser_ShowRgbImageCallback, &mainName);
            break;
        case '2':
            liveviewSample->StartViceCameraStream(&DjiUser_ShowRgbImageCallback, &viceName);
            break;
        case '3':
            liveviewSample->StartTopCameraStream(&DjiUser_ShowRgbImageCallback, &topName);
            break;
        default:
            cout << "No camera selected";
            delete liveviewSample;
            return;
    }

    cout << "Please enter the 'q' or 'Q' to quit camera stream view\n"
         << endl;

    while (true) {
        cin >> isQuit;
        if (isQuit == 'q' || isQuit == 'Q') {
            break;
        }
    }

    switch (cameraIndexChar) {
        case '0':
            liveviewSample->StopFpvCameraStream();
            break;
        case '1':
            liveviewSample->StopMainCameraStream();
            break;
        case '2':
            liveviewSample->StopViceCameraStream();
            break;
        case '3':
            liveviewSample->StopTopCameraStream();
            break;
        default:
            cout << "No camera selected";
            delete liveviewSample;
            return;
    }

    delete liveviewSample;
}

/* Private functions definition-----------------------------------------------*/
static void DjiUser_ShowRgbImageCallback(CameraRGBImage img, void *userData)
{
    string name = string(reinterpret_cast<char *>(userData));

#ifdef OPEN_CV_INSTALLED
    Mat mat(img.height, img.width, CV_8UC3, img.rawData.data(), img.width * 3);

    if (s_demoIndex == 0) {
        cvtColor(mat, mat, COLOR_RGB2BGR);
        imshow(name, mat);
    } else if (s_demoIndex == 1) {
        cvtColor(mat, mat, COLOR_RGB2GRAY);
        Mat mask;
        cv::threshold(mat, mask, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
        imshow(name, mask);
    } else if (s_demoIndex == 2) {
        cvtColor(mat, mat, COLOR_RGB2BGR);
        snprintf(tempFileDirPath, DJI_FILE_PATH_SIZE_MAX, "%s/data/haarcascade_frontalface_alt.xml", curFileDirPath);
        auto faceDetector = cv::CascadeClassifier(tempFileDirPath);
        std::vector<Rect> faces;
        faceDetector.detectMultiScale(mat, faces, 1.1, 3, 0, Size(50, 50));

        for (int i = 0; i < faces.size(); ++i) {
            cout << "index: " << i;
            cout << "  x: " << faces[i].x;
            cout << "  y: " << faces[i].y << endl;

            cv::rectangle(mat, cv::Point(faces[i].x, faces[i].y),
                          cv::Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height),
                          Scalar(0, 0, 255), 2, 1, 0);
        }
        imshow(name, mat);
    } else if (s_demoIndex == 3) {
        snprintf(prototxtFileDirPath, DJI_FILE_PATH_SIZE_MAX,
                 "%s/data/tensorflow/ssd_inception_v2_coco_2017_11_17.pbtxt",
                 curFileDirPath);
        //Attention: If you want to run the Tensorflow Object detection demo, Please download the tensorflow model.
        //Download Url: http://download.tensorflow.org/models/object_detection/ssd_inception_v2_coco_2017_11_17.tar.gz
        snprintf(weightsFileDirPath, DJI_FILE_PATH_SIZE_MAX, "%s/data/tensorflow/frozen_inference_graph.pb",
                 curFileDirPath);

        dnn::Net net = cv::dnn::readNetFromTensorflow(weightsFileDirPath, prototxtFileDirPath);
        Size frame_size = mat.size();

        Size cropSize;
        if (frame_size.width / (float) frame_size.height > WHRatio) {
            cropSize = Size(static_cast<int>(frame_size.height * WHRatio),
                            frame_size.height);
        } else {
            cropSize = Size(frame_size.width,
                            static_cast<int>(frame_size.width / WHRatio));
        }

        Rect crop(Point((frame_size.width - cropSize.width) / 2,
                        (frame_size.height - cropSize.height) / 2),
                  cropSize);

        cv::Mat blob = cv::dnn::blobFromImage(mat, 1, Size(300, 300));
        net.setInput(blob);
        Mat output = net.forward();
        Mat detectionMat(output.size[2], output.size[3], CV_32F, output.ptr<float>());

        mat = mat(crop);
        float confidenceThreshold = 0.50;

        for (int i = 0; i < detectionMat.rows; i++) {
            float confidence = detectionMat.at<float>(i, 2);
            if (confidence > confidenceThreshold) {
                auto objectClass = (size_t) (detectionMat.at<float>(i, 1));

                int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * mat.cols);
                int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * mat.rows);
                int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * mat.cols);
                int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * mat.rows);

                ostringstream ss;
                ss << confidence;
                String conf(ss.str());

                Rect object((int) xLeftBottom, (int) yLeftBottom,
                            (int) (xRightTop - xLeftBottom),
                            (int) (yRightTop - yLeftBottom));

                rectangle(mat, object, Scalar(0, 255, 0), 2);
                String label = String(classNames[objectClass]) + ": " + conf;

                int baseLine = 0;
                Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
                rectangle(mat, Rect(Point(xLeftBottom, yLeftBottom - labelSize.height),
                                    Size(labelSize.width, labelSize.height + baseLine)), Scalar(0, 255, 0), cv::FILLED);
                putText(mat, label, Point(xLeftBottom, yLeftBottom), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));
            }
        }
        imshow(name, mat);
    }

    cv::waitKey(1);
#endif
}

static T_DjiReturnCode DjiUser_GetCurrentFileDirPath(const char *filePath, uint32_t pathBufferSize, char *dirPath)
{
    uint32_t i = strlen(filePath) - 1;
    uint32_t dirPathLen;

    while (filePath[i] != '/') {
        i--;
    }

    dirPathLen = i + 1;

    if (dirPathLen + 1 > pathBufferSize) {
        return DJI_ERROR_SYSTEM_MODULE_CODE_INVALID_PARAMETER;
    }

    memcpy(dirPath, filePath, dirPathLen);
    dirPath[dirPathLen] = 0;

    return DJI_ERROR_SYSTEM_MODULE_CODE_SUCCESS;
}

/****************** (C) COPYRIGHT DJI Innovations *****END OF FILE****/
