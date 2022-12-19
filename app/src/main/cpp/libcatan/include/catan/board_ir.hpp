#ifndef INCLUDE_CATAN_BOARD_IR
#define INCLUDE_CATAN_BOARD_IR

#include <catan/image_correction.hpp>
#include <catan/board_coords.hpp>

namespace ctn {

	/**
	 * @brief Intermediate representation of a Catan board.
	 */
	struct BoardIR {
		std::map<VertexCoord, cv::Rect> cornerRect;
		std::map<CellCoord, cv::Rect> cellRect;
		std::map<EdgeCoord, cv::RotatedRect> edgeRect;

		std::map<VertexCoord, cv::Mat> corners;
		std::map<CellCoord, cv::Mat> cells;
		std::map<EdgeCoord, cv::Mat> edges;
		
		cv::Vec3b roadColor;
		cv::Mat warpedBoard;
	};

	BoardIR CreateBoardIR(cv::Mat warpedBoard);
	cv::Vec3f GetRoadColor(cv::Mat warped, cv::Mat thresWarped = ctn::GenerateIdealEdgeMask({255,255,255}, 15));
}

#endif /* INCLUDE_CATAN_BOARD_IR */
