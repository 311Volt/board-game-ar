
#include <opencv2/opencv.hpp>

#include <catan/board_detection.hpp>
#include <catan/analysis.hpp>
#include <catan/utility_opencv.hpp>

#include <fmt/format.h>


void DrawCellTypes(cv::Mat board, const ctn::BoardInfo& boardInfo)
{
	ctn::ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});

	for(const auto& [coord, type]: boardInfo.cellTypes) {
		auto pos = mapper(coord) - cv::Point2d{40, 0};
		auto color = cv::Scalar{0, 255, 0};
		cv::putText(board, type, pos, cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color);
	}
}


void DrawSettlements(cv::Mat board, const ctn::BoardInfo& boardInfo)
{
	ctn::ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});

	for(const auto& [coord, settlement]: boardInfo.settlements) {
		cv::circle(board, mapper(coord), 9, ctn::PlayerColorBGR(settlement.color), -1);
	}
}


int main()
{
	auto src = cv::imread("resources/sample.jpg");
	CatanBoardDetector detector {SEA_COLOR_YCBCR_6500K};

	auto warpedOpt = detector.findBoard(src);
	if(!warpedOpt.has_value()) {
		std::cerr << "error: board not found\n";
		return 1;
	}
	cv::Mat warped = warpedOpt.value();

	ctn::BoardIR boardIR = ctn::CreateBoardIR(warped);
	
	ctn::BoardInfo boardInfo = ctn::AnalyzeBoard(boardIR);

	cv::imshow("Warped board, YUV", cvutil::Convert(warped, cv::COLOR_BGR2YCrCb));

	DrawCellTypes(warped, boardInfo);
	DrawSettlements(warped, boardInfo);

	cv::imshow("Warped board", warped);
	cv::waitKey();
}
