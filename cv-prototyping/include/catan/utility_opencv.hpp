#include <opencv2/opencv.hpp>

void showScaledFixed(std::string name, cv::Mat mat);
cv::Mat scaleImage(cv::Mat image, float scale);
void drawPoints(const std::vector<cv::Point2d>& points, cv::Mat outImg, cv::Scalar color);
cv::Mat rotateImage(cv::Mat image, int angle);