#include <iostream>

#include <opencv2/opencv.hpp>
//#include <compare>
//#include <concepts>
//#include <span>

#include <board_coords.hpp>
#include <board_detection.hpp>
#include <image_correction.hpp>
#include <utility_opencv.hpp>

#include <fmt/format.h>

#include <ranges>
#include <algorithm>

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
	cv::Mat warped;
	cv::warpPerspective(src, warped, corr, {1000, 866});

	ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});

	for(auto& c: GenerateFieldCoords(3)) {
		cv::putText(warped, fmt::format("({},{},{})", c.x, c.y, c.z), mapper(c), cv::FONT_HERSHEY_PLAIN, 1, {255,255,255});
	}

	drawPoints(mapper(GenerateFieldCoords(2)), warped, {255,255,180});
	drawPoints(mapper(GenerateVertexCoords()), warped, {255,0,0});
	
	showScaled("Source", src);
	showScaled("CrCb", crcb);
	showScaled("sqDiff(CrCb, sea color)", sq);
	showScaled("Threshold", thres);
	cv::imshow("Warped", warped);

	cv::waitKey();
}
