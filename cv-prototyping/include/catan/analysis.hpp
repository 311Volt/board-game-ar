#ifndef INCLUDE_CATAN_ANALYSIS
#define INCLUDE_CATAN_ANALYSIS

#include <catan/board_coords.hpp>
#include <catan/analysis_cells.hpp>
#include <catan/analysis_settlements.hpp>

namespace ctn {

	struct BoardInfo {
		std::map<CellCoord, std::string> cellTypes;
		std::map<CellCoord, int> cellNumbers;
		std::map<VertexCoord, Settlement> settlements;
		//std::map<EdgeCoord, Road> roads;
		CellCoord robberPosition;
	};

	//TODO: jobject* CreateBoardInfoJavaObject(const BoardInfo& boardInfo);

	BoardInfo AnalyzeBoard(const BoardIR& boardIR);

};

#endif /* INCLUDE_CATAN_ANALYSIS */
