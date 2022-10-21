#include <opencv2/opencv.hpp>

//sea: 38 110 160 (CrCb: 89, 163)

inline const cv::Vec3f SEA_COLOR_YCBCR_6500K = { 0, 89, 163 };
inline const cv::Vec3f SEA_COLOR_YCBCR_3400K = { 0, 126, 132 };

cv::Mat convertToCrCb(cv::Mat image);
cv::Mat squareDist(cv::Mat source, cv::Vec3f vec);
std::vector<cv::Point> findBoardVertices(cv::Mat thres);