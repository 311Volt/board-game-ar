#include <catan/image_correction.hpp>

void showScaled(std::string name, cv::Mat mat)
{
	cv::namedWindow(name, cv::WINDOW_NORMAL);
	cv::imshow(name, mat);
	cv::resizeWindow(name, 640, 480);
}

void drawPoints(const std::vector<cv::Point2d>& points, cv::Mat outImg, cv::Scalar color)
{
	for (const auto& p : points) {
		cv::circle(outImg, p, 10, color, -1);
	}
}
