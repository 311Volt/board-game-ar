#include <catan/image_correction.hpp>

void showScaledFixed(std::string name, cv::Mat mat)
{
	cv::namedWindow(name, cv::WINDOW_NORMAL);
	cv::imshow(name, mat);
	cv::resizeWindow(name, 640, 480);
}

cv::Mat scaleImage(cv::Mat image, float scale)
{
	cv::Mat scaled;
	cv::resize(image, scaled, cv::Size(image.size().width * scale, image.size().height * scale), cv::INTER_AREA);
	return scaled;
}


void drawPoints(const std::vector<cv::Point2d>& points, cv::Mat outImg, cv::Scalar color)
{
	for (const auto& p : points) {
		cv::circle(outImg, p, 10, color, -1);
	}
}

cv::Mat rotateImage(cv::Mat image, int angle)
{
	cv::Point2i center = cv::Point2i(image.size().width / 2, image.size().height / 2);
	auto rotationMatrix = cv::getRotationMatrix2D(center, angle, 1.0);
	cv::Mat rotatedImage;
	cv::warpAffine(image, rotatedImage, rotationMatrix, image.size(), cv::INTER_LINEAR);
	return rotatedImage;
}