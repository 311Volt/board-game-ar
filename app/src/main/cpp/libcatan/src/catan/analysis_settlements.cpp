#include <catan.hpp>

static constexpr float INV_255 = 1.0f / 255.0f;

float rating(float x)
{
    return 0.5f - std::atan(4.0f*x - 5) / 3.141591f;
}


enum class BuildingColor: char {
	Blue=0, Red=1, Orange=2, None=3, Unknown=4
};

cv::Vec3b BuildingColors1Hot[4] = {
	{255,0,0}, {0,255,0}, {0,0,255}, {0,0,0}
};

class MinDistThresLookupTable {

	std::vector<BuildingColor> data;
	std::function<BuildingColor(cv::Vec3b)> fn;

public:
	MinDistThresLookupTable(std::function<BuildingColor(cv::Vec3b)> fn)
		: fn(fn)
	{
		data = std::vector<BuildingColor>(32*32*32, BuildingColor::Unknown);
	}

	BuildingColor operator()(cv::Vec3b v)
	{
		int ab = v[0] >> 3;
		int ag = v[1] >> 3;
		int ar = v[2] >> 3;
		int addr = ab + (ag<<5) + (ar<<10);
		if(data[addr] == BuildingColor::Unknown) {
			data[addr] = fn(v);
		}
		return data[addr];
	}
};

std::vector<cv::Vec3b> CreateSpectrum(cv::Mat inImage, cv::Vec3b maskColor, int numBins)
{
	std::vector<cv::Vec3b> allPixels;
	std::vector<std::vector<cv::Vec3b>> bins(numBins);
	std::vector<cv::Vec3b> finalAverages(numBins);

	for(int y=0; y<inImage.rows; y++) {
		cv::Vec3b* row = inImage.ptr<cv::Vec3b>(y);
		for(int x=0; x<inImage.cols; x++) {
			if(row[x] != maskColor) {
				allPixels.push_back(row[x]);
			}
		}
	}

	static const auto lum = cvutil::SumOfChannels;
	static const auto lumComp = [](const cv::Vec3b& a, const cv::Vec3b& b){
		return lum(a) < lum(b);
	};


	int minLum = lum(*std::min_element(allPixels.begin(), allPixels.end(), lumComp));
	int maxLum = lum(*std::max_element(allPixels.begin(), allPixels.end(), lumComp));

	for(auto pixel: allPixels) {
		int pxLum = lum(pixel);
		int pxBin = (numBins * (pxLum - minLum)) / (1+maxLum - minLum);
		bins.at(pxBin).push_back(pixel);
	}

	for(int i=0; i<numBins; i++) {
		cv::Vec3f sum {0,0,0};
		for(auto pixel: bins[i]) {
			sum += cv::Vec3f(pixel);
		}
		finalAverages[i] = sum / std::max<int>(1U, bins[i].size());
	}
	cv::Vec3b lastNonZero;
	for(int i=0; i<numBins; i++) {
		if(finalAverages[i] != cv::Vec3b{0,0,0})
			lastNonZero = finalAverages[i];
		else
			finalAverages[i] = lastNonZero;
	}

	return finalAverages;
}

BuildingColor PlayerStructureTest(cv::Vec3b pixel)
{
	static std::vector<std::vector<cv::Vec3b>> spectrum = {
		CreateSpectrum(ctn::GetBitmapResource("element_blue"), {0,0,0}, 10),
		CreateSpectrum(ctn::GetBitmapResource("element_red"), {0,0,0}, 10),
		CreateSpectrum(ctn::GetBitmapResource("element_orange"), {0,0,0}, 10)
	};

	float minDist = 10e8;
	BuildingColor result = BuildingColor::None;

	for(int i=0; i<spectrum.size(); i++) {
		for(auto ref: spectrum[i]) {
			auto dist = cvmath::SquareDist(pixel, ref);
			if(dist < minDist) {
				result = static_cast<BuildingColor>(i);
				minDist = dist;
			}
		}
	}
	return minDist > 600 ? BuildingColor::None : result;
}

void ErosionDilationCycle(cv::Mat& img)
{
	auto elem = cv::getStructuringElement(cv::MORPH_ELLIPSE, {3,3});
	
	cv::erode(img, img, elem);
	cv::dilate(img, img, elem);
}

cv::Mat CreateStructureMask(const cv::Mat& input, cv::Vec2f relativeCenter)
{
	cv::Mat output = input.clone();
	cv::Vec2f refPoint = cv::Vec2f(input.cols/2.f, input.rows/2.f) - relativeCenter;
	static MinDistThresLookupTable lut(PlayerStructureTest);

	for(int y=0; y<input.rows; y++) {
		const cv::Vec3b* inRow = input.ptr<cv::Vec3b>(y);
		cv::Vec3b* outRow = output.ptr<cv::Vec3b>(y);

		for(int x=0; x<input.cols; x++) {
			cv::Vec2f position (x, y);
			float sqDist = cvmath::SquareDist({refPoint[0], refPoint[1], 0}, {position[0], position[1], 0});
			if(
				sqDist > 110000
				&& inRow[x][0] > inRow[x][1] && inRow[x][0] > inRow[x][2]
				&& cvutil::SumOfChannels(inRow[x]) > 250
			) {
				outRow[x] = {};
				continue;
			}
			auto colorResult = static_cast<char>(lut(inRow[x]));

			if(colorResult >= 3 || sqDist > 150000) {
				outRow[x] = {};
			} else if(colorResult < 3) {
				outRow[x] = BuildingColors1Hot[colorResult];
			} else {
				outRow[x] = {};
			}
		}
	}

	for(int i=0; i<5; i++) {
		ErosionDilationCycle(output);
	}
	cv::medianBlur(output, output, 5);

	return output;
}

std::array<float, 3> RateCornerMask(cv::Mat corner)
{
	cv::Vec3f result = 0;
	cv::Vec2f center = {corner.cols/2.f, corner.rows/2.f};
	for(int y=0; y<corner.rows; y++) {
		cv::Vec3b* inRow = corner.ptr<cv::Vec3b>(y);
		for(int x=0; x<corner.cols; x++) {
			cv::Vec2f position(x, y);
			float distFromCenter = cv::norm(position - center);
			result += cv::Vec3f(inRow[x]) * (1.0f / (1.0f + distFromCenter)) / 255.0f;
		}
	}
	return {result[0], result[1], result[2]};
}

std::vector<cv::Point> GetLargestContour(cv::Mat mask)
{
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(mask, contours, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
	if(contours.size() == 0) {
		throw std::runtime_error("error: settlement detected but mask image is empty");
	}
	return *std::max_element(contours.begin(), contours.end(), [](auto&& a, auto&& b) {
		return cv::contourArea(a) < cv::contourArea(b);
	});
}

std::vector<cv::Point2d> SmoothContour(const std::vector<cv::Point>& inputContour)
{
	cv::Mat mat = cv::Mat::zeros({1, (int)inputContour.size()}, CV_64FC2);
	for(int i=0; i<mat.rows; i++)
		mat.at<cv::Point2d>(i) = inputContour[i];
	cv::Mat output;
	cv::GaussianBlur(mat, output, {1, 21}, 12);
	return output;
}

float AngleDeltaHarmonicMean(const std::vector<cv::Point2d>& smoothedContour)
{
	int n = 1;
	double recSum = 10e-9;
	static constexpr double r2d = 180.0/3.141592;

	for(int i=0; i<smoothedContour.size() - 2; i++) {
		auto p1 = smoothedContour[i];
		auto p2 = smoothedContour[i+1];
		auto p3 = smoothedContour[i+2];
		
		auto diff1 = p2 - p1;
		auto diff2 = p3 - p2;

		float a1 = std::atan2(diff1.y, diff1.x) * r2d;
		float a2 = std::atan2(diff2.y, diff2.x) * r2d;

		float absAngleDelta = std::abs(a1 - a2);
		if(absAngleDelta < 0.03) {
			continue;
		}

		absAngleDelta = std::min({absAngleDelta, std::abs(180.0f - absAngleDelta), std::abs(180.0f + absAngleDelta)});
		recSum += 1.0 / std::max<double>(10e-9, absAngleDelta);
		n++;
	}

	return n / recSum;
}

ctn::SettlementType DetermineSettlementType(cv::Mat corner, cv::Mat mask)
{
	mask = cvutil::Convert(mask, cv::COLOR_BGR2GRAY);
	auto contour = SmoothContour(GetLargestContour(mask));

	auto hm = AngleDeltaHarmonicMean(contour);

	return hm > 0.9 ? ctn::SettlementType::City : ctn::SettlementType::Settlement;
}

std::map<ctn::VertexCoord, ctn::Settlement> ctn::FindSettlements(const BoardIR& boardIR)
{
	std::map<ctn::VertexCoord, ctn::Settlement> result;

	ScreenCoordMapper mapper({.center={500,433}, .size=150});

	for(const auto& [coord, corner]: boardIR.corners) {
		auto mask = CreateStructureMask(corner, cv::Vec2d(mapper(coord)) - cv::Vec2d(500,433));

		auto rating = RateCornerMask(mask);
		auto maxRatingIter = std::max_element(rating.begin(), rating.end());
		int maxRatingIdx = maxRatingIter - rating.begin();

		if(*maxRatingIter > 80) {
			PlayerColor colors[3] = {PlayerColor::Blue, PlayerColor::Red, PlayerColor::Orange};

			result[coord] = {
				.type=DetermineSettlementType(corner, mask), 
				.color=colors[maxRatingIdx]
			};
		}
	}

	return result;
}

std::map<ctn::EdgeCoord, ctn::Road> ctn::FindRoads(const BoardIR& boardIR)
{
	std::map<ctn::EdgeCoord, ctn::Road> result;

	ScreenCoordMapper mapper({.center={500,433}, .size=150});
	for(const auto& [coord, edge]: boardIR.edges) {
		auto mask = CreateStructureMask(edge, cv::Vec2d(mapper(coord)) - cv::Vec2d(500,433));

		auto rating = RateCornerMask(mask);
		auto maxRatingIter = std::max_element(rating.begin(), rating.end());
		int maxRatingIdx = maxRatingIter - rating.begin();

		if(*maxRatingIter > 30) {
			PlayerColor colors[3] = {PlayerColor::Blue, PlayerColor::Red, PlayerColor::Orange};
			result[coord] = {
				.color = colors[maxRatingIdx]
			};
		}
	}
	return result;
}
