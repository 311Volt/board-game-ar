#include <opencv2/opencv.hpp>

#include <catan/analysis_settlements.hpp>
#include <catan/utility_opencv.hpp>
#include <catan/common_math.hpp>

static constexpr float INV_255 = 1.0f / 255.0f;

float rating(float x)
{
    return 0.5f - std::atan(4.0f*x - 5) / 3.141591f;
}

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

/*
 * transforms the input image so that darker pixels on the output
 * represent where a building is likely to be
 */
cv::Mat BuildingInverseLikelihood(const cv::Mat& warped)
{
	static constexpr float LUMINANCE_WEIGHT = 0.3f;

	//cv::Vec3b shore = ycrcbOf({192, 170, 109});
	cv::Vec3b sand = cvutil::YCrCbOf({139, 179, 178});
	cv::Vec3b gap = cvutil::YCrCbOf({27, 59, 64});

	//shore[0] *= LUMINANCE_WEIGHT;
	sand[0] *= LUMINANCE_WEIGHT;

	cv::Mat output = cv::Mat::zeros(warped.rows, warped.cols, warped.depth());

	cv::Mat warpedCrCb = NEW_MAT(tmp) {cv::cvtColor(warped, tmp, cv::COLOR_BGR2YCrCb);};

	for(int y=0; y<warped.rows; y++) {
		cv::Vec3b* inRow = warpedCrCb.ptr<cv::Vec3b>(y);
		uint8_t* outRow = output.ptr<uint8_t>(y);
		for(int x=0; x<warped.cols; x++) {
			//float k1 = rating(sqDistUC3(shore, inRow[x]) * 64.0f);

			//float k2 = rating(sqDistUC3(gap, inRow[x]) * 128.0f) * 0.7f;
			float k2 = 0;
			inRow[x][0] *= LUMINANCE_WEIGHT;
			
			float k1 = rating(cvmath::SquareDist(cv::Vec3f(sand) * INV_255, cv::Vec3f(inRow[x]) * INV_255) * 128.0f);
			
			outRow[x] = std::max(k1, k2) * 255.0f;
		}
	}
	return output;
}

cv::Mat MaskBuildingsAlternative(const cv::Mat& warped)
{
	auto blueSpectrum = CreateSpectrum(cv::imread("resources/elements/blue.png"), {0,0,0}, 20);
	auto redSpectrum = CreateSpectrum(cv::imread("resources/elements/red.png"), {0,0,0}, 20);
	auto orangeSpectrum = CreateSpectrum(cv::imread("resources/elements/orange.png"), {0,0,0}, 20);

	cv::Mat output = warped.clone();

	for(int y=0; y<warped.rows; y++) {
		const cv::Vec3b* inRow = warped.ptr<cv::Vec3b>(y);
		cv::Vec3b* outRow = output.ptr<cv::Vec3b>(y);

		for(int x=0; x<warped.cols; x++) {
			float minDist = 10e8;

			for(auto px: blueSpectrum) 
				minDist = std::min(minDist, cvmath::SquareDist(inRow[x], px));
			for(auto px: redSpectrum) 
				minDist = std::min(minDist, cvmath::SquareDist(inRow[x], px));
			for(auto px: orangeSpectrum) 
				minDist = std::min(minDist, cvmath::SquareDist(inRow[x], px));

			if(minDist > 300) {
				outRow[x] = {};
			}
		}
	}
	return output;
}

float rateChunk(cv::Mat chunk)
{
	cv::Vec2i center {chunk.cols/2, chunk.rows/2};
	float ret = 0;
	for(int y=0; y<chunk.rows; y++) {
		uint8_t* inRow = chunk.ptr<uint8_t>(y);
		for(int x=0; x<chunk.cols; x++) {
			if(cv::norm(cv::Vec2i{x,y} - center) < 10) {
				ret += inRow[x];
			}
		}
	}
	return ret;
}

cv::Mat IsolateBuilding(const cv::Mat& corner, cv::Vec3b targetColor)
{
	auto cornerYCrCb = cvutil::Convert(corner, cv::COLOR_BGR2YCrCb);
	return cvmath::TransformBin<cv::Vec3b, cv::Vec3b>(
		corner, cornerYCrCb, 
		[&targetColor](cv::Vec3b pixel, cv::Vec3b pixelYCrCb) {
			float val = cvmath::WeightedSquareDist<0,100,100>(pixelYCrCb, targetColor);
			if(val < 1600) {
				return pixel;
			}
			return cv::Vec3b{0, 0, 0};
		}
	);
}

std::map<ctn::VertexCoord, ctn::Settlement> ctn::FindSettlements(const BoardIR& boardIR)
{
	static cv::Vec3b sand = cvutil::YCrCbOf({127, 180, 181});

	static cv::Vec3b cityRed = cvutil::YCrCbOf({75, 0, 146});
	static cv::Vec3b cityBlue = cvutil::YCrCbOf({100, 52, 15});
	static cv::Vec3b cityOrange = cvutil::YCrCbOf({25, 136, 208});
	
	std::map<ctn::VertexCoord, ctn::Settlement> result;

	for(const auto& [coord, corner]: boardIR.corners) {
		auto cornerYCrCb = cvutil::Convert(corner, cv::COLOR_BGR2YCrCb);
		auto rating = rateChunk(BuildingInverseLikelihood(corner));

		if(rating < 3000) {

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
				.type=SettlementType::Settlement, 
				.color=colors[idx]
			};
		}
	}

	return result;
}
