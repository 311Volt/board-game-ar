#include <catan.hpp>

ctn::BoardInfo ctn::AnalyzeBoard(const BoardIR &boardIR)
{
	BoardInfo result;
	result.cellTypes = DetermineCellTypes(boardIR);
	result.settlements = FindSettlements(boardIR);
	result.roads = FindRoads(boardIR);

	return result;
}

std::optional<ctn::BoardInfo> ctn::AnalyzeBoardImage(const cv::Mat &image)
{
	CatanBoardDetector detector {SEA_COLOR_YCBCR_6500K};
	auto optWarped = detector.findBoard(image);

	if(optWarped) {
		return AnalyzeBoard(CreateBoardIR(optWarped.value()));
	}
	return {};
}
