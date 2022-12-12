#include <catan.hpp>
#include <fmt/format.h>

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

cv::Vec3f getRoadColor(cv::Mat warped, cv::Mat thresWarped)
{
	if(warped.cols != thresWarped.cols || warped.rows != thresWarped.rows) {
		throw std::runtime_error("non-matching dimensions");
	}
	if(warped.depth() != CV_32F) {
		warped = cvutil::ToFloat(warped);
	}

	std::vector<float> vb(1), vg(1), vr(1);

	for(int y=0; y<warped.rows; y+=4) {
		cv::Vec3f* imgRow = warped.ptr<cv::Vec3f>(y);
		uint8_t* thresRow = thresWarped.ptr<uint8_t>(y);
		for(int x=0; x<warped.cols; x+=4) {
			if(thresRow[x] > 200) {
				vb.push_back(imgRow[x][0]);
				vg.push_back(imgRow[x][1]);
				vr.push_back(imgRow[x][2]);
			}
		}
	}

	std::sort(vb.begin(), vb.end());
	std::sort(vg.begin(), vg.end());
	std::sort(vr.begin(), vr.end());
	
	return {vb[vb.size()/2] * 1.0f, vg[vg.size()/2] * 1.0f, vr[vr.size()/2] * 1.0f};
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
	
	fmt::print("wanted road color: {} {} {}\n", wantedRoadColor[0], wantedRoadColor[1], wantedRoadColor[2]);
	fmt::print("actual road color: {} {} {}\n", actualRoadColor[0], actualRoadColor[1], actualRoadColor[2]);
		

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


std::optional<cv::Mat> CatanBoardDetector::findBoard(cv::Mat photo)
{
	auto photoMedian = NEW_MAT(tmp) {cv::medianBlur(photo, tmp, 5);};
	cv::Mat crcb = cvutil::ConvertToCrCb(photoMedian);
	cv::Mat sq = squareDist(crcb, seaColor);
	
	cv::Mat thres = NEW_MAT(tmp) {cv::threshold(sq, tmp, 20, 255, cv::THRESH_BINARY_INV);};

	auto boardVtxs = findBoardVertices(thres);
	if(!boardVtxs) {
		return std::nullopt;
	}

	cv::Mat corr = ctn::GetBoardPerspectiveCorrectionMatrix(boardVtxs.value());
	cv::Mat warped = NEW_MAT(tmp) {cv::warpPerspective(photo, tmp, corr, {1000, 866});};
	cv::Mat thresWarped = NEW_MAT(tmp) {cv::warpPerspective(thres, tmp, corr, {1000, 866});};
	
	auto roadMask = ctn::GenerateIdealEdgeMask({255,255,255}, 15);
	cv::Vec3f roadColor = getRoadColor(warped, roadMask);

#ifdef CATAN_APPLY_FINE_ALIGNMENT
	cv::Mat warpMtx = ctn::FindFineAlignment(ctn::CreateDarkEdgeMask(warped), ctn::GenerateIdealEdgeMask());
	cv::Mat warped1;
	cv::warpAffine(warped, warped1, warpMtx, warped.size(), cv::INTER_LINEAR + cv::WARP_INVERSE_MAP);
	warped = warped1;
#endif

	return applyColorCorrection(warped, roadColor, {0.439f, 0.603f, 0.627f});
}
