#ifndef INCLUDE_CATAN_BOARD_COORDS
#define INCLUDE_CATAN_BOARD_COORDS

#include <opencv2/opencv.hpp>

#include <concepts>
#include <fmt/format.h>

namespace ctn {

	/* 
	* Maps a hexagonal cell to coordinates (x,y,z), where x+y+z=0
	*/
	struct CellCoord: public cv::Point3i {
		using cv::Point3i::Point3i;

		inline auto operator<=>(const CellCoord& b) const
		{
			return std::make_tuple(x,y,z) <=> std::make_tuple(b.x, b.y, b.z);
		}
	};


	/* 
	* Uniquely describes each vertex on a hexagonal grid and relates it
	* to an "origin" cell, whose rightmost edge the vertex belongs to.
	*/
	struct VertexCoord {
		CellCoord origin;
		bool high;

		inline auto operator<=>(const VertexCoord& b) const
		{
			if(origin != b.origin) {
				return origin <=> b.origin;
			}
			return high <=> b.high;
		}
	};


	/* 
	* Uniquely describes each edge on a hexagonal grid and relates it
	* to an "origin" cell, whose right side the edge belongs to.
	*/
	struct EdgeCoord {
		CellCoord origin;
		short side; // -1 - top, 1 - bottom, 0 - straight

		inline auto operator<=>(const EdgeCoord& b) const
		{
			if (origin != b.origin) {
				return origin <=> b.origin;
			}
			return side <=> b.side;
		}
	};

	template<typename T>
	concept CoordType = std::same_as<T, CellCoord> || std::same_as<T, VertexCoord> || std::same_as<T, EdgeCoord>;


	struct HexGridView {
		cv::Point2d center;
		double size;
	};


	/* 
	* Maps hex grid coordinates to screen coordinates.
	*/
	class ScreenCoordMapper {
		HexGridView view;
		
	public:
		ScreenCoordMapper(const HexGridView& view);

		cv::Point2d operator()(const CellCoord& cell);
		cv::Point2d operator()(const VertexCoord& vtx);
		cv::Point2d operator()(const EdgeCoord& vtx);

		template<CoordType T>
		std::vector<cv::Point2d> operator()(const std::vector<T>& v)
		{
			std::vector<cv::Point2d> ret;
			for(auto& p: v) {
				ret.push_back((*this)(p));
			}
			return ret;
		}
	};



	std::vector<CellCoord> GenerateCellCoords(int maxDepth);
	std::vector<VertexCoord> GenerateVertexCoords();
	std::vector<EdgeCoord> GenerateEdgeCoords();

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


template<>
struct fmt::formatter<ctn::VertexCoord>
{
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(const ctn::VertexCoord& x, FormatContext& ctx)
	{
		return fmt::format_to(ctx.out(), "({},{},{} {})", x.origin.x,x.origin.y,x.origin.z,x.high ? "high" : "low");
	}
};


template<>
struct fmt::formatter<ctn::EdgeCoord>
{
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(const ctn::EdgeCoord& x, FormatContext& ctx)
	{
		return fmt::format_to(ctx.out(), "({},{},{} side={})", x.origin.x,x.origin.y,x.origin.z,x.side);
	}
};


#endif /* INCLUDE_CATAN_BOARD_COORDS */
