#include <catan.hpp>

#include <span>

template<typename T>
concept CVRectType = std::same_as<T, cv::Rect> || std::same_as<T, cv::RotatedRect>;

cv::Rect makeRect(cv::Point2f rectCenter, cv::Vec2f rectSize)
{
	return {cv::Point2i{cv::Vec2f{rectCenter} - rectSize*0.5f}, cv::Size2i{(int)rectSize[0], (int)rectSize[1]}};
}

std::map<ctn::CellCoord, cv::Rect> MakeCellRectangles(std::span<ctn::CellCoord> cellCoords)
{
	ctn::ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});
	std::map<ctn::CellCoord, cv::Rect> ret;
	for(const auto& coord: cellCoords) {
		ret[coord] = makeRect(mapper(coord), {130, 150});
	}
	return ret;
} 

std::map<ctn::VertexCoord, cv::Rect> MakeCornerRectangles(std::span<ctn::VertexCoord> cornerCoords)
{
	ctn::ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});
	std::map<ctn::VertexCoord, cv::Rect> ret;
	for(const auto& coord: cornerCoords) {
		ret[coord] = makeRect(mapper(coord), {72, 72});
	}
	return ret;
} 


std::map<ctn::EdgeCoord, cv::RotatedRect> MakeEdgeRectangles(std::span<ctn::EdgeCoord> edgeCoords)
{
	ctn::ScreenCoordMapper mapper ({.center = {500,433}, .size = 150});
	std::map<ctn::EdgeCoord, cv::RotatedRect> ret;
	for(const auto& coord: edgeCoords) {
		auto center = mapper(coord);
		cv::Size2f rectSize = {24, 72};
		float angle = -60.f * coord.side;
		ret[coord] = cv::RotatedRect{center, rectSize, angle};
	}
	return ret;
} 

template<ctn::CoordType CoordT>
std::map<CoordT, cv::Mat> CreateImages(cv::Mat warpedBoard, std::map<CoordT, cv::Rect> rects)
{
	std::map<CoordT, cv::Mat> ret;
	for(const auto& [coord, rect]: rects) {
		ret[coord] = warpedBoard(rect);
	}
	return ret;
}


/*
std::map<ctn::CellCoord, cv::Mat> createCellImages(cv::Mat warpedBoard, std::span<cv::Rect> rectangles)
{
	return CreateImages(warpedBoard, cellCoords, {130, 150});
}

std::map<ctn::VertexCoord, cv::Mat> createCornerImages(cv::Mat warpedBoard, std::span<cv::Rect> rectangles)
{
	return CreateImages(warpedBoard, cornerCoords, {72, 72});
}
*/

std::map<ctn::EdgeCoord, cv::Mat> CreateEdgeImages(cv::Mat warpedBoard, std::map<ctn::EdgeCoord, cv::RotatedRect> edgeCoords)
{
	std::map<ctn::EdgeCoord, cv::Mat> ret;
	for(const auto& [coord, rect]: edgeCoords) {
		ret[coord] = cvutil::CropRotatedRect(warpedBoard, rect);
	}
	return ret;
}


cv::Vec3f ctn::GetRoadColor(cv::Mat warped, cv::Mat thresWarped)
{
	if(warped.cols != thresWarped.cols || warped.rows != thresWarped.rows) {
		throw std::runtime_error("non-matching dimensions");
	}
	if(warped.depth() != CV_32F) {
		warped = cvutil::ToFloat(warped);
	}

	std::vector<float> vb(1), vg(1), vr(1);

	for(int y=0; y<warped.rows; y+=4) {
		cv::Vec3f* imgRow = warped.ptr<cv::Vec3f>(y);
		uint8_t* thresRow = thresWarped.ptr<uint8_t>(y);
		for(int x=0; x<warped.cols; x+=4) {
			if(thresRow[x] > 200) {
				vb.push_back(imgRow[x][0]);
				vg.push_back(imgRow[x][1]);
				vr.push_back(imgRow[x][2]);
			}
		}
	}

	std::sort(vb.begin(), vb.end());
	std::sort(vg.begin(), vg.end());
	std::sort(vr.begin(), vr.end());
	
	return {vb[vb.size()/2] * 1.0f, vg[vg.size()/2] * 1.0f, vr[vr.size()/2] * 1.0f};
}


ctn::BoardIR ctn::CreateBoardIR(cv::Mat warpedBoard)
{
	std::vector<CellCoord> cellCoords = GenerateCellCoords(2);
	std::vector<VertexCoord> cornerCoords = GenerateVertexCoords();
	std::vector<EdgeCoord> edgeCoords = GenerateEdgeCoords();

	auto cellRect = MakeCellRectangles(cellCoords);
	auto cornerRect = MakeCornerRectangles(cornerCoords);
	auto edgeRect = MakeEdgeRectangles(edgeCoords);

	return BoardIR {
		.cornerRect = cornerRect,
		.cellRect = cellRect,
		.edgeRect = edgeRect,
		.corners = CreateImages(warpedBoard, cornerRect),
		.cells = CreateImages(warpedBoard, cellRect),
		.edges = CreateEdgeImages(warpedBoard, edgeRect),
		.roadColor = GetRoadColor(warpedBoard),
		.warpedBoard = warpedBoard
	};

}
