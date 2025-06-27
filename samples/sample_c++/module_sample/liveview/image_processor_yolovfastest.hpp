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
#ifndef __IMAGE_PROCESSOR_DIAPLAY_H__
#define __IMAGE_PROCESSOR_DIAPLAY_H__
#ifdef OPEN_CV_INSTALLED

#include <memory>
#include "opencv2/opencv.hpp"
#include <dji_liveview.h>

class ImageProcessorYolovFastest {
public:
    ImageProcessorYolovFastest(const std::string& name) : show_name_(name) {}

    ~ImageProcessorYolovFastest() {}

    int32_t Init();

    using Image = cv::Mat;
    void Process(const std::shared_ptr<Image>& image, std::vector<T_DjiLiveViewBoundingBox>& bounding_boxes);
    std::vector<T_DjiLiveViewBoundingBox> Process(const std::shared_ptr<Image>& image);

private:
    std::string show_name_;
    enum {
        kFilePathSizeMax = 256,
        kCurrentFilePathSizeMax = 128,
    };

    cv::dnn::Net net_;
    char cur_file_dir_path_[kCurrentFilePathSizeMax];
    char prototxt_file_dir_path_[kFilePathSizeMax];
    char weights_file_dir_path_[kFilePathSizeMax];
    void post_process(cv::Mat& frame, const std::vector<cv::Mat>& outs, std::vector<T_DjiLiveViewBoundingBox>& bounding_boxes);
};
#endif
#endif
// #endif
