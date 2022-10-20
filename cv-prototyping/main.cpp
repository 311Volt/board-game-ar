#include <iostream>

#include <opencv2/opencv.hpp>
#include <compare>
#include <numbers>
#include <concepts>
#include <span>

//sea: 38 110 160 (CrCb: 89, 163)

inline const cv::Vec3f SEA_COLOR_YCBCR_6500K = {0, 89, 163};
inline const cv::Vec3f SEA_COLOR_YCBCR_3400K = {0, 126, 132};

struct HexCoordComparer {
	bool operator()(const cv::Point3i& a, const cv::Point3i& b) const
	{
		return std::make_tuple(a.x,a.y,a.z) < std::make_tuple(b.x,b.y,b.z);
	}
};

cv::Point2d hexToCoord(cv::Point3i hex)
{
	static constexpr double M[] = {1, 0.5, 0, -std::numbers::sqrt3/2.0};
	return {
		hex.x*M[0] + hex.y*M[1],
		hex.x*M[2] + hex.y*M[3]
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
	return {std::cos(theta), -std::sin(theta)};
}

std::vector<cv::Point3i> generateFieldCoords(int maxDepth)
{
	std::vector<cv::Point3i> result;
	for(int i=-maxDepth; i<=maxDepth; i++) {
		int lo = std::max(-maxDepth, -maxDepth-i);
		int hi = std::min(maxDepth, maxDepth-i);
		for(int j=lo; j<=hi; j++) {
			result.push_back({i, j, -i-j});
		}
	}
	return result;
}

std::vector<cv::Point3i> generateCrossingCoords()
{
	auto fc = generateFieldCoords(3);
	std::vector<cv::Point3i> result;
	for(const auto& p: fc) {
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
	for(auto& k: coords) {
		result.push_back(center + size*hexToCoord(k));
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
	for(auto& k: coords) {
		if(k.x < 3 && k.y < 3 && k.z > -3) {
			result.push_back(center + size*(hexToCoord(k) + r*cis(30_deg)));
		}

		if(k.x > -3 && k.y < 3 && k.z > -3) {
			result.push_back(center + size*(hexToCoord(k) + r*cis(90_deg)));
		}
	}
	printf("result size = %d\n", (int)result.size());
	return result;
}

cv::Mat convertToCrCb(cv::Mat image)
{
	cv::Mat ycrcb;
	cv::cvtColor(image, ycrcb, cv::COLOR_BGR2YCrCb);
	cv::Mat ch[3];
	cv::split(ycrcb, ch);
	ch[0] = cv::Mat::zeros(ch[0].rows, ch[0].cols, CV_8UC1);
	cv::merge(ch, 3, ycrcb);
	return ycrcb;
}

cv::Mat squareDist(cv::Mat source, cv::Vec3f vec)
{
	cv::Mat out = cv::Mat::zeros(source.rows, source.cols, CV_8UC1);
	
	for(int y=0; y<source.rows; y++) {
		uint8_t* outRow = out.ptr<uint8_t>(y);
		cv::Vec3b* inRow = source.ptr<cv::Vec3b>(y);
		for(int x=0; x<source.cols; x++) {
			cv::Vec3f diff = cv::Vec3f(inRow[x]) - vec;
			outRow[x] = cv::norm(diff);
		}
	}

	return out;
}

cv::Mat getPerspectiveCorrectionMatrix(const std::vector<cv::Point>& points)
{
	std::vector<cv::Point> dstPoints = {
		{0, 433},
		{250, 866},
		{750, 866},
		{1000, 433},
		{750, 0},
		{250, 0}
	};
	return cv::findHomography(points, dstPoints);
}

void showScaled(std::string name, cv::Mat mat)
{
	cv::namedWindow(name, cv::WINDOW_NORMAL);
	cv::imshow(name, mat);
	cv::resizeWindow(name, 640, 480);
}

std::vector<cv::Point> findBoardVertices(cv::Mat thres)
{
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Point> hex;
	cv::findContours(thres, contours, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

	auto it = std::max_element(contours.begin(), contours.end(), [](auto&& a, auto&& b) {
		return cv::contourArea(a) < cv::contourArea(b);
	});

	int index = it - contours.begin();

	auto factor = 0.03;
	cv::approxPolyDP(contours[index], hex, cv::arcLength(contours[index], true) * factor, true);
	return hex;
}

void drawPoints(const std::vector<cv::Point2d>& points, cv::Mat outImg, cv::Scalar color)
{
	for(const auto& p: points) {
		cv::circle(outImg, p, 10, color, -1);
	}
}

int main()
{
	auto src = cv::imread("resources/sampleGlare2.jpg");
	cv::Mat crcb = convertToCrCb(src);
	cv::Mat sq = squareDist(crcb, SEA_COLOR_YCBCR_6500K);
	//cv::Mat sq = squareDist(crcb, SEA_COLOR_YCBCR_3400K);
	
	cv::Mat thres;
	cv::threshold(sq, thres, 20, 255, cv::THRESH_BINARY_INV);

	std::vector<std::vector<cv::Point>> hex = {findBoardVertices(thres)};
	cv::drawContours(src, hex, 0, {255,0,255}, 7);
	
	cv::Mat corr = getPerspectiveCorrectionMatrix(hex[0]);
	cv::Mat warped, warpedMasked;
	cv::warpPerspective(src, warped, corr, {1000, 866});

	cv::Mat mask = cv::imread("resources/catan-mask.png", CV_8U);
	cv::resize(mask, mask, warped.size());

	cv::copyTo(warped, warpedMasked, {});

	auto coords = generateFieldCoords(3);
	for(auto& c: coords) {
		char buf[4096];
		sprintf(buf, "(%d,%d,%d)", c.x, c.y, c.z);
		auto pnt = cv::Point2d{500,433} + 150*hexToCoord(c);
		cv::putText(warpedMasked, buf, pnt, cv::FONT_HERSHEY_PLAIN, 1, {255,255,255});
	}

	drawPoints(generateFieldPositions({500,433}, 150), warpedMasked, {255,255,180});
	drawPoints(generateCrossingPositions({500,433}, 150), warpedMasked, {255,0,0});
	

	showScaled("Source", src);
	showScaled("CrCb", crcb);
	showScaled("sqDiff(CrCb, sea color)", sq);
	showScaled("Threshold", thres);
	cv::imshow("Warped", warpedMasked);

	cv::waitKey();
}
