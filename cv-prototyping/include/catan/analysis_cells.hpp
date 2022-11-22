#ifndef INCLUDE_CATAN_ANALYSIS_CELLS
#define INCLUDE_CATAN_ANALYSIS_CELLS

#include <opencv2/opencv.hpp>

#include <catan/board_coords.hpp>
#include <catan/board_ir.hpp>

namespace ctn {
	std::map<CellCoord, std::string> DetermineCellTypes(const ctn::BoardIR& board);
};

#endif /* INCLUDE_CATAN_ANALYSIS_CELLS */
