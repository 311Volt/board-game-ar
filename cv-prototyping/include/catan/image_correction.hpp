#ifndef INCLUDE_CATAN_IMAGE_CORRECTION
#define INCLUDE_CATAN_IMAGE_CORRECTION

#include <opencv2/opencv.hpp>


cv::Mat getPerspectiveCorrectionMatrix(const std::vector<cv::Point>& points);


#endif /* INCLUDE_CATAN_IMAGE_CORRECTION */
