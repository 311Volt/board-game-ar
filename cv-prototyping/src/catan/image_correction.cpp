#include <catan/image_correction.hpp>

cv::Mat getPerspectiveCorrectionMatrix(const std::vector<cv::Point>& points)
{
	std::vector<cv::Point> dstPoints = {
		{0, 433},
		{250, 866},
		{750, 866},
		{1000, 433},
		{750, 0},
		{250, 0}
	};
	return cv::findHomography(points, dstPoints);
}
