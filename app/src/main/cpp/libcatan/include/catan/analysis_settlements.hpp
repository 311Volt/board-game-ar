#ifndef INCLUDE_CATAN_ANALYSIS_SETTLEMENTS
#define INCLUDE_CATAN_ANALYSIS_SETTLEMENTS

#include <catan/board_ir.hpp>

namespace ctn {
	enum class PlayerColor: uint8_t {
		Red = 0,
		Blue = 1,
		Orange = 2,
		White = 3
	};

	inline cv::Scalar PlayerColorBGR(PlayerColor color)
	{
		switch (color) {
			case PlayerColor::Red: return {0,0,255};
			case PlayerColor::Blue: return {255,0,0};
			case PlayerColor::Orange: return {0,127,255};
			default: return {0,0,0};
		}
	}

	enum class SettlementType: uint8_t {
		Settlement = 0,
		City = 1
	};

	struct Settlement {
		SettlementType type;
		PlayerColor color;
	};

	struct Road {
		PlayerColor color;
	};

	std::map<VertexCoord, Settlement> FindSettlements(const BoardIR& boardIR);
	std::map<EdgeCoord, Road> FindRoads(const BoardIR& boardIR);
};

cv::Mat CreateStructureMask(const cv::Mat& warped, cv::Vec2f relativeCenter); //TODO DECLARATION FOR DEBUGGING ONLY - REMOVE LATER
cv::Mat CreateRoadMask(cv::Mat input);

#endif /* INCLUDE_CATAN_ANALYSIS_SETTLEMENTS */
