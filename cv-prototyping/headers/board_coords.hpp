#include <opencv2/opencv.hpp>
#include <numbers>


struct HexCoordComparer {
	bool operator()(const cv::Point3i& a, const cv::Point3i& b) const
	{
		return std::make_tuple(a.x, a.y, a.z) < std::make_tuple(b.x, b.y, b.z);
	}
};

cv::Point2d hexToCoord(cv::Point3i hex);
cv::Point2d cis(double theta);

long double operator""_deg(long double x);
long double operator""_deg(unsigned long long x);

std::vector<cv::Point3i> generateFieldCoords(int maxDepth);
std::vector<cv::Point3i> generateCrossingCoords();

std::vector<cv::Point2d> generateFieldPositions(cv::Point2d center, double size);
std::vector<cv::Point2d> generateCrossingPositions(cv::Point2d center, double size);