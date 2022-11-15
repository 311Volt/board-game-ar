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
	//std::map<EdgeCoord, Road> FindRoads(const BoardIR& boardIR);
};

#endif /* INCLUDE_CATAN_ANALYSIS_SETTLEMENTS */
