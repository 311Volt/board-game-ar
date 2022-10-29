#include <opencv2/opencv.hpp>


cv::Mat copyImageAreaContainingVertex(cv::Mat& image, cv::Point2d mappedVtx);

cv::Mat findAMaskForElementsOnBoard(cv::Mat emptyBoard, cv::Mat boardWithPawns);
cv::Mat findAMaskForElementsOnBoardHSV(cv::Mat emptyBoard, cv::Mat boardWithPawns);
cv::Mat findAMaskForElementsOnBoardKernel(cv::Mat emptyBoard, cv::Mat boardWithPawns, int kernel);
cv::Mat findAMaskForElementsOnBoardKernelHSV(cv::Mat emptyBoard, cv::Mat boardWithPawns, int kernel);

cv::Mat convertToHS(cv::Mat image);
cv::Mat findAMaskForWhitePawns(cv::Mat boardWithPawns);