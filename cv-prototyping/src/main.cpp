
#include <opencv2/core.hpp>

#include <catan/board_coords.hpp>
#include <catan/board_detection.hpp>
#include <catan/image_correction.hpp>
#include <catan/utility_opencv.hpp>

#include <fmt/format.h>

cv::Mat loadYCross()
{
	cv::Mat ycross = cv::imread("resources/y-cross.png");
	fmt::print("sum {}\n\n", cv::sum(ycross)[0]);
	ycross /= cv::sum(ycross) / 25.0;
	return ycross;
}

cv::Mat convolutionWithCross(cv::Mat input)
{
	static cv::Mat ycross = loadYCross();
	cv::Mat inputGray;
	cv::cvtColor(input, inputGray, cv::COLOR_BGR2GRAY);
	cv::Mat out;
	cv::filter2D(inputGray, out, -1, ycross, {-1,-1}, -255, cv::BORDER_CONSTANT);
	return out;
}

int main()
{
	auto src = cv::imread("resources/sampleGrayBG.jpg");
	CatanBoardDetector detector {SEA_COLOR_YCBCR_6500K};
	cv::Mat warped;
	try {
		warped = detector.findBoard(src).value();
	} catch(std::bad_optional_access& ex) {
		std::cerr << "error: board not found";
		return 1;
	}

	ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});

	/*for(auto& c: GenerateFieldCoords(3)) {
		cv::putText(warped, fmt::format("({},{},{})", c.x, c.y, c.z), mapper(c), cv::FONT_HERSHEY_PLAIN, 1, {255,255,255});
	}*/

	//drawPoints(mapper(GenerateFieldCoords(2)), warped, {255,255,180});

	std::vector<VertexCoord> vertexCoords = GenerateVertexCoords();
	for (auto& c : vertexCoords) {
		//cv::putText(warped, fmt::format("({},{},{})", c.origin.x, c.origin.y, c.origin.z), mapper(c), cv::FONT_HERSHEY_PLAIN, 1, { 255,255,255 });
		auto m = mapper(c);

		float rectSize = 48;

		cv::Rect roi(m - cv::Point2d{0.5,0.5}*rectSize, cv::Size(rectSize, rectSize));
		cv::Mat destinationROI = warped(roi);

		cv::imshow("bruh", convolutionWithCross(destinationROI));
		cv::waitKey();
	}

	//drawPoints(mapper(GenerateVertexCoords()), warped, {255,0,0});
	
	/*showScaled("Source", src);
	showScaled("CrCb", crcb);
	showScaled("sqDiff(CrCb, sea color)", sq);
	showScaled("Threshold", thres);*/

	cv::imshow("Warped", warped);
	cv::waitKey();
}
