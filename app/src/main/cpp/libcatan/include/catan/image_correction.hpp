#ifndef INCLUDE_CATAN_IMAGE_CORRECTION
#define INCLUDE_CATAN_IMAGE_CORRECTION

#include <opencv2/opencv.hpp>


namespace ctn {
	cv::Mat GetBoardPerspectiveCorrectionMatrix(const std::vector<cv::Point>& points);
	cv::Mat GenerateIdealEdgeMask(cv::Scalar color, float thickness);
	cv::Mat FindFineAlignment(cv::Mat actualEdges, cv::Mat idealEdges);
	cv::Mat CreateDarkEdgeMask(cv::Mat input);
};


#endif /* INCLUDE_CATAN_IMAGE_CORRECTION */
