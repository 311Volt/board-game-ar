#ifndef INCLUDE_CATAN_IMAGE_CORRECTION
#define INCLUDE_CATAN_IMAGE_CORRECTION

#include <opencv2/opencv.hpp>


cv::Mat getPerspectiveCorrectionMatrix(const std::vector<cv::Point>& points);
cv::Mat GenerateReferenceEdgeThres();
cv::Mat FindAlignment(cv::Mat actualEdges, cv::Mat idealEdges);
cv::Mat IsolateDarkEdges(cv::Mat input);

#endif /* INCLUDE_CATAN_IMAGE_CORRECTION */