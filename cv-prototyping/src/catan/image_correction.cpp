#include <catan/image_correction.hpp>
#include <catan/utility_opencv.hpp>
#include <catan/common_math.hpp>
#include <catan/board_coords.hpp>

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

cv::Mat GenerateReferenceEdgeThres()
{
	cv::Mat output = cv::Mat::zeros({1000, 866}, CV_8UC1);

	ctn::ScreenCoordMapper mapper({.center = {500, 433}, .size = 150});
	auto edges = ctn::GenerateEdgeCoords();

	for(const auto& edge: edges) {
		auto vt = edge.asVertexCoordPair();
		
		auto p1 = mapper(vt.first);
		auto p2 = mapper(vt.second);
		cv::line(output, p1, p2, {255,255,255});
	}

	return output;
}

cv::Mat FindAlignment(cv::Mat actualEdges, cv::Mat idealEdges)
{
	cv::Mat warpMtx = cv::Mat::eye(2, 3, CV_32FC1);
	cv::TermCriteria criteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 70, 1e-6);

	cv::findTransformECC(idealEdges, actualEdges, warpMtx, cv::MOTION_EUCLIDEAN, criteria);
	return warpMtx;
}

cv::Mat IsolateDarkEdges(cv::Mat input)
{
	cv::Mat median = NEW_MAT(tmp) {cv::medianBlur(input, tmp, 5);};
	cv::Mat thres = cvmath::TransformBin<cv::Vec3b, uint8_t>(input, median, [](cv::Vec3b pxIn, cv::Vec3b pxMed) {
		const auto lum = cvutil::SumOfChannels;
		if(lum(pxIn) < lum(pxMed) - 60) {
			return uint8_t(255);
		}
		return uint8_t(0);
	});

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(thres, contours, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

	for(int i=0; i<contours.size(); i++) {
		if(cv::contourArea(contours[i]) < 20) {
			cv::drawContours(thres, contours, i, {0,0,0}, -2);
		}
	}

	return thres;
}
