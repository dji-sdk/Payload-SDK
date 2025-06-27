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
#ifdef OPEN_CV_INSTALLED
#include "image_processor_yolovfastest.hpp"

/* Includes ------------------------------------------------------------------*/
#include <sys/time.h>
#include <dji_logger.h>
#include <fstream>
#include <iostream>
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <sstream>
#include <utils/util_misc.h>

using namespace cv;
using namespace dnn;
using namespace std;

int32_t ImageProcessorYolovFastest::Init() {

    memset(cur_file_dir_path_, 0, kCurrentFilePathSizeMax);
    memset(prototxt_file_dir_path_, 0, kFilePathSizeMax);
    memset(weights_file_dir_path_, 0, kFilePathSizeMax);

    if (DjiUserUtil_GetCurrentFileDirPath(__FILE__, sizeof(cur_file_dir_path_),
                              cur_file_dir_path_) != 0) {
        USER_LOG_ERROR("get path failed");
        return -1;
    }

    snprintf(prototxt_file_dir_path_, kFilePathSizeMax,
             "%s/data/yolo-fastest-1.1_coco/yolo-fastest-1.1-xl.cfg",
             cur_file_dir_path_);
    snprintf(weights_file_dir_path_, kFilePathSizeMax,
             "%s/data/yolo-fastest-1.1_coco/yolo-fastest-1.1-xl.weights",
             cur_file_dir_path_);

    USER_LOG_DEBUG("%s, %s", prototxt_file_dir_path_, weights_file_dir_path_);
    net_ = readNetFromDarknet(prototxt_file_dir_path_, weights_file_dir_path_);

    if (net_.empty()) {
        USER_LOG_ERROR("Failed to load network");
        return -1;
    }

    return 0;
}

void ImageProcessorYolovFastest::post_process(cv::Mat& frame, const std::vector<cv::Mat>& outs, std::vector<T_DjiLiveViewBoundingBox>& bounding_boxes) {
    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (size_t i = 0; i < outs.size(); ++i) {
        float* data = (float*)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols) {
            cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            cv::Point classid_point;
            double confidence;
            cv::minMaxLoc(scores, 0, &confidence, 0, &classid_point);
            if (confidence > 0.5) {
                int cx = (int)(data[0] * frame.cols);
                int cy = (int)(data[1] * frame.rows);
                int w = (int)(data[2] * frame.cols);
                int h = (int)(data[3] * frame.rows);
                int left = cx - (w >> 1);
                int top = cy - (h >> 1);
                class_ids.push_back(classid_point.x);
                confidences.push_back((float)confidence);
                boxes.push_back(cv::Rect(left, top, w, h));
            }
        }
    }

    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, 0.5, 0.4, indices);
    for (size_t i = 0; i < indices.size(); ++i) {
        int idx = indices[i];
        cv::Rect box = boxes[idx];

        T_DjiLiveViewBoundingBox bounding_box;
        bounding_box.id = i;
        bounding_box.type = class_ids[idx];
        bounding_box.state = 1;
        bounding_box.box.cx = (uint16_t)((box.x + box.width / 2) * 10000 / frame.cols);
        bounding_box.box.cy = (uint16_t)((box.y + box.height / 2) * 10000 / frame.rows);
        bounding_box.box.w = (uint16_t)(box.width * 10000 / frame.cols);
        bounding_box.box.h = (uint16_t)(box.height * 10000 / frame.rows);
        bounding_box.box.distance = 0;
        bounding_boxes.push_back(bounding_box);

        std::cout << "Bounding Box " << i << ": "
                  << "Class ID = " << class_ids[idx] << ", "
                  << "Confidence = " << confidences[idx] << ", "
                  << "Box = [" << box.x << ", " << box.y << ", " << box.width << ", " << box.height << "]"
                  << std::endl;

        cv::rectangle(frame, box, cv::Scalar(0, 255, 0), 2);
        std::string label = cv::format("ID: %d Conf: %.2f", class_ids[idx], confidences[idx]);
        cv::putText(frame, label, cv::Point(box.x, box.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
    }
}

void ImageProcessorYolovFastest::Process(const std::shared_ptr<Image>& image, std::vector<T_DjiLiveViewBoundingBox>& bounding_boxes) {
    auto detect = [&](cv::Mat& frame, std::vector<cv::Mat>& outs) {
        cv::Mat blob;
        cv::dnn::blobFromImage(frame, blob, 1 / 255.0, cv::Size(320, 320), cv::Scalar(0, 0, 0), true, false);
        net_.setInput(blob);
        net_.forward(outs, net_.getUnconnectedOutLayersNames());
    };

    cv::Mat frame = *image;
    std::vector<cv::Mat> outs;
    detect(frame, outs);

    post_process(frame, outs, bounding_boxes);
}

#endif
