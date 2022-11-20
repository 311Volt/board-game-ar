#include <opencv2/opencv.hpp>

#include <catan/analysis_settlements.hpp>
#include <catan/utility_opencv.hpp>
#include <catan/common_math.hpp>

static constexpr float INV_255 = 1.0f / 255.0f;

float rating(float x)
{
    return 0.5f - std::atan(4.0f*x - 5) / 3.141591f;
}


class MinDistThresLookupTable {

	//TODO BSP of the first 9 address bits to quickly discard large areas

	enum {
		False = 0,
		True = 1,
		Unknown = 2
	};

	std::vector<char> data;
	std::function<bool(cv::Vec3b)> fn;

public:
	MinDistThresLookupTable(std::function<bool(cv::Vec3b)> fn)
		: fn(fn)
	{
		data = std::vector<char>(32*32*32, Unknown);
	}

	bool operator()(cv::Vec3b v)
	{
		int ab = v[0] >> 3;
		int ag = v[1] >> 3;
		int ar = v[2] >> 3;
		int addr = ab + (ag<<5) + (ar<<10);
		if(data[addr] == Unknown) {
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

bool PlayerStructureTest(cv::Vec3b pixel)
{
	static auto blueSpectrum = CreateSpectrum(cv::imread("resources/elements/blue.png"), {0,0,0}, 10);
	static auto redSpectrum = CreateSpectrum(cv::imread("resources/elements/red.png"), {0,0,0}, 10);
	static auto orangeSpectrum = CreateSpectrum(cv::imread("resources/elements/orange.png"), {0,0,0}, 10);

	float minDist = 10e8;

	for(auto ref: blueSpectrum) 
		minDist = std::min(minDist, cvmath::SquareDist(pixel, ref));
	for(auto ref: redSpectrum)  
		minDist = std::min(minDist, cvmath::SquareDist(pixel, ref));
	for(auto ref: orangeSpectrum) 
		minDist = std::min(minDist, cvmath::SquareDist(pixel, ref));
	
	return minDist > 600;
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

			if(lut(inRow[x]) || sqDist > 150000) {
				outRow[x] = {};
			} else {
				outRow[x] = {255, 255, 255};
			}
		}
	}

	for(int i=0; i<5; i++) {
		ErosionDilationCycle(output);
		
	}
	cv::medianBlur(output, output, 5);

	return output;
}

float RateCornerMask(cv::Mat corner)
{
	float result = 0;
	cv::Vec2f center = {corner.cols/2.f, corner.rows/2.f};
	for(int y=0; y<corner.rows; y++) {
		cv::Vec3b* inRow = corner.ptr<cv::Vec3b>(y);
		for(int x=0; x<corner.cols; x++) {
			if(inRow[x][0] == 0) {
				continue;
			}
			cv::Vec2f position(x, y);
			float distFromCenter = cv::norm(position - center);
			result += 1.0f / (1.0f + distFromCenter);
		}
	}
	return result;
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
	static cv::Vec3b sand = cvutil::YCrCbOf({127, 180, 181});

	static cv::Vec3b cityRed = cvutil::YCrCbOf({75, 0, 146});
	static cv::Vec3b cityBlue = cvutil::YCrCbOf({100, 52, 15});
	static cv::Vec3b cityOrange = cvutil::YCrCbOf({25, 136, 208});
	
	std::map<ctn::VertexCoord, ctn::Settlement> result;

	ScreenCoordMapper mapper({.center={500,433}, .size=150});

	for(const auto& [coord, corner]: boardIR.corners) {
		auto cornerYCrCb = cvutil::Convert(corner, cv::COLOR_BGR2YCrCb);
		auto mask = CreateStructureMask(corner, cv::Vec2d(mapper(coord)) - cv::Vec2d(500,433));

		
		//auto rating = rateChunk(BuildingInverseLikelihood(corner));

		if(RateCornerMask(mask) > 80) {
			cv::imshow(fmt::format("mask at {}", coord), mask);

			cv::Point center = {corner.cols/2, corner.rows/2};
			float distRed = cvmath::WeightedSquareDist<0, 100, 100>(cornerYCrCb.at<cv::Vec3b>(center), cityRed);
			float distBlue = cvmath::WeightedSquareDist<0, 100, 100>(cornerYCrCb.at<cv::Vec3b>(center), cityBlue);
			float distOrange = cvmath::WeightedSquareDist<0, 100, 100>(cornerYCrCb.at<cv::Vec3b>(center), cityOrange);

			float distances[3] = {distRed, distBlue, distOrange};
			PlayerColor colors[3] = {PlayerColor::Red, PlayerColor::Blue, PlayerColor::Orange};
			cv::Vec3b crcbColors[3] = {cityRed, cityBlue, cityOrange};

			int idx = std::min_element(distances, distances+3) - distances;

			//auto isolated = IsolateBuilding(corner, crcbColors[idx]);
			//cv::imshow(fmt::format("isolated building at {}", coord), isolated);

			result[coord] = {
				.type=DetermineSettlementType(corner, mask), 
				.color=colors[idx]
			};
		}
	}

	return result;
}
