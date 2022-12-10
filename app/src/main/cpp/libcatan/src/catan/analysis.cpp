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

void ctn::DrawBoardInfo(const BoardInfo &boardInfo, cv::Mat &warpedBoard)
{
	DrawCellTypes(warpedBoard, boardInfo);
	DrawSettlements(warpedBoard, boardInfo);
	DrawRoads(warpedBoard, boardInfo);
}

std::string ctn::SerializeBoardInfo(const BoardInfo &boardInfo)
{
	auto cell2str = [](CellCoord x) {
		char buf[256];
		sprintf(buf, "%d %d %d", x.x, x.y, x.z);
		return std::string(buf);
	};

	auto vtx2str = [cell2str](VertexCoord x) {
		char buf[256];
		sprintf(buf, "%s / %s", cell2str(x.origin).c_str(), x.high ? "true" : "false");
		return std::string(buf);
	};

	auto edge2str = [cell2str](EdgeCoord x) {
		char buf[256];
		sprintf(buf, "%s / %d", cell2str(x.origin).c_str(), (int)x.side);
		return std::string(buf);
	};


	auto pc2str = [](PlayerColor r) {
		switch (r) {
			case PlayerColor::Red: return "red";
			case PlayerColor::Blue: return "blue";
			case PlayerColor::Orange: return "orange";
            default: break;
		}
		return "";
	};

	std::string outputData;
	outputData += "cells\n";
	for(const auto& [cellCoord, cellType]: boardInfo.cellTypes) {
		outputData += cell2str(cellCoord) + " : " + cellType + "\n";
	}
	outputData += "roads\n";
	for(const auto& [edgeCoord, road]: boardInfo.roads) {
		outputData += edge2str(edgeCoord) + " : " + pc2str(road.color) + "\n";
	}
	outputData += "settlements\n";
	for(const auto& [vertexCoord, settlement]: boardInfo.settlements) {
		outputData
           += vtx2str(vertexCoord) + " : "
            + pc2str(settlement.color) + " "
            + ((settlement.type == SettlementType::City) ? "true" : "false")
            + "\n";
	}
    return outputData;

}