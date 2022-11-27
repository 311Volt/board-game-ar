#include <catan/board_coords.hpp>

#include <numbers>

cv::Point2d cis(double theta)
{
	return { std::cos(theta), -std::sin(theta) };
}
long double operator""_deg(long double x)
{
	return x * std::numbers::pi / 180.0;
}
long double operator""_deg(unsigned long long x)
{
	return x * std::numbers::pi / 180.0;
}

std::pair<ctn::VertexCoord, ctn::VertexCoord> ctn::EdgeCoord::asVertexCoordPair() const
{
	ctn::VertexCoord ret[4];
	ctn::CellCoord origins[3] = {
		{origin.x, origin.y-1, origin.z+1},
		{origin.x, origin.y, origin.z},
		{origin.x-1, origin.y+1, origin.z}
	};
	for(int i=0; i<4; i++) {
		ret[i] = {.origin = origins[(i+1)/2], .high = ((i&1) == 0)};
	}
	return std::make_pair(ret[side+1], ret[side+2]);
}

ctn::ScreenCoordMapper::ScreenCoordMapper(const HexGridView& view)
	: view {view}
{

}

cv::Point2d ctn::ScreenCoordMapper::operator()(const CellCoord& cell)
{
	static constexpr double M[] = { 1, 0.5, 0, -std::numbers::sqrt3 / 2.0 };
	return view.center + view.size * cv::Point2d{
		cell.x * M[0] + cell.y * M[1],
		cell.x * M[2] + cell.y * M[3]
	};
}
cv::Point2d ctn::ScreenCoordMapper::operator()(const VertexCoord& vtx)
{
	static constexpr double r = 1.0 / std::numbers::sqrt3;
	return (*this)(vtx.origin) + view.size * r * cis(vtx.high ? 30_deg : -30_deg);
}
cv::Point2d ctn::ScreenCoordMapper::operator()(const EdgeCoord& edge)
{
	static constexpr double r = 0.5;
	return (*this)(edge.origin) + view.size * r * cis(edge.side * 60_deg);
}


std::vector<ctn::CellCoord> ctn::GenerateCellCoords(int maxDepth)
{
	std::vector<ctn::CellCoord> result;
	for (int i = -maxDepth; i <= maxDepth; i++) {
		int lo = std::max(-maxDepth, -maxDepth - i);
		int hi = std::min(maxDepth, maxDepth - i);
		for (int j = lo; j <= hi; j++) {
			result.push_back({ i, j, -i - j });
		}
	}
	return result;
}

std::vector<ctn::VertexCoord> ctn::GenerateVertexCoords()
{
	auto fc = GenerateCellCoords(3);
	std::vector<ctn::VertexCoord> result;
	for (const auto& origin : fc) {
		if(origin.x < 3 && origin.y < 3 && origin.z > -3) {
			result.push_back(ctn::VertexCoord{.origin = origin, .high = true});
		}
		if(origin.x < 3 && origin.y > -3 && origin.z > -3) {
			result.push_back(ctn::VertexCoord{.origin = origin, .high = false});
		}
	}
	return result;
}

std::vector<ctn::EdgeCoord> ctn::GenerateEdgeCoords()
{
	auto fc = GenerateCellCoords(3);
	std::vector<ctn::EdgeCoord> result;
	for (const auto& origin : fc) {
		if ((origin.x < 3 && origin.y < 3 && origin.z > -3 && origin.x > -3)) {
			result.push_back(ctn::EdgeCoord{ .origin = origin, .side = 1 });
		}
		if (origin.x < 3 && origin.y > -3 && origin.z > -3 && origin.y < 3) {
			result.push_back(ctn::EdgeCoord{ .origin = origin, .side = 0 });
		}
		if (origin.x < 3 && origin.y > -3 && origin.z > -3 && origin.z < 3) {
			result.push_back(ctn::EdgeCoord{ .origin = origin, .side = -1 });
		}
	}
	return result;
}
