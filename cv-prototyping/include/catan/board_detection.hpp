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


cv::Mat convertToCrCb(cv::Mat image);
cv::Mat squareDist(cv::Mat source, cv::Vec3f vec);
std::vector<cv::Point> approxPoly(const std::vector<cv::Point>& contour, float epsilon);
std::optional<std::vector<cv::Point>> tryApproxHexagonBin(
	const std::vector<cv::Point>& contour,
	float epsilonLow = 0.006,
	float epsilonHigh = 0.02,
	int maxIters = 7
);
std::optional<std::vector<cv::Point>> findBoardVertices(cv::Mat thres);


cv::Mat findBlueBoardFrame(cv::Mat image);
cv::Mat findAMaskForBoard(cv::Mat image, std::vector<cv::Point> boardVerticies);
cv::Mat prepareBoard(cv::Mat image, std::vector<cv::Point> boardVerticies);