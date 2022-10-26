
#include <opencv2/opencv.hpp>

#include <catan/board_coords.hpp>
#include <catan/board_detection.hpp>
#include <catan/image_correction.hpp>
#include <catan/utility_opencv.hpp>

#include <fmt/format.h>

static constexpr float INV_255 = 1.0f / 255.0f;

inline float sqDistUC3(cv::Vec3b a, cv::Vec3b b)
{
	cv::Vec3f af(a), bf(b);
	af *= INV_255;
	bf *= INV_255;

	return (af[0]-bf[0])*(af[0]-bf[0]) + (af[1]-bf[1])*(af[1]-bf[1]) + (af[2]-bf[2])*(af[2]-bf[2]);
}

cv::Vec3b ycrcbOf(cv::Vec3b bgr)
{
	cv::Mat xd = cv::Mat::zeros(1,1,CV_8UC3);
	xd.at<cv::Vec3b>(0,0) = bgr;
	cv::cvtColor(xd, xd, cv::COLOR_BGR2YCrCb);
	cv::Vec3b ret = xd.at<cv::Vec3b>(0,0);
	return ret;
}

cv::Vec3b crcbOf(cv::Vec3b bgr)
{
	cv::Vec3b ret = ycrcbOf(bgr);
	ret[0] = 0;
	return ret;
}

float rating(float x)
{
    return 0.5f - std::atan(4.0f*x - 5) / 3.141591f;
}

cv::Mat spookyShit(cv::Mat warped)
{
	static constexpr float LUMINANCE_WEIGHT = 0.3f;

	//cv::Vec3b shore = ycrcbOf({192, 170, 109});
	cv::Vec3b sand = ycrcbOf({139, 179, 178});
	cv::Vec3b gap = ycrcbOf({27, 59, 64});

	//shore[0] *= LUMINANCE_WEIGHT;
	sand[0] *= LUMINANCE_WEIGHT;

	cv::Mat output = cv::Mat::zeros(warped.rows, warped.cols, warped.depth());

	cv::Mat warpedCrCb = NEW_MAT(tmp) {cv::cvtColor(warped, tmp, cv::COLOR_BGR2YCrCb);};

	for(int y=0; y<warped.rows; y++) {
		cv::Vec3b* inRow = warpedCrCb.ptr<cv::Vec3b>(y);
		uint8_t* outRow = output.ptr<uint8_t>(y);
		for(int x=0; x<warped.cols; x++) {
			//float k1 = rating(sqDistUC3(shore, inRow[x]) * 64.0f);

			float k2 = rating(sqDistUC3(gap, inRow[x]) * 128.0f) * 0.7f;
			inRow[x][0] *= LUMINANCE_WEIGHT;
			float k1 = rating(sqDistUC3(sand, inRow[x]) * 128.0f);
			
			outRow[x] = std::max(k1, k2) * 255.0f;
		}
	}
	return output;
}

int main()
{
	auto src = cv::imread("resources/sampleGlare.jpg");
	CatanBoardDetector detector {SEA_COLOR_YCBCR_6500K};

	cv::Mat warped;
	try {
		warped = detector.findBoard(src).value();
	} catch(std::bad_optional_access& ex) {
		std::cerr << "error: board not found\n";
		return 1;
	}

	ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});

	/*for(auto& c: GenerateFieldCoords(3)) {
		cv::putText(warped, fmt::format("({},{},{})", c.x, c.y, c.z), mapper(c), cv::FONT_HERSHEY_PLAIN, 1, {255,255,255});
	}*/

	//drawPoints(mapper(GenerateFieldCoords(2)), warped, {255,255,180});

	cvutil::Window win1("warped board"), win2("chunk", {.waitKeyOnExit=true}), win3("proc");
	win1.show(warped);
	win3.show(spookyShit(warped));

	std::vector<VertexCoord> vertexCoords = GenerateVertexCoords();
	for (auto& c : vertexCoords) {
		//cv::putText(warped, fmt::format("({},{},{})", c.origin.x, c.origin.y, c.origin.z), mapper(c), cv::FONT_HERSHEY_PLAIN, 1, { 255,255,255 });
		auto m = mapper(c);

		float rectSize = 48;

		cv::Rect roi(m - cv::Point2d{0.5,0.5}*rectSize, cv::Size(rectSize, rectSize));
		cv::Mat destinationROI = warped(roi);
		cv::Mat flipped;
		cv::flip(destinationROI, flipped, 0);
		
		win2.show(destinationROI, {.waitKey=true});
	}

	//drawPoints(mapper(GenerateVertexCoords()), warped, {255,0,0});
	
	/*showScaled("Source", src);
	showScaled("CrCb", crcb);
	showScaled("sqDiff(CrCb, sea color)", sq);
	showScaled("Threshold", thres);*/

}
