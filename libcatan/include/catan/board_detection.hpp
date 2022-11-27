#ifndef INCLUDE_CATAN_BOARD_DETECTION
#define INCLUDE_CATAN_BOARD_DETECTION

#include <opencv2/opencv.hpp>
#include <optional>

//sea: 38 110 160 (CrCb: 89, 163)

inline const cv::Vec3f SEA_COLOR_YCBCR_6500K = { 0, 89, 163 };
inline const cv::Vec3f SEA_COLOR_YCBCR_3400K = { 0, 126, 132 };

class CatanBoardDetector {
public:
	CatanBoardDetector(cv::Vec3f seaColor);

	std::optional<cv::Mat> findBoard(cv::Mat photo);
private:
	cv::Vec3f seaColor;
};


#endif /* INCLUDE_CATAN_BOARD_DETECTION */
