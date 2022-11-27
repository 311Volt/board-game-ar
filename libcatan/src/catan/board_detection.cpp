#include <catan.hpp>

cv::Mat squareDist(cv::Mat source, cv::Vec3f vec)
{
	cv::Mat out = cv::Mat::zeros(source.rows, source.cols, CV_8UC1);

	for (int y = 0; y < source.rows; y++) {
		uint8_t* outRow = out.ptr<uint8_t>(y);
		cv::Vec3b* inRow = source.ptr<cv::Vec3b>(y);
		for (int x = 0; x < source.cols; x++) {
			cv::Vec3f diff = cv::Vec3f(inRow[x]) - vec;
			outRow[x] = cv::norm(diff);
		}
	}

	return out;
}

std::vector<cv::Point> approxPoly(const std::vector<cv::Point>& contour, float epsilon)
{
	std::vector<cv::Point> dpOutput;
	double len = epsilon * cv::arcLength(contour, true);
	cv::approxPolyDP(contour, dpOutput, len, true);
	return dpOutput;
}

std::optional<std::vector<cv::Point>> tryApproxHexagonBin(
	const std::vector<cv::Point>& contour, 
	float epsilonLow = 0.006, 
	float epsilonHigh = 0.02, 
	int maxIters = 7
)
{
	for(int i=0; i<maxIters; i++)
	{
		float mid = (epsilonLow + epsilonHigh) / 2.0;
		auto poly = approxPoly(contour, mid);

		if(poly.size() == 6) {
			return poly;
		} else if(poly.size() > 6) {
			epsilonLow = mid;
		} else if(poly.size() < 6) {
			epsilonHigh = mid;
		}
	}
	return std::nullopt;
}


std::optional<std::vector<cv::Point>> findBoardVertices(cv::Mat thres)
{
	std::vector<std::vector<cv::Point>> contours;

	cv::findContours(thres, contours, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

	auto it = std::max_element(contours.begin(), contours.end(), [](auto&& a, auto&& b) {
		return cv::contourArea(a) < cv::contourArea(b);
	});

	return tryApproxHexagonBin(*it);
}


CatanBoardDetector::CatanBoardDetector(cv::Vec3f seaColor)
	: seaColor(seaColor)
{

}

std::optional<cv::Mat> CatanBoardDetector::findBoard(cv::Mat photo)
{
	cv::Mat crcb = cvutil::ConvertToCrCb(photo);
	cv::Mat sq = squareDist(crcb, seaColor);
	
	cv::Mat thres = NEW_MAT(tmp) {cv::threshold(sq, tmp, 20, 255, cv::THRESH_BINARY_INV);};
	
	auto boardVtxs = findBoardVertices(thres);
	if(!boardVtxs) {
		return std::nullopt;
	}

	cv::Mat corr = ctn::GetBoardPerspectiveCorrectionMatrix(boardVtxs.value());
	cv::Mat warped = NEW_MAT(tmp) {cv::warpPerspective(photo, warped, corr, {1000, 866});};

	cv::Mat warpMtx = ctn::FindFineAlignment(ctn::CreateDarkEdgeMask(warped), ctn::GenerateIdealEdgeMask());
	cv::Mat warped1;
	cv::warpAffine(warped, warped1, warpMtx, warped.size(), cv::INTER_LINEAR + cv::WARP_INVERSE_MAP);
	warped = warped1;

	return warped;
}
