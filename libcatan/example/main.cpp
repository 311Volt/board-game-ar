#include <catan.hpp>

#include <fmt/format.h>


void DrawCellTypes(cv::Mat board, const ctn::BoardInfo& boardInfo)
{
	ctn::ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});

	for(const auto& [coord, type]: boardInfo.cellTypes) {
		auto pos = mapper(coord) - cv::Point2d{40, 0};
		auto color = cv::Scalar{0, 255, 0};
		cv::putText(board, type, pos, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.9, color);
	}
}


void DrawSettlements(cv::Mat board, const ctn::BoardInfo& boardInfo)
{
	ctn::ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});

	for(const auto& [coord, settlement]: boardInfo.settlements) {
		cv::circle(board, mapper(coord), 9, ctn::PlayerColorBGR(settlement.color), -1);
		cv::putText(
			board, 
			(settlement.type == ctn::SettlementType::City ? "city" : "settlement"), 
			mapper(coord), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, 
			{255,255,255}
		);
	}
}

void DrawRoads(cv::Mat board, const ctn::BoardInfo& boardInfo)
{
	ctn::ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});

	for(const auto& [coord, road]: boardInfo.roads) {
		cv::circle(board, mapper(coord), 7, {255,255,255}, -1);
		cv::circle(board, mapper(coord), 4, ctn::PlayerColorBGR(road.color), -1);
	}
}

double GetTime()
{
	static auto t0 = std::chrono::high_resolution_clock::now();
	auto t1 = std::chrono::high_resolution_clock::now();
	return 1e-9 * std::chrono::duration_cast<std::chrono::nanoseconds>(t1-t0).count();
}

void msg(const std::string& msg)
{
	fmt::print("[{:.6f}] {}\n", GetTime(), msg);
}

int main()
{
	msg("init");
	auto src = cvutil::SafeImread("resources/samples/sample1.jpg");
	CatanBoardDetector detector {SEA_COLOR_YCBCR_6500K};

	msg("detector initialized");

	auto warpedOpt = detector.findBoard(src);
	msg("findBoard finished");
	if(!warpedOpt.has_value()) {
		std::cerr << "error: board not found\n";
		return 1;
	}
	cv::Mat warped = warpedOpt.value();
	msg("board detected");

	ctn::BoardIR boardIR = ctn::CreateBoardIR(warped);

	msg("board IR created");
	ctn::BoardInfo boardInfo = ctn::AnalyzeBoard(boardIR);

	msg("board info created");

	DrawCellTypes(warped, boardInfo);
	DrawSettlements(warped, boardInfo);
	DrawRoads(warped, boardInfo);

	msg("info drawn");
	
	cv::imshow("Warped board", warped);
	cv::waitKey();
}
