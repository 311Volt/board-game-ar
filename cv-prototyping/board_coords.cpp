#include <opencv2/opencv.hpp>
#include <numbers>


cv::Point2d hexToCoord(cv::Point3i hex)
{
	static constexpr double M[] = { 1, 0.5, 0, -std::numbers::sqrt3 / 2.0 };
	return {
		hex.x * M[0] + hex.y * M[1],
		hex.x * M[2] + hex.y * M[3]
	};
}


long double operator""_deg(long double x)
{
	return x * std::numbers::pi / 180.0;
}
long double operator""_deg(unsigned long long x)
{
	return x * std::numbers::pi / 180.0;
}

cv::Point2d cis(double theta)
{
	return { std::cos(theta), -std::sin(theta) };
}

std::vector<cv::Point3i> generateFieldCoords(int maxDepth)
{
	std::vector<cv::Point3i> result;
	for (int i = -maxDepth; i <= maxDepth; i++) {
		int lo = std::max(-maxDepth, -maxDepth - i);
		int hi = std::min(maxDepth, maxDepth - i);
		for (int j = lo; j <= hi; j++) {
			result.push_back({ i, j, -i - j });
		}
	}
	return result;
}

std::vector<cv::Point3i> generateCrossingCoords()
{
	auto fc = generateFieldCoords(3);
	std::vector<cv::Point3i> result;
	for (const auto& p : fc) {
		//if(p.z > 2 || p.y > 2 || p.x > 2)
			//continue;
		result.push_back(p);
	}
	return result;
}

std::vector<cv::Point2d> generateFieldPositions(cv::Point2d center, double size)
{
	auto coords = generateFieldCoords(2);

	std::vector<cv::Point2d> result;
	result.reserve(coords.size());
	for (auto& k : coords) {
		result.push_back(center + size * hexToCoord(k));
	}
	printf("result size = %d\n", (int)result.size());
	return result;
}

std::vector<cv::Point2d> generateCrossingPositions(cv::Point2d center, double size)
{
	auto coords = generateCrossingCoords();

	std::vector<cv::Point2d> result;
	result.reserve(coords.size());
	static constexpr double r = 1.0 / std::numbers::sqrt3;
	for (auto& k : coords) {
		if (k.x < 3 && k.y < 3 && k.z > -3) {
			result.push_back(center + size * (hexToCoord(k) + r * cis(30_deg)));
		}

		if (k.x > -3 && k.y < 3 && k.z > -3) {
			result.push_back(center + size * (hexToCoord(k) + r * cis(90_deg)));
		}
	}
	printf("result size = %d\n", (int)result.size());
	return result;
}