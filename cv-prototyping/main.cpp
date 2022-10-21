#include <iostream>

#include <opencv2/opencv.hpp>
//#include <compare>
//#include <concepts>
//#include <span>

#include "headers/board_coords.hpp"
#include "headers/board_detection.hpp"
#include "headers/image_correction.hpp"
#include "headers/utility_opencv.hpp"



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
	cv::Mat warped; //, warpedMasked;
	cv::warpPerspective(src, warped, corr, {1000, 866});

	// zakomentowane, bo nie mam pliku catan-mask.png
	/*cv::Mat mask = cv::imread("resources/catan-mask.png", CV_8U);
	cv::resize(mask, mask, warped.size());

	cv::copyTo(warped, warpedMasked, {});*/

	auto coords = generateFieldCoords(2);
	char buf[10];
	for(auto& c: coords) {
		//char buf[4096];
		sprintf(buf, "(%d,%d,%d)", c.x, c.y, c.z);
		auto pnt = cv::Point2d{500,433} + 150*hexToCoord(c);
		cv::putText(warped, buf, pnt, cv::FONT_HERSHEY_PLAIN, 1, {255,255,255});
	}

	drawPoints(generateFieldPositions({500,433}, 150), warped, {255,255,180});
	drawPoints(generateCrossingPositions({500,433}, 150), warped, {255,0,0});
	

	showScaled("Source", src);
	showScaled("CrCb", crcb);
	showScaled("sqDiff(CrCb, sea color)", sq);
	showScaled("Threshold", thres);
	cv::imshow("Warped", warped);

	cv::waitKey();
}
