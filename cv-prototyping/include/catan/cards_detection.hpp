#include <opencv2/opencv.hpp>


cv::Mat scaleImage(cv::Mat image, float scale);
/*cv::Mat detectWhiteFields(cv::Mat image);
std::vector<std::vector<cv::Point>> filterContoursForBigQuadrangles(std::vector<std::vector<cv::Point>> contours);
cv::Mat maskAndCropOutCard(cv::Mat image, std::vector<cv::Point> c);*/


std::vector<cv::Mat> detectCards(cv::Mat image);