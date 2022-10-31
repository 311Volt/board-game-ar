
#include <opencv2/opencv.hpp>

#include <catan/board_coords.hpp>
#include <catan/board_detection.hpp>
#include <catan/image_correction.hpp>
#include <catan/utility_opencv.hpp>
#include <catan/board_ir.hpp>
#include <catan/common_math.hpp>
#include <catan/analysis_cells.hpp>

#include <fmt/format.h>

double GetTime()
{
	static auto t0 = std::chrono::high_resolution_clock::now();
	auto dif = std::chrono::high_resolution_clock::now() - t0;
	return 0.000000001 * std::chrono::duration_cast<std::chrono::nanoseconds>(dif).count();
}


template<>
struct fmt::formatter<ctn::CellCoord>
{
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(const ctn::CellCoord& x, FormatContext& ctx)
	{
		return fmt::format_to(ctx.out(), "({},{},{})", x.x,x.y,x.z);
	}
};


int main()
{
	auto src = cv::imread("resources/sample.jpg");
	CatanBoardDetector detector {SEA_COLOR_YCBCR_6500K};

	double t0,t1;
	t0=GetTime();
	auto warpedOpt = detector.findBoard(src);
	if(!warpedOpt.has_value()) {
		std::cerr << "error: board not found\n";
		return 1;
	}
	cv::Mat warped = warpedOpt.value();
	t1=GetTime();

	fmt::print("finding board: {:.6f} secs\n", t1-t0);

	ctn::BoardIR boardIR = ctn::CreateBoardIR(warped);
	cv::Mat cellMask = ctn::GenerateCellMask(50, 1000);
	std::vector<std::pair<ctn::CellCoord, cv::Mat>> cells {boardIR.cells.begin(), boardIR.cells.end()};

	cv::Mat maskU8 = NEW_MAT(tmp) {cv::cvtColor(cvutil::ToByte(cellMask), tmp, cv::COLOR_GRAY2BGR);};

	std::array<std::string, 6> cellTypeNames = {
		"desert", "fields", "forest", "hills", "mountains", "pasture"
	};

	std::array<cv::Mat, 6> refCells, refCells32;
	
	for(int i=0; i<refCells.size(); i++) {
		refCells[i] = cv::imread(fmt::format("resources/cells/{}.jpg", cellTypeNames[i]));
		cvmath::ApplyBin<cv::Vec3b>(refCells[i], maskU8, cvmath::Mask8UC3);
		refCells32[i] = cvutil::ToFloat(refCells[i]);
	}
	ctn::ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});
	
	t0=GetTime();

	for(auto& [coord, img]: boardIR.cells) {
		cv::Mat maskedImg = cvmath::TransformBin<cv::Vec3b, cv::Vec3b>(img, maskU8, cvmath::Mask8UC3);
		cv::Mat maskedImg32 = cvutil::ToFloat(maskedImg);
		
		std::array<float, 6> results;
		results[0] = 10e40;
		for(int i=1; i<refCells.size(); i++) {
			cv::Mat sqDiff = cvmath::TransformBin<cv::Vec3f, float>(cvutil::convertToCrCb(maskedImg32), cvutil::convertToCrCb(refCells32[i]), cvmath::SquareDist);
			results[i] = cv::sum(sqDiff)[0];
		}

		auto min = std::min_element(results.begin(), results.end());
		int idx = min - results.begin();
		if(coord == ctn::CellCoord{0,0,0}) {
			idx = 0;
			continue;
		}



		cv::Mat& ximg = img;
		cv::Mat imgGray = NEW_MAT(tmp) {cv::cvtColor(ximg, tmp, cv::COLOR_BGR2GRAY);};
		std::vector<cv::Vec3f> circles;
		cv::HoughCircles(imgGray, circles, cv::HOUGH_GRADIENT, 1, 90, 100, 15, 21, 27);
		
		if(circles.size() == 1) {
			double radius = circles[0][2];
			cv::Point2d center {circles[0][0], circles[0][1]};
			auto origin = mapper(coord) - cv::Point2d{65, 75};
			cv::Point2d offset {radius, radius};

			cv::Mat numberROI = img(cv::Rect(center - offset, center + offset)).clone();
			cv::circle(warped, center+origin, radius, {255,0,255}, 1);

			cv::imshow(fmt::format("numROI_{}", coord), numberROI);
		}


		cv::putText(warped, cellTypeNames[idx], mapper(coord), cv::FONT_HERSHEY_COMPLEX, 0.6, {0,255,0}, 2);

	}

	t1=GetTime();
	fmt::print("classifying: {:.6f} secs\n", t1-t0);


	cv::imshow("Warped board", warped);
	
	cv::waitKey();
}
