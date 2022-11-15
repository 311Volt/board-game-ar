#include <opencv2/opencv.hpp>

#include <catan/analysis_settlements.hpp>
#include <catan/utility_opencv.hpp>
#include <catan/common_math.hpp>

static constexpr float INV_255 = 1.0f / 255.0f;

float rating(float x)
{
    return 0.5f - std::atan(4.0f*x - 5) / 3.141591f;
}

cv::Mat FilterLikelyBuildings(cv::Mat warped)
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

float rateChunk(cv::Mat chunk)
{
	cv::Vec2i center {24,24};
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


std::map<ctn::VertexCoord, ctn::Settlement> ctn::FindSettlements(const BoardIR& boardIR)
{
	return {};
}
