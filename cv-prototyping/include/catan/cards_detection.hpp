#include <opencv2/opencv.hpp>


cv::Mat scaleImage(cv::Mat image, float scale);


std::vector<cv::Mat> detectCards(cv::Mat image);
std::vector<cv::Mat> detectCardsPlasticVer(cv::Mat image);