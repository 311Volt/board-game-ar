#ifndef INCLUDE_CATAN_BOARD_IR
#define INCLUDE_CATAN_BOARD_IR

#include <catan/board_coords.hpp>

namespace ctn {

	/**
	 * @brief Intermediate representation of a Catan board.
	 */
	struct BoardIR {
		std::map<VertexCoord, cv::Mat> corners;
		std::map<CellCoord, cv::Mat> cells;
		//std::map<EdgeCoord, cv::Mat> edges;
	};

	BoardIR CreateBoardIR(cv::Mat warpedBoard);
}

#endif /* INCLUDE_CATAN_BOARD_IR */
