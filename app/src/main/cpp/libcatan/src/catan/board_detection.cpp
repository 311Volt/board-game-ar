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

	if(contours.size() == 0) {
		return std::nullopt;
	}

	auto it = std::max_element(contours.begin(), contours.end(), [](auto&& a, auto&& b) {
		return cv::contourArea(a) < cv::contourArea(b);
	});

	return tryApproxHexagonBin(*it);
}


CatanBoardDetector::CatanBoardDetector(cv::Vec3f seaColor)
	: seaColor(seaColor)
{

}


cv::Mat applyColorCorrection(cv::Mat image, cv::Vec3f actualRoadColor, cv::Vec3f wantedRoadColor)
{
	cv::Mat squaredWarped = cvmath::Transform<cv::Vec3f, cv::Vec3f>(
		cvutil::ToFloat(image), 
		[](cv::Vec3f px){return cv::Vec3f{px[0]*px[0], px[1]*px[1], px[2]*px[2]};}
	);
	//wantedRoadColor = wantedRoadColor * wantedRoadColor;
	//actualRoadColor = actualRoadColor * actualRoadColor;
	cv::multiply(wantedRoadColor, wantedRoadColor, wantedRoadColor);
	cv::multiply(actualRoadColor, actualRoadColor, actualRoadColor);
	
	//fmt::print("wanted road color: {} {} {}\n", wantedRoadColor[0], wantedRoadColor[1], wantedRoadColor[2]);
	//fmt::print("actual road color: {} {} {}\n", actualRoadColor[0], actualRoadColor[1], actualRoadColor[2]);
		

	cv::Vec3f scaleFactor = {
		wantedRoadColor[0] / actualRoadColor[0],
		wantedRoadColor[1] / actualRoadColor[1],
		wantedRoadColor[2] / actualRoadColor[2]
	};

	cvmath::Apply<cv::Vec3f>(squaredWarped, [scaleFactor](cv::Vec3f frag){
		return cv::Vec3f {
			frag[0] * scaleFactor[0],
			frag[1] * scaleFactor[1],
			frag[2] * scaleFactor[2]
		};
	});

	cvmath::Apply<cv::Vec3f>(squaredWarped, [scaleFactor](cv::Vec3f frag){
		return cv::Vec3f {std::sqrt(frag[0]), std::sqrt(frag[1]), std::sqrt(frag[2])};
	});



	return cvutil::ToByte(squaredWarped);
}

void beforeafter(cv::Mat warped, cv::Mat warped1)
{
	ctn::ScreenCoordMapper mapper({.center = {500, 433}, .size = 150});

	warped = warped.clone();
	warped1 = warped1.clone();

	for(auto& coord: ctn::GenerateCellCoords(2)) {
		cv::circle(warped, mapper(coord), 9, {0,0,0}, -1);
		cv::circle(warped1, mapper(coord), 9, {0,0,0}, -1);
		cv::circle(warped, mapper(coord), 6, {0,255,0}, -1);
		cv::circle(warped1, mapper(coord), 6, {0,255,0}, -1);
	}

	cv::imshow("before fine align", warped);
	cv::imshow("after fine align", warped1);
}

void discardOutOfBoundsPixels(cv::Mat& warped)
{
	for(int y=0; y<warped.rows; y++) {
		cv::Vec3b* row = warped.ptr<cv::Vec3b>(y);
		for(int x=0; x<warped.cols; x++) {
			double v1 = y - x*1.732 - 433; // y + = sqrt(3)*x - 433
			double v2 = y + x*1.732 - 433;
			double v3 = y - (x-1000)*1.732 - 433;
			double v4 = y + (x-1000)*1.732 - 433;
			
			if(v1 > 0 || v2 < 0 || v3 < 0 || v4 > 0) {
				row[x] = 0;
			}
		}
	}
}

std::optional<cv::Mat> CatanBoardDetector::findBoard(cv::Mat photo)
{
	int medianBlurRadius = 5*(photo.rows / 1200) + 7;
	medianBlurRadius -= (medianBlurRadius % 2) - 1;
	auto photoMedian = NEW_MAT(tmp) {cv::medianBlur(photo, tmp, medianBlurRadius);};
	cv::Mat crcb = cvutil::ConvertToCrCb(photoMedian);
	cv::Mat sq = squareDist(crcb, seaColor);
	
	cv::Mat thres = NEW_MAT(tmp) {cv::threshold(sq, tmp, 26, 255, cv::THRESH_BINARY_INV);};

	auto boardVtxs = findBoardVertices(thres);
	if(!boardVtxs) {
		return std::nullopt;
	}

	cv::Mat corr = ctn::GetBoardPerspectiveCorrectionMatrix(boardVtxs.value());
	cv::Mat warped = NEW_MAT(tmp) {cv::warpPerspective(photo, tmp, corr, {1000, 866});};
	discardOutOfBoundsPixels(warped);
	cv::Mat thresWarped = NEW_MAT(tmp) {cv::warpPerspective(thres, tmp, corr, {1000, 866});};

	// auto channels = cvutil::SplitBGR(warped);
	// for(auto& ch: channels)
	// 	cv::equalizeHist(ch, ch);
	// warped = cvutil::MergeBGR(channels);

	//auto roadMask = ctn::GenerateIdealEdgeMask({255,255,255}, 15);
	//cv::Vec3f roadColor = getRoadColor(warped, roadMask);

#ifndef ANDROID
#define CATAN_APPLY_FINE_ALIGNMENT
#endif

#ifdef CATAN_APPLY_FINE_ALIGNMENT
	cv::Mat warpMtx = ctn::FindFineAlignment(ctn::CreateDarkEdgeMask(warped), ctn::GenerateIdealEdgeMask({255,255,255}, 4));
	cv::Mat warped1;
	cv::warpAffine(warped, warped1, warpMtx, warped.size(), cv::INTER_LINEAR + cv::WARP_INVERSE_MAP);
	//beforeafter(warped, warped1);
	warped = warped1;
#endif

	return warped;
	//return applyColorCorrection(warped, roadColor, {0.439f, 0.603f, 0.627f});
}
