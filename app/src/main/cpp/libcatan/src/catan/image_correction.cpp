#include <catan.hpp>

cv::Mat ctn::GetBoardPerspectiveCorrectionMatrix(const std::vector<cv::Point>& points)
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

cv::Mat ctn::GenerateIdealEdgeMask(cv::Scalar color, float thickness)
{
	cv::Mat output = cv::Mat::zeros({1000, 866}, CV_8UC1);

	ctn::ScreenCoordMapper mapper({.center = {500, 433}, .size = 150});
	auto edges = ctn::GenerateEdgeCoords();

	for(const auto& edge: edges) {
		auto vt = edge.asVertexCoordPair();
		
		auto p1 = mapper(vt.first);
		auto p2 = mapper(vt.second);
		cv::line(output, p1, p2, color, thickness);
	}

	return output;
}


cv::Mat ctn::FindFineAlignment(cv::Mat actualEdges, cv::Mat idealEdges)
{
	static constexpr int SCALE_FACTOR = 4;

	cv::Mat warpMtx = cv::Mat::eye(2, 3, CV_32FC1);
	cv::TermCriteria criteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 1e-7);

	cv::Mat actualEdgesScaled = NEW_MAT(tmp) {cv::resize(actualEdges, tmp, actualEdges.size()/SCALE_FACTOR);};
	cv::Mat idealEdgesScaled = NEW_MAT(tmp) {cv::resize(idealEdges, tmp, idealEdges.size()/SCALE_FACTOR);};

    int sum = cv::sum(actualEdgesScaled)[0];
	if(sum > 40000) {
		cv::findTransformECC(idealEdgesScaled, actualEdgesScaled, warpMtx, cv::MOTION_EUCLIDEAN, criteria);
	}

	double tDet = cv::determinant(warpMtx(cv::Rect{0, 0, 2, 2}));
	double tNorm = cv::norm(warpMtx(cv::Rect{2,0,1,2}));
	warpMtx.at<float>(0, 2) *= SCALE_FACTOR;
	warpMtx.at<float>(1, 2) *= SCALE_FACTOR;

	//if either det(transform) or norm(translation) is suspiciously high, fall back to identity
	if(std::abs(1.0 - tDet) > 0.03 || tNorm > 30) {
		return cv::Mat::eye(2, 3, CV_32FC1);
	}

	return warpMtx;
}

cv::Mat ctn::CreateDarkEdgeMask(cv::Mat input)
{
	//step 1: mark pixels that are substantially darker than their neighborhood
	cv::Mat median = NEW_MAT(tmp) {cv::medianBlur(input, tmp, 9);};
	cv::Mat thres = cvmath::TransformBin<cv::Vec3b, uint8_t>(input, median, [](cv::Vec3b pxIn, cv::Vec3b pxMed) {
		const auto lum = cvutil::SumOfChannels;
		if(lum(pxIn) < lum(pxMed) - 60) {
			return uint8_t(255);
		}
		return uint8_t(0);
	});

	//cv::imshow("initial thres", thres.clone());

	//step 2: clean up the image (remove all islands with <20px area)
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(thres, contours, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

	for(int i=0; i<contours.size(); i++) {
		if(cv::contourArea(contours[i]) < 20) {
			cv::drawContours(thres, contours, i, {0,0,0}, -2);
		}
	}

	//cv::imshow("processed thres", thres);

	return thres;
}