#include <opencv2/opencv.hpp>
#include <catan/cards_detection.hpp>


std::vector<cv::Mat> readTemplateCards();

std::vector<std::vector<cv::Mat>> cutOutCardHeadings(std::vector<cv::Mat> cards, float headingAreaCoeff);