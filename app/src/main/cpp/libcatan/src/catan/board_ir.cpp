#include <catan.hpp>

#include <span>

cv::Rect makeRect(cv::Point2f rectCenter, cv::Vec2f rectSize)
{
	return {cv::Point2i{cv::Vec2f{rectCenter} - rectSize*0.5f}, cv::Size2i{(int)rectSize[0], (int)rectSize[1]}};
}

template<ctn::CoordType CoordT>
std::map<CoordT, cv::Mat> CreateImages(cv::Mat warpedBoard, std::span<CoordT> coords, cv::Vec2f imageSize)
{
	ctn::ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});

	std::map<CoordT, cv::Mat> ret;
	for(const auto& coord: coords) {
		ret[coord] = warpedBoard(makeRect(mapper(coord), imageSize));
	}
	return ret;
}

std::map<ctn::CellCoord, cv::Mat> createCellImages(cv::Mat warpedBoard, std::span<ctn::CellCoord> cellCoords)
{
	return CreateImages(warpedBoard, cellCoords, {130, 150});
}

std::map<ctn::VertexCoord, cv::Mat> createCornerImages(cv::Mat warpedBoard, std::span<ctn::VertexCoord> cornerCoords)
{
	return CreateImages(warpedBoard, cornerCoords, {72, 72});
}

std::map<ctn::EdgeCoord, cv::Mat> createEdgeImages(cv::Mat warpedBoard, std::span<ctn::EdgeCoord> edgeCoords)
{
	ctn::ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});

	std::map<ctn::EdgeCoord, cv::Mat> ret;
	for(const auto& coord: edgeCoords) {
		auto center = mapper(coord);
		cv::Size2f rectSize = {24, 72};
		float angle = -60.f * coord.side;
		ret[coord] = cvutil::CropRotatedRect(warpedBoard, cv::RotatedRect{center, rectSize, angle});
	}
	return ret;
}


ctn::BoardIR ctn::CreateBoardIR(cv::Mat warpedBoard)
{
	std::vector<CellCoord> cellCoords = GenerateCellCoords(2);
	std::vector<VertexCoord> cornerCoords = GenerateVertexCoords();
	std::vector<EdgeCoord> edgeCoords = GenerateEdgeCoords();

	return BoardIR {
		.corners = createCornerImages(warpedBoard, cornerCoords),
		.cells = createCellImages(warpedBoard, cellCoords),
		.edges = createEdgeImages(warpedBoard, edgeCoords)
	};

}