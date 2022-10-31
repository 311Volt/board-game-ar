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
