#include <opencv2/opencv.hpp>


cv::Mat copyImageAreaContainingVertex(cv::Mat& image, cv::Point2d mappedVtx);
cv::Mat findAMaskForElementsOnBoard(cv::Mat emptyBoard, cv::Mat boardWithPawns);